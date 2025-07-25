#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <Stream.h>

typedef std::vector<uint8_t> vec;

class PacketReader : public DS::Stream {
	friend class StreamToIP;
	friend class TCP;
	friend class Log;

	uint32_t readed = 0;
	uint32_t size = 0;

	PacketReader() = default;

	void readBegin();

	const vec& readVecBlocking(size_t len);
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();
	uint32_t readVarInt();
	std::string readString();

	const vec& readToEnd();

	//void skipToEnd(size_t pack_len);
};