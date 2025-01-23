#include "OBEXServer.h"
#include "OBEX.h"

void OBEXServer::getPacketType()
{
	reader.read_count = 0;
	current_pack = reader.readUInt8();
	current_pack_size = reader.readUInt16();
}

void OBEXServer::readPacket() {
	switch (current_pack & (1 << 7)) {
	case Connect:
		readConnectPacket();
		break;
	}
}

void OBEXServer::readConnectPacket() {
	version = reader.readUInt8();
	flags = reader.readUInt8();
	max_pack_size = reader.readUInt16();

	makeConnectResponse();

	skipPacketToEnd();
}

void OBEXServer::makeConnectResponse() {
	out_pack.init(Success);
	out_pack.putUInt8(version);
	out_pack.putUInt8(0x00);
	out_pack.putUInt16(max_pack_size);
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
