#pragma once
#include "TCPSock.h"
#include "PacketMaker.h"
#include "ReaderFromBuf.h"

#include <cstdint>
#include <memory>
#include <atomic>
#include <thread>

#include <Stream.h>


class TCP {
	friend class StreamToIP;

	class StreamToIP& owner;
	ReaderFromBuf &reader;

	std::map<uint16_t, TCPSock> TCPs;

	TCP(class StreamToIP& owner_);

	void parseTCPPacket();
	void parseTCPConnectPacket();
	void parseTCPSendPacket();
	void parseTCPReceivePacket();
	void parseTCPDisconnectPacket();

};