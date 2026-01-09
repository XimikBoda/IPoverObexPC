#include "UDP.h"
#include "StreamToIP.h"
#include <iostream>

UDP::UDP(StreamToIP& owner_) : owner(owner_), reader(owner.reader) {}

void UDP::parseUDPPacket() {
	uint8_t act = reader.readUInt8();

	switch (act) {
	case UDPSock::Bind:
		parseUDPBindPacket();
		break;
	case UDPSock::Send:
		parseUDPSendPacket();
		break;
	case UDPSock::Receive:
		parseUDPReceivePacket();
		break;
	case UDPSock::Unbind:
		parseUDPUnbindPacket();
		break;
	default:
		break;
	}
}

void UDP::parseUDPBindPacket() {
	uint16_t port = reader.readUInt16();
	size_t receive_buf = reader.readVarInt();

	UDPsockets[owner.id].bind(&owner.writer, owner.makeTypeId(owner.TCP_SOCK_T, owner.id), receive_buf, port);
}

void UDP::parseUDPSendPacket() {
	uint32_t ip = reader.readUInt32();
	uint16_t port = reader.readUInt16();
	vec buf = reader.readToEnd();

	UDPsockets[owner.id].send(buf, sf::IpAddress(ip), port);
}

void UDP::parseUDPReceivePacket() {
	size_t receive = reader.readVarInt();
	UDPsockets[owner.id].receive(receive);
}

void UDP::parseUDPUnbindPacket() {
	UDPsockets[owner.id].unbind();
	UDPsockets.erase(owner.id);
}
