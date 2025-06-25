#include "TCP.h"
#include "StreamToIP.h"

#include <SFML/Network/IpAddress.hpp>

TCP::RspStatus TCP::mapSfStatus(sf::Socket::Status status) {
	switch (status)
	{
	case sf::Socket::Status::Done:
		return TCP::Done;
		break;
	case sf::Socket::Status::NotReady:
		return TCP::NotReady;
		break;
	case sf::Socket::Status::Partial:
		return TCP::Error; //TODO
		break;
	case sf::Socket::Status::Disconnected:
		return TCP::Disconnected;
		break;
	case sf::Socket::Status::Error:
		return TCP::Error;
		break;
	default:
		return TCP::Error;
		break;
	}
}

void TCP::connect(std::string addr, uint16_t port, std::function<void(uint8_t)> result) {
	if (connect_future.valid())
		return result(Busy);

	connect_future = std::async(std::launch::async, [addr, port, this, result]() {
		auto ip = sf::IpAddress::resolve(addr);
		auto res = sock->connect(ip.value(), port);

		if (res == sf::Socket::Status::Done)
			connected = true;

		result(mapSfStatus(res));
		});
}

void TCP::disconnect() {
	sock->disconnect();
	connected = false;
}

