#include "PacketMaker.h"

static const int SEGMENT_BITS = 0x7F;
static const int CONTINUE_BIT = 0x80;

void PacketMaker::update_size() {
	uint16_t size16 = buf.size();
	uint8_t* val8 = (uint8_t*)&size16;
	buf[0] = val8[0];
	buf[1] = val8[1];
}

void PacketMaker::init(uint16_t type_id) {
	if (mutex.try_lock())
		throw std::runtime_error("mutex must be locked!");

	buf.resize(0);
	putUInt16(0);
	putUInt16(type_id);
}

void PacketMaker::putBuf(const void* cbuf, size_t len) {
	if (!len)
		return;
	size_t old_len = buf.size();
	buf.resize(old_len + len);
	memcpy(buf.data() + old_len, cbuf, len);
}

void PacketMaker::putBuf(const vec& wbuf) {
	if (!wbuf.size())
		return;

	putBuf(wbuf.data(), wbuf.size());
}

void PacketMaker::putUInt8(uint8_t val) {
	buf.push_back(val);
}

void PacketMaker::putUInt16(uint16_t val) {
	uint8_t* val8 = (uint8_t*)&val;
	buf.push_back(val8[0]);
	buf.push_back(val8[1]);
}

void PacketMaker::putUInt32(uint32_t val) {
	uint8_t* val8 = (uint8_t*)&val;
	putBuf(&val, sizeof(val));
}

void PacketMaker::putVarInt(int32_t val) {
	uint32_t value = val;
	while (true) {
		if ((value & ~SEGMENT_BITS) == 0) {
			putUInt8(value & 0xFF);
			return;
		}

		putUInt8(((value & SEGMENT_BITS) | CONTINUE_BIT) & 0xFF);;

		value >>= 7;
	}
}

void PacketMaker::putString(std::string str) {
	putVarInt(str.length());
	putBuf(str.data(), str.length());
}

void PacketMaker::send() {
	update_size();
	write(buf);
}
