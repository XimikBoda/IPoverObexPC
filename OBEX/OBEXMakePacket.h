#pragma once
#include <vector>
#include <functional>
#include <cstdint>
#include <cstring>

#include <WriterAgent.h>

class OBEXMakePacket : public DS::WriterAgent {
	friend class OBEXClient;
	friend class OBEXServer;

	vec buf;

	void init(uint8_t id);
	void update_size();

	void putBuf(const vec &buf);
	void putUInt8(uint8_t val);
	void putUInt16(uint16_t val);
	void putUInt32(uint32_t val);

	void send();
};