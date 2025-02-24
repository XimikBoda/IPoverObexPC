#pragma once
#include <cstdint>
#include <memory>
#include <future>

#include <SFML/Network/TcpSocket.hpp>


class TCP {
public:
	enum TCPAct : uint8_t {
		Connect_A
	};

	enum RspStatus : uint8_t {
		Done,
		NotReady,
		Disconnected,
		Error,
		Busy
	};
private:
	std::shared_ptr<sf::TcpSocket> sock = std::make_shared<sf::TcpSocket>();
	std::future<void> connect_future;

	TCP::RspStatus mapSfStatus(sf::Socket::Status status);
public:
	TCP() = default;
	void connect(std::string addr, uint16_t port, std::function<void(uint8_t)> result);
};