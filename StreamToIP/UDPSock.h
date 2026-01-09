#pragma once
#include "PacketMaker.h"

#include <cstdint>
#include <memory>
#include <atomic>
#include <thread>
#include <map>
#include <queue>

#include <SFML/Network/UdpSocket.hpp>
#include <QueueBlocking.h>

class UDPSock {
	friend class TCPListener;
public:
	enum UDPAct : uint8_t {
		Bind,
		Send,
		Receive,
		Unbind
	};

	enum RspStatus : uint8_t {
		Done,
		NotReady,
		Disconnected,
		Error,
		Busy,
		NameNotResolved
	};

	struct Pack {
		vec data = {};
		std::optional<sf::IpAddress> ip;
		uint16_t port = 0;
	};

	static UDPSock::RspStatus mapSfStatus(sf::Socket::Status status);
private:
	std::shared_ptr<sf::UdpSocket> sock = std::make_shared<sf::UdpSocket>();
	std::thread send_thread;
	std::thread receive_thread;

	bool send_thread_busy = false;

	uint16_t type_id = 0;
	PacketMaker* writer = 0;

	DS::QueueBlocking<Pack> send_pakets;

	class StreamToIP* owner;

	size_t receive_max_packet_size = UINT16_MAX; //todo
	size_t receive_buf_size = 0;
	std::atomic<size_t> receive_buf_used = 0;

	bool binded = false;

	void makeRspBind(RspStatus status, uint16_t port);
	void makeRspSend(RspStatus status, size_t sended);
	void makeRspReceive(RspStatus status, const Pack& pack);

public:
	UDPSock() = default;
	void bind(PacketMaker* writer, uint16_t type_id, size_t receive_buf, uint16_t port);
	void send(const vec& buf, sf::IpAddress ip, uint16_t port);
	void receive(size_t received);
	void unbind();

	~UDPSock();
};