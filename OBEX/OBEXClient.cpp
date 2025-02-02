#include "OBEXClient.h"
#include "OBEX.h"

#define NO_HI(x) ((x) & 0b01111111)

void OBEXClient::makeConnectPacket() {
	out_pack.init(last_pack = Connect);
	out_pack.putUInt8(0x10);
	out_pack.putUInt8(0x00);
	out_pack.putUInt16(max_pack_size);
	out_pack.send(writer);
}

void OBEXClient::makeFistPutPacket(std::string name, uint32_t size) {
	last_pack = Put;
	out_pack.init(NO_HI(Put));
	makeTextHeader(Name, name);
	makeIntHeader(Length, size);
	out_pack.send(writer);
}

void OBEXClient::makePutPacket(vec buf) {
	last_pack = Put;
	out_pack.init(NO_HI(Put));
	makeBytesHeader(Body, buf);
	out_pack.send(writer);
}

void OBEXClient::makePutPacketAndResponce(vec buf) {
	makePutPacket(buf);
	readResponsePacket();
}

void OBEXClient::makeTextHeader(uint8_t hid, std::string text) {
	out_pack.putUInt8(hid);
	out_pack.putUInt16(3 + text.size() * 2 + 2);
	for (int i = 0; i < text.size(); ++i)
		out_pack.putUInt16(text[i]);
	out_pack.putUInt16(0);
}

void OBEXClient::makeBytesHeader(uint8_t hid, vec buf) {
	out_pack.putUInt8(hid);
	out_pack.putUInt16(3 + buf.size());
	out_pack.putBuf(buf);
}

void OBEXClient::makeByteHeader(uint8_t hid, uint8_t val) {
	out_pack.putUInt8(hid);
	out_pack.putUInt8(val);
}

void OBEXClient::makeIntHeader(uint8_t hid, uint32_t val) {
	out_pack.putUInt8(hid);
	out_pack.putUInt32(val);
}

void OBEXClient::readResponsePacket() {
	reader.read_count = 0;
	current_response_pack = reader.readUInt8();
	current_response_pack_size = reader.readUInt16();

	switch (last_pack | (1 << 7))
	{
	case Connect:
		readConnectResponsePacket();
		break;
	default:
		skipPacketToEnd();
		break;
	}
}

void OBEXClient::readConnectResponsePacket() {
	rversion = reader.readUInt8();
	rflags = reader.readUInt8();
	max_pack_size = reader.readUInt16();
	readHeaders();
}

void OBEXClient::readHeaders() {
	while (reader.read_count != current_response_pack_size) {
		readHeader();
	}
}

void OBEXClient::readHeader() {
	uint8_t header_id = reader.readUInt8();
	uint8_t headerType = ((header_id >> 6) & 0b11);

	switch (headerType) {
	case TextH: {
		uint16_t size = reader.readUInt16() - 3;
		vec str_buf = reader.readVecBlocking(size);
		break;
	}
	case BytesH: {
		uint16_t size = reader.readUInt16() - 3;
		vec buf = reader.readVecBlocking(size);
		break;
	}
	case ByteH: {
		uint8_t byte = reader.readUInt8();
		break;
	}
	case IntH: {
		uint32_t val = reader.readUInt32();
		break;
	}

	}
}

void OBEXClient::skipPacketToEnd() {
	reader.skipToEnd(current_response_pack_size);
	current_response_pack = 0;
}

void OBEXClient::connet() {
	makeConnectPacket();
	readResponsePacket();
}

void OBEXClient::initPutStream(std::string name, uint32_t size) {
	makeFistPutPacket(name, size);
	readResponsePacket();
}

void OBEXClient::PutStreamData(vec buf) {
	size_t max_buf_size = max_pack_size - 6;
	int count = buf.size() / max_buf_size;
	for (int i = 0; i < count; ++i)
		makePutPacketAndResponce(vec(buf.begin() + i * max_buf_size, buf.begin() + (i + 1) * max_buf_size));

	if (buf.size() % max_buf_size != 0)
		makePutPacketAndResponce(vec(buf.begin() + count * max_buf_size, buf.end()));
}