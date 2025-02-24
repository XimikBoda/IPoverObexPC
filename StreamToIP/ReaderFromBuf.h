#pragma once
#include <vector>
#include <string>
#include <cstdint>

typedef std::vector<uint8_t> vec;

class ReaderFromBuf {
public:
	size_t read_count = 0;
	vec buf;


	ReaderFromBuf() = default;

	vec readVecBlocking(size_t len);
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();
	uint32_t readVarInt();
	std::string readString();

	void skipToEnd(size_t pack_len);
};