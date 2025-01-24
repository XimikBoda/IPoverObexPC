#pragma once
#include <vector>
#include "ReaderFromFunc.h"
#include "OBEXMakePacket.h"
#include "OBEX.h"


class OBEXClient {
	uint8_t last_pack = 0;
	uint8_t current_response_pack = 0;
	uint16_t current_response_pack_size = 0;

	uint8_t rversion = 0x10;
	uint8_t rflags = 0;
	uint16_t max_pack_size = 0xFFFF;

	OBEXMakePacket out_pack;

	void makeConnectPacket();

	void readResponsePacket();

	void readConnectResponsePacket();

	void readHeaders();
	void readHeader();


	void skipPacketToEnd();
public:
	ReaderFromFunc reader;
	WriteFunc writer;

	void connet();
};