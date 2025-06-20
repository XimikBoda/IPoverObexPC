#pragma once
#include <vector>
#include <string>
#include "Reader.h"
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
	void makeFistPutPacket(std::string name, uint32_t size);
	void makePutPacket(vec buf);

	void makePutPacketAndResponce(vec buf);

	void makeTextHeader(uint8_t hid, std::string text);
	void makeBytesHeader(uint8_t hid, vec buf);
	void makeByteHeader(uint8_t hid, uint8_t val);
	void makeIntHeader(uint8_t hid, uint32_t val);

	void readResponsePacket();

	void readConnectResponsePacket();

	void readHeaders();
	void readHeader();


	void skipPacketToEnd();
public:
	Reader reader;
	WriteFunc writer;

	void connet();
	void initPutStream(std::string name, uint32_t size);
	void PutStreamData(vec buf);
};