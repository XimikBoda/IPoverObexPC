#include "TCPListener.h"
#include "StreamToIP.h"
#include <iostream>

void TCPListener::makeRspBind(TCPSock::RspStatus status) {
	std::lock_guard lg(writer->mutex);
	writer->init(type_id);
	writer->putUInt8(TCPAct::Bind);
	writer->putUInt8(status);
	writer->send();
}

void TCPListener::makeRspAccept(TCPSock::RspStatus status, uint32_t ip) {
	std::lock_guard lg(writer->mutex);
	writer->init(type_id);
	writer->putUInt8(TCPAct::Accept);
	writer->putUInt8(status);
	writer->putUInt32(ip);
	writer->send();
}

void TCPListener::init(PacketMaker* writer, uint16_t type_id) {
	this->writer = writer;
	this->type_id = type_id;
}

void TCPListener::bind(uint16_t port) {
	auto status = listener.listen(port);
	makeRspBind(TCPSock::mapSfStatus(status));
}

void TCPListener::accept(TCPSock& sock) {
	if (listen_thread_busy)
		return makeRspAccept(TCPSock::Busy, 0);

	if (listen_thread.joinable())
		listen_thread.join();

	listen_thread = std::thread([&sock, this]() {
		auto res = listener.accept(*sock.sock);

		uint32_t ip = 0;
		if (res == sf::Socket::Status::Done) {
			auto address = sock.sock->getRemoteAddress();
			if (address.has_value())
				ip = address->toInteger();

			sock.connected = true;
			sock.makeRspConnect(TCPSock::Done, ip);
			sock.receive(0); //todo
		}

		makeRspAccept(TCPSock::mapSfStatus(res), ip);

		listen_thread_busy = false;
		});
}

void TCPListener::close() {
	listener.close();
}

TCPListener::~TCPListener() {
	listener.close();
	if (listen_thread.joinable())
		listen_thread.join();
}