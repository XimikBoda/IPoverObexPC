#include "OBEXClient.h"
#include "OBEX.h"

void OBEXClient::makeConnectPacket() {
	out_pack.init(last_pack = Connect);
	out_pack.putUInt8(0x10);
	out_pack.putUInt8(0x00);
	out_pack.putUInt16(max_pack_size);
	out_pack.send(writer);
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