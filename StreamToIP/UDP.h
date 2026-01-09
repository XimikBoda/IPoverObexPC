#pragma once
#include "UDPSock.h"
#include "PacketMaker.h"
#include "PacketReader.h"

#include <cstdint>
#include <memory>
#include <atomic>
#include <thread>

#include <Stream.h>


class UDP {
	friend class StreamToIP;

	class StreamToIP& owner;
	PacketReader &reader;

	std::map<uint16_t, UDPSock> UDPsockets;

	UDP(class StreamToIP& owner_);

	void parseUDPPacket();
	void parseUDPBindPacket();
	void parseUDPSendPacket();
	void parseUDPReceivePacket();
	void parseUDPUnbindPacket();
};