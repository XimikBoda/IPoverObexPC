#include "Reader.h"

const vec& Reader::readVecBlocking(size_t len) {
	const auto& buf = read(len);
	read_count += len;
	return buf;
}

const vec& Reader::readVecPartial(size_t len) {
	setReadBlocking(DS::BlockingPartial);
	const auto& buf = read(len);
	setReadBlocking(DS::Blocking);

	read_count += len;
	return buf;
}


uint8_t Reader::readUInt8() {
	const auto& buf = readVecBlocking(1);
	return buf[0];
}

uint16_t Reader::readUInt16() {
	const auto& buf = readVecBlocking(2);
	return (buf[0] << 8) | buf[1];
}

uint32_t Reader::readUInt32() {
	const auto& buf = readVecBlocking(4);
	return (buf[0] << 24) | (buf[1] << 16) | (buf[2] << 8) | buf[3];
}

void Reader::skipToEnd(size_t pack_len) {
	if (read_count >= pack_len)
		return;
	readVecBlocking(pack_len - read_count);
}