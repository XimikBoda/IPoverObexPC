#include "ReaderFromBuf.h"

static const int SEGMENT_BITS = 0x7F;
static const int CONTINUE_BIT = 0x80;

const vec& ReaderFromBuf::readVecBlocking(size_t len) {
	return read(len, DS::Blocking);
}

uint8_t ReaderFromBuf::readUInt8() {
	const auto& buf = readVecBlocking(1);
	return buf[0];
}

uint16_t ReaderFromBuf::readUInt16() {
	const auto& buf = readVecBlocking(2);
	return (buf[1] << 8) | buf[0];
}

uint32_t ReaderFromBuf::readUInt32() {
	const auto& buf = readVecBlocking(4);
	return (buf[3] << 24) | (buf[2] << 16) | (buf[1] << 8) | buf[0];
}

uint32_t ReaderFromBuf::readVarInt() {
	int32_t value = 0;
	uint8_t position = 0;
	uint8_t currentByte;

	while (true) {
		currentByte = readUInt8();
		value |= ((currentByte & SEGMENT_BITS) << position);

		if ((currentByte & CONTINUE_BIT) == 0) break;

		position += 7;
	}

	return value;
}

std::string ReaderFromBuf::readString() {
	uint32_t len = readVarInt();
	const auto& buf = readVecBlocking(len);

	return std::string(buf.begin(), buf.end());
}

//void ReaderFromBuf::skipToEnd(size_t pack_len) {
//	if (read_count >= pack_len)
//		return;
//	readVecBlocking(pack_len - read_count);
//}