#pragma once
#include <vector>
#include "ReaderFromFunc.h"
#include "OBEXMakePacket.h"


enum OBEXServerStates {
	Inited,
	Connected
};

class OBEXServer {
	uint8_t current_pack = 0;
	uint16_t current_pack_size = 0;

	uint8_t version = 0x10;
	uint8_t flags = 0;
	uint16_t max_pack_size = 0xFFFF;

	OBEXMakePacket out_pack;
public:
	ReaderFromFunc reader;
	WriteFunc writer;

	void getPacketType();
	void readPacket();

	void readConnectPacket();

	void makeConnectResponse();

	void skipPacketToEnd();

	void run();
};