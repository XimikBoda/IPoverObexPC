#pragma once
#include <vector>
#include "Reader.h"
#include "OBEXMakePacket.h"
#include "OBEX.h"

typedef std::function<void(vec)> WriteFunc; // temp

class OBEXServer {
	uint8_t current_pack = 0;
	uint16_t current_pack_size = 0;

	uint8_t version = 0x10;
	uint8_t flags = 0;
	uint16_t max_pack_size = 0xFFFF;

	OBEX_States state = Inited;


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

public:
	Reader reader;
	OBEXMakePacket writer;
	WriteFunc stream_writer;

	void run();
};