#pragma once
#include <vector>
#include <functional>
#include <cstdint>

#include <ReaderAgent.h>

class Reader : public DS::ReaderAgent {
public:
	size_t read_count = 0;

	Reader() = default;

	vec readVecBlocking(size_t len);
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();

	void skipToEnd(size_t pack_len);
};