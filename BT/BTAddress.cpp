#include "BTAddress.h"

BTAddress::BTAddress(uint64_t val){
	fromUInt64(val);
}

BTAddress::BTAddress(uint8_t mac[MAC_LEN]) {
	fromArray(mac);
}

uint64_t BTAddress::toUInt64(){
	uint64_t res = 0;
	uint8_t* res8 = (uint8_t*)&res;
	for (int i = 0; i < MAC_LEN; ++i)
		res8[MAC_LEN - 1 - i] = mac[i];
	return res;
}

std::string BTAddress::toString(){
	char str[MAC_LEN * 3 + 2] = {};
	for (int i = 0; i < MAC_LEN; ++i)
		sprintf(str + i * 3, "%02X:", mac[i]);
	str[MAC_LEN * 3 - 1] = 0;
	return std::string(str);
}

void BTAddress::fromUInt64(uint64_t val){
	uint8_t* val8 = (uint8_t*)&val;
	for (int i = 0; i < MAC_LEN; ++i)
		mac[i] = val8[MAC_LEN - 1 - i];
}

void BTAddress::fromArray(uint8_t mac[MAC_LEN]){
	for (int i = 0; i < MAC_LEN; ++i)
		this->mac[i] = mac[i];
}