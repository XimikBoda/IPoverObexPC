#pragma once
#include "TCPSock.h"
#include "TCPListener.h"
#include "PacketMaker.h"
#include "PacketReader.h"

#include <cstdint>
#include <memory>
#include <atomic>
#include <thread>

#include <Stream.h>


class TCP {
	friend class StreamToIP;

	class StreamToIP& owner;
	PacketReader &reader;

	std::map<uint16_t, TCPSock> TCPsockets;
	std::map<uint16_t, TCPListener> TCPlisteners;

	TCP(class StreamToIP& owner_);

	void parseTCPPacket();
	void parseTCPConnectPacket();
	void parseTCPSendPacket();
	void parseTCPReceivePacket();
	void parseTCPDisconnectPacket();

	void parseTCPLPacket(); //TCPL is TCP listeners
	void parseTCPLBindPacket();
	void parseTCPLAcceptPacket();
	void parseTCPLClosePacket();
};