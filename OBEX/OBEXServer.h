#pragma once
#include <vector>
#include "ReaderFromFunc.h"
#include "OBEXMakePacket.h"
#include "OBEX.h"


class OBEXServer {
	uint8_t current_pack = 0;
	uint16_t current_pack_size = 0;

	uint8_t version = 0x10;
	uint8_t flags = 0;
	uint16_t max_pack_size = 0xFFFF;

	OBEX_States state = Inited;

	OBEXMakePacket out_pack;

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
	ReaderFromFunc reader;
	WriteFunc writer;
	WriteFunc stream_writer;

	void run();
};