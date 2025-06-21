#include "OBEXMakePacket.h"

void OBEXMakePacket::init(uint8_t id) {
	buf.resize(0);
	putUInt8(id);
	putUInt16(0);
}

void OBEXMakePacket::update_size() {
	uint16_t size = buf.size();
	if (size < 3)
		return;
	uint8_t* val8 = (uint8_t*)&size;
	buf[1] = val8[1];
	buf[2] = val8[0];
}

void OBEXMakePacket::putBuf(const vec& wbuf) {
	if (!wbuf.size())
		return;
	size_t old_len = buf.size();
	buf.resize(old_len + wbuf.size());
	memcpy(buf.data() + old_len, wbuf.data(), wbuf.size());
}

void OBEXMakePacket::putUInt8(uint8_t val) {
	putBuf({ val });
}

void OBEXMakePacket::putUInt16(uint16_t val) {
	uint8_t* val8 = (uint8_t*)&val;
	putBuf({ val8[1], val8[0] });
}

void OBEXMakePacket::putUInt32(uint32_t val) {
	uint8_t* val8 = (uint8_t*)&val;
	putBuf({ val8[3], val8[2], val8[1], val8[0] });
}

void OBEXMakePacket::send() {
	update_size();
	write(buf);
}


