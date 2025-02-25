#include "StreamToIP.h"

uint16_t StreamToIP::getType(uint16_t type_id) {
	return type_id >> bits_for_type;
}

uint16_t StreamToIP::getId(uint16_t type_id) {
	return type_id & ((1 << bits_for_type) - 1);
}

uint16_t StreamToIP::makeTypeId(uint16_t type, uint16_t id) {
	return (type << bits_for_type) | (id & ((1 << bits_for_type) - 1));
}

void StreamToIP::makeRspGeneral(uint16_t type_id, uint8_t act, uint8_t rsp) {

}

void StreamToIP::parsePacket() {
	size = reader.readVarInt();
	uint16_t type_id = reader.readUInt16();
	type = getType(type_id);
	id = getId(type_id);

	switch (type) {
	case TCP_T:
		parseTCPPacket();
		break;
	default:
		break;
	}
}

void StreamToIP::parseTCPPacket() {
	uint8_t act = reader.readUInt8();

	switch (act) {
	case TCP::Connect:
		parseTCPConnectPacket();
		break;
	case TCP::Disconnect:
		parseTCPConnectPacket();
		break;
	default:
		break;
	}
}

void StreamToIP::parseTCPConnectPacket() {
	std::string adders = reader.readString();
	uint16_t port = reader.readUInt16();

	uint16_t id = this->id;
	
	TCPs[id].connect(adders, port, [&, id](uint8_t res) {
		makeRspGeneral(makeTypeId(TCP_T, id), TCP::Connect, res);
		});
}

void StreamToIP::parseTCDisconnectPacket() {
	std::string adders = reader.readString();
	uint16_t port = reader.readUInt16();

	TCPs[id].disconnect();
}
