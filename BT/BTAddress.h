#pragma once
#include <string>

const int MAC_LEN = 6;

class BTAddress {
	uint8_t mac[MAC_LEN] = {};
public:
	BTAddress() = default;
	BTAddress(uint64_t val);
	BTAddress(uint8_t mac[MAC_LEN]);

	uint64_t toUInt64();
	std::string toString();

	void fromUInt64(uint64_t val);
	void fromArray(uint8_t mac[MAC_LEN]);
};