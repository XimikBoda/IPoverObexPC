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

	TCPsockets[owner.id].init(&owner.writer, owner.makeTypeId(owner.TCP_SOCK_T, owner.id), receive_buf);
	TCPsockets[owner.id].connect(adders, port);
}

void TCP::parseTCPSendPacket() {
	vec buf = reader.readToEnd();
	TCPsockets[owner.id].send(buf);
}

void TCP::parseTCPReceivePacket() {
	size_t receive = reader.readVarInt();
	TCPsockets[owner.id].receive(receive);
}

void TCP::parseTCPDisconnectPacket() {
	TCPsockets[owner.id].disconnect();
}

void TCP::parseTCPLPacket() {
	uint8_t act = reader.readUInt8();

	switch (act) {
	case TCPListener::Bind:
		parseTCPLBindPacket();
		break;
	case TCPListener::Accept:
		parseTCPSendPacket();
		break;
	case TCPListener::Close:
		//parseTCPSendPacket();
		break;
	}
}

void TCP::parseTCPLBindPacket() {
	uint16_t port = reader.readUInt16();

	TCPlisteners[owner.id].init(&owner.writer, owner.makeTypeId(owner.TCP_SOCK_T, owner.id));
	TCPlisteners[owner.id].bind(port);
}

void TCP::parseTCPLAcceptPacket() {
	uint16_t id = reader.readUInt16();

	TCPlisteners[owner.id].accept(TCPsockets[id]);
}

void TCP::parseTCPLClosePacket() {
	TCPlisteners[owner.id].close();
}