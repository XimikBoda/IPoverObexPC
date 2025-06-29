#pragma once
#include <StreamAgent.h>
#include <string>
#include <mutex>

class PacketMaker : public DS::StreamAgent {
	friend class StreamToIP;
	friend class TCP;

	std::mutex mutex;

	vec buf;

	void update_size();

	void init(uint16_t type_id);

	void putBuf(const void* buf, size_t len);
	void putBuf(const vec& buf);
	void putUInt8(uint8_t val);
	void putUInt16(uint16_t val);
	void putUInt32(uint32_t val);

	void putVarInt(int32_t val);

	void putString(std::string str);

	void send();
};