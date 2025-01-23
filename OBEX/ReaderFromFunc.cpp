#include "ReaderFromFunc.h"

ReaderFromFunc::ReaderFromFunc(ReaderFunc func){
	this->reader_func = func;
}

vec ReaderFromFunc::readVecBlocking(size_t len){
	auto buf = reader_func(len, false);
	read_count += len;
	return buf;
}

uint8_t ReaderFromFunc::readUInt8() {
	auto buf = readVecBlocking(1);
	return buf[0];
}

uint16_t ReaderFromFunc::readUInt16() {
	auto buf = readVecBlocking(2);
	return (buf[0] << 8) | buf[1];
}

void ReaderFromFunc::skipToEnd(size_t pack_len) {
	if (read_count >= pack_len)
		return;
	readVecBlocking(pack_len - read_count);
}