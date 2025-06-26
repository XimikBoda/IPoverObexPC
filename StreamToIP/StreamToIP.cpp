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

void StreamToIP::makeRspGeneral(uint16_t type_id, uint8_t act, uint8_t rsp) { //Do I need this
	std::lock_guard lg(writer.mutex);
	writer.init(type_id);
	writer.putUInt8(act);
	writer.putUInt8(rsp);
	writer.send();
}

void StreamToIP::parsePacket() {
	//size = reader.readVarInt(); // Do we nned this?
	size = reader.readUInt16();
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
	case TCP::Send:
		parseTCPSendPacket();
		break;
	case TCP::Receive:
		parseTCPReceivePacket();
		break;
	case TCP::Disconnect:
		parseTCPDisconnectPacket();
		break;
	default:
		break;
	}
}

void StreamToIP::parseTCPConnectPacket() {
	std::string adders = reader.readString();
	uint16_t port = reader.readUInt16();
	size_t receive_buf = reader.readVarInt();
	
	TCPs[id].init(&writer, makeTypeId(TCP_T, id), receive_buf);
	TCPs[id].connect(adders, port);
}

void StreamToIP::parseTCPSendPacket() {
	vec buf = reader.readVecBlocking(size - 5);
	TCPs[id].send(buf);
}

void StreamToIP::parseTCPReceivePacket() {
	size_t receive = reader.readVarInt();
	TCPs[id].receive(receive);
}

void StreamToIP::parseTCPDisconnectPacket() {
	TCPs[id].disconnect();
}

void StreamToIP::run() {
	while (1) {
		parsePacket();
	}
}