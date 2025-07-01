#pragma once
#include <cstdint>

#include "ReaderFromBuf.h"
#include "PacketMaker.h"
#include "TCP.h"


class StreamToIP {
	friend class TCP;

	const uint8_t bits_for_type = 11;

	enum Types : uint8_t{
		TCP_SOCK_T,
		TCP_LISTENER_T
	};

	bool running = true;

	std::shared_ptr<std::thread> worker_thr;
	uint16_t type = 0;
	uint16_t id = 0;

	uint16_t getType(uint16_t type_id);
	uint16_t getId(uint16_t type_id);
	uint16_t makeTypeId(uint16_t type, uint16_t id);

	void makeRspGeneral(uint16_t type_id, uint8_t act, uint8_t rsp);

	void parsePacket();

	void worker();

	TCP tcp;
public:
	StreamToIP();

	ReaderFromBuf reader;
	PacketMaker writer;

	void run();
	void wait();

	~StreamToIP();
};