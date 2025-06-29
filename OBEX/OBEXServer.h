#pragma once
#include <vector>
#include <StreamAgent.h>
#include "Reader.h"
#include "OBEXMakePacket.h"
#include "OBEX.h"

#include <thread>

class OBEXServer {
	uint8_t current_pack = 0;
	uint16_t current_pack_size = 0;

	uint8_t version = 0x10;
	uint8_t flags = 0;
	uint16_t max_pack_size = 0x2800;

	OBEX_States state = Inited;

	std::shared_ptr<std::thread> worker_thr;

	void getPacketType();
	void readPacket();

	void readConnectPacket();
	void readPutPacket();
	void readDisconnectPacket();

	void readHeaders();
	void readHeader();

	void readStream();

	void makeConnectSuccessResponse();
	void makePutContinueResponse();
	void makePutFinalResponse();
	void makeDisconnectSuccessResponse();

	void skipPacketToEnd();

	void worker();

public:
	Reader reader;
	OBEXMakePacket writer;
	DS::StreamAgent stream_writer;

	void run();
	void wait();

	~OBEXServer();
};