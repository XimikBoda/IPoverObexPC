#pragma once
#include "PacketMaker.h"
#include "TCPSock.h"

#include <cstdint>
#include <memory>
#include <atomic>
#include <thread>
#include <map>

#include <SFML/Network/TcpListener.hpp>
#include <Stream.h>


class TCPListener {
public:
	enum TCPAct : uint8_t {
		Bind,
		Accept,
		Close,
	};
private:
	sf::TcpListener listener;

	std::thread listen_thread;
	
	uint16_t type_id = 0;
	PacketMaker* writer = 0;

	class StreamToIP* owner = 0;

	void makeRspBind(TCPSock::RspStatus status);
	void makeRspAccept(TCPSock::RspStatus status, uint32_t ip);

public:
	TCPListener() = default;

	void init(PacketMaker* writer, uint16_t type_id);
	void bind(uint16_t port);
	void accept(TCPSock& sock);
	void close();

	~TCPListener();
};