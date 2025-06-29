#pragma once
#include <cstdint>
#include <memory>
#include <atomic>

#include <SFML/Network/TcpSocket.hpp>
#include "PacketMaker.h"
#include <Stream.h>


class TCP {
public:
	enum TCPAct : uint8_t {
		Connect,
		Send,
		Receive,
		Disconnect
	};

	enum RspStatus : uint8_t {
		Done,
		NotReady,
		Disconnected,
		Error,
		Busy,
		NameNotResolved
	};
private:
	std::shared_ptr<sf::TcpSocket> sock = std::make_shared<sf::TcpSocket>();
	std::thread connect_thread;
	std::thread send_thread;
	std::thread receive_thread;

	std::condition_variable connect_thread_cv;

	uint16_t type_id = 0;
	PacketMaker* writer = 0;

	DS::Stream send_buf;

	class StreamToIP* owner;

	size_t receive_max_chunk = 10 * 1024; //todo
	size_t receive_buf_size = 0;
	std::atomic<size_t> receive_buf_used = 0;

	bool connected = false;

	TCP::RspStatus mapSfStatus(sf::Socket::Status status);

	void makeRspConnect(RspStatus status);
	void makeRspSend(RspStatus status, size_t sended);
	void makeRspReceive(RspStatus status, const vec& buf);

public:
	TCP() = default;
	void init(PacketMaker* writer, uint16_t type_id, size_t receive_buf);
	void connect(std::string addr, uint16_t port);
	void send(const vec& buf);
	void receive(size_t received);

	void disconnect();

	~TCP();
};