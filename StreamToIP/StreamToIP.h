#pragma once
#include <cstdint>
#include <map>

#include "ReaderFromBuf.h"
#include "PacketMaker.h"
#include "TCP.h"


class StreamToIP {
	const uint8_t bits_for_type = 11;

	enum Types : uint8_t{
		TCP_T
	};


	std::map<uint16_t, TCP> TCPs;

	uint32_t size;
	uint16_t type;
	uint16_t id;

	uint16_t getType(uint16_t type_id);
	uint16_t getId(uint16_t type_id);
	uint16_t makeTypeId(uint16_t type, uint16_t id);

	void makeRspGeneral(uint16_t type_id, uint8_t act, uint8_t rsp);

	void parsePacket();

	void parseTCPPacket();
	void parseTCPConnectPacket();
	void parseTCDisconnectPacket();
public:
	ReaderFromBuf reader;
	PacketMaker writer;

	void run();
};