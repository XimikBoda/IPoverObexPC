#pragma once
#include <vector>
#include <functional>

typedef std::vector<uint8_t> vec;
typedef std::function<void(vec)> WriteFunc;

class OBEXMakePacket {
public:
	vec buf;

	void init(uint8_t id);
	void update_size();

	void putBuf(const vec &buf);
	void putUInt8(uint8_t val);
	void putUInt16(uint16_t val);

	void send(WriteFunc writer);
};