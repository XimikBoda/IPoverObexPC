#include "OBEXServer.h"
#include "OBEX.h"

void OBEXServer::getPacketType()
{
	reader.read_count = 0;
	current_pack = reader.readUInt8();
	current_pack_size = reader.readUInt16();
}

void OBEXServer::readPacket() {
	uint8_t hi_current_pack = current_pack | (1 << 7);
	switch (hi_current_pack) {
	case OBEX_Operations::Connect:
		readConnectPacket();
		break;
	case OBEX_Operations::Put:
		readPutPacket();
		break;
	}
}

void OBEXServer::readConnectPacket() {
	version = reader.readUInt8();
	flags = reader.readUInt8();
	max_pack_size = reader.readUInt16();

	makeConnectSuccessResponse();
	state = Connected;

	readHeaders();
}

void OBEXServer::readPutPacket() {
	readHeaders();

	makeConnectSuccessResponse();
	state = GettingFile;
}

void OBEXServer::readHeaders() {
	while (reader.read_count != current_pack_size) {
		readHeader();
	}
}

void OBEXServer::readHeader() {
	uint8_t header_id = reader.readUInt8();
	uint8_t headerType = ((header_id >> 6) & 0b11);

	switch (headerType) {
	case TextH: {
		uint16_t size = reader.readUInt16();
		vec str_buf = reader.readVecBlocking(size);
		break;
	}
	case BytesH: {
		uint16_t size = reader.readUInt16();
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

void OBEXServer::makeConnectSuccessResponse() {
	out_pack.init(Success);
	out_pack.putUInt8(version);
	out_pack.putUInt8(0x00);
	out_pack.putUInt16(max_pack_size);
	out_pack.send(writer);
}

void OBEXServer::makePutContinueResponse() {
	out_pack.init(Continue);
	out_pack.send(writer);
}

void OBEXServer::skipPacketToEnd() {
	reader.skipToEnd(current_pack_size);
	current_pack = 0;
}

void OBEXServer::run()
{
	while (1) {
		getPacketType();
		readPacket();
	}
}
