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

void TCPListener::makeRspAccept(TCPSock::RspStatus status) {
	std::lock_guard lg(writer->mutex);
	writer->init(type_id);
	writer->putUInt8(TCPAct::Accept);
	writer->putUInt8(status);
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
	if (listen_thread.joinable())
		return makeRspAccept(TCPSock::Busy);

	listen_thread = std::thread([&sock, this]() {

		auto res = listener.accept(*sock.sock);

		makeRspAccept(TCPSock::mapSfStatus(res));
		});
}

void TCPListener::close() {
	listener.close();
}

TCPListener::~TCPListener() {
	if (listen_thread.joinable())
		listen_thread.join();
}