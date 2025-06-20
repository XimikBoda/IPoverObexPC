#pragma once
#include <vector>
#include <functional>
#include <cstdint>

#include <ReaderAgent.h>

typedef std::vector<uint8_t> vec;

class Reader : public DataStream::ReaderAgent {
public:
	size_t read_count = 0;

	Reader() = default;

	vec readVecBlocking(size_t len);
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();

	void skipToEnd(size_t pack_len);
};