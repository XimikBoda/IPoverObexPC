#ifdef __unix__
#include "BTSockListener.h"
#include "BluezProfile.h"

#include <iostream>
#include <unistd.h>

using namespace std::string_literals;

BTAddress objToBTAddr(const std::string& input) {
	size_t pos = input.find("dev_");
	if (pos == std::string::npos || pos + 4 >= input.length())
		return BTAddress();

	std::string macStr = input.substr(pos + 4);


	uint8_t mac[6];
	sscanf(macStr.c_str(), "%hhx_%hhx_%hhx_%hhx_%hhx_%hhx",
		&mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

	return BTAddress(mac);
}

BTSockListener::BTSockListener() {
	serviceId = 0;
}

bool BTSockListener::bind(uint16_t id) {
	try {
		BluezProfile::getById(id).bind();
		serviceId = id;
	}
	catch (const sdbus::Error& err) {
		std::cout << "Bind error: " << err.getMessage() << "\n";
		return false;
	}
	return true;
}

bool BTSockListener::accept(BTSock& btsock, bool block) {
	sdbus::UnixFd fd;
	std::string dev;
	bool res = BluezProfile::getById(serviceId).accept(dev, fd, block);

	if (res)
		btsock = BTSock(fd, objToBTAddr(dev));

	return res;
}

uint16_t BTSockListener::getShortId() {
	return serviceId;
}

#endif // __unix__
