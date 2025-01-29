#pragma once
#include <vector>
#include <functional>
#include <cstdint>

typedef std::vector<uint8_t> vec;
typedef std::function<vec(size_t, bool)> ReaderFunc;

class ReaderFromFunc {
public:
	size_t read_count = 0;

	ReaderFunc reader_func;

	ReaderFromFunc() = default;
	ReaderFromFunc(ReaderFunc func);

	vec readVecBlocking(size_t len);
	uint8_t readUInt8();
	uint16_t readUInt16();
	uint32_t readUInt32();

	void skipToEnd(size_t pack_len);
};