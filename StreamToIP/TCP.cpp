#include "TCP.h"
#include "StreamToIP.h"
#include <iostream>

TCP::TCP(StreamToIP& owner_) : owner(owner_), reader(owner.reader) {}

void TCP::parseTCPPacket() {
	uint8_t act = reader.readUInt8();

	switch (act) {
	case TCPSock::Connect:
		parseTCPConnectPacket();
		break;
	case TCPSock::Send:
		parseTCPSendPacket();
		break;
	case TCPSock::Receive:
		parseTCPReceivePacket();
		break;
	case TCPSock::Disconnect:
		parseTCPDisconnectPacket();
		break;
	default:
		break;
	}
}

void TCP::parseTCPConnectPacket() {
	std::string adders = reader.readString();
	uint16_t port = reader.readUInt16();
	size_t receive_buf = reader.readVarInt();

	TCPs[owner.id].init(&owner.writer, owner.makeTypeId(owner.TCP_SOCK_T, owner.id), receive_buf);
	TCPs[owner.id].connect(adders, port);
}

void TCP::parseTCPSendPacket() {
	vec buf = reader.readVecBlocking(owner.size - 5);
	TCPs[owner.id].send(buf);
}

void TCP::parseTCPReceivePacket() {
	size_t receive = reader.readVarInt();
	TCPs[owner.id].receive(receive);
}

void TCP::parseTCPDisconnectPacket() {
	TCPs[owner.id].disconnect();
}
