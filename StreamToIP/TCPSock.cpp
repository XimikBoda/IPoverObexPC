#include "TCPSock.h"
#include "StreamToIP.h"
#include <iostream>

#include <SFML/Network/IpAddress.hpp>

TCPSock::RspStatus TCPSock::mapSfStatus(sf::Socket::Status status) {
	switch (status)
	{
	case sf::Socket::Status::Done:
		return TCPSock::Done;
		break;
	case sf::Socket::Status::NotReady:
		return TCPSock::NotReady;
		break;
	case sf::Socket::Status::Partial:
		return TCPSock::Error; //TODO
		break;
	case sf::Socket::Status::Disconnected:
		return TCPSock::Disconnected;
		break;
	case sf::Socket::Status::Error:
		return TCPSock::Error;
		break;
	default:
		return TCPSock::Error;
		break;
	}
}

void TCPSock::makeRspConnect(RspStatus status, uint32_t ip) {
	std::lock_guard lg(writer->mutex);
	writer->init(type_id);
	writer->putUInt8(TCPAct::Connect);
	writer->putUInt8(status);
	writer->putUInt32(ip);
	writer->send();
}

void TCPSock::makeRspSend(RspStatus status, size_t sended) {
	std::lock_guard lg(writer->mutex);
	writer->init(type_id);
	writer->putUInt8(TCPAct::Send);
	writer->putUInt8(status);
	writer->putVarInt(sended);
	writer->send();
}

void TCPSock::makeRspReceive(RspStatus status, const vec& buf) {
	std::lock_guard lg(writer->mutex);
	writer->init(type_id);
	writer->putUInt8(TCPAct::Receive);
	writer->putUInt8(status);
	writer->putBuf(buf);
	writer->send();
}

void TCPSock::init(PacketMaker* writer, uint16_t type_id, size_t receive_buf) {
	this->writer = writer;
	this->type_id = type_id;
	receive_buf_size = receive_buf;
	send_buf_a.sdsa_connect(&send_buf);
}

void TCPSock::connect(std::string addr, uint16_t port) {
	if (connect_thread.joinable())
		return makeRspConnect(Busy, 0);

	connect_thread = std::thread([addr, port, this]() {
		auto ip = sf::IpAddress::resolve(addr);
		if (!ip.has_value())
			return makeRspConnect(NameNotResolved, 0);

		auto res = sock->connect(ip.value(), port, sf::seconds(30));

		if (res == sf::Socket::Status::Done) {
			connected = true;
			receive(0); // Start receiving
		}

		makeRspConnect(mapSfStatus(res), ip->toInteger());
		});
}

void TCPSock::send(const vec& buf) {
	send_buf.sds_write(buf.data(), buf.size());

	if(send_thread_busy)
		return;

	if (send_thread.joinable())
		send_thread.join();

	send_thread_busy = true;

	send_thread = std::thread([this]() {
		while (true) {
			if (!connected)
				break;

			vec buf = send_buf.readAll(DS::Blocking);
			if (!buf.size())
				break;

			auto res = sock->send(buf.data(), buf.size());
			if (res != sf::Socket::Status::Done)
				connected = false;

			makeRspSend(mapSfStatus(res), buf.size());
		}
		send_thread_busy = false;
		});
}

void TCPSock::receive(size_t received) {
	if (receive_buf_used < received)
		throw std::runtime_error("Some went wrong"); //todo

	receive_buf_used -= received;

	if (receive_thread.joinable()) {
		connect_thread_cv.notify_one();
		return;
	}


	receive_thread = std::thread([this]() {
		while (true) {
			if (!connected)
				break;

			size_t size = receive_buf_size - receive_buf_used;
			if (size > receive_max_chunk)
				size = receive_max_chunk;



			if (!size) {
				std::mutex m;
				std::unique_lock lk(m);
				connect_thread_cv.wait(lk, [&] { return receive_buf_size - receive_buf_used || !connected; });
				continue;
			}

			vec buf(size);

			auto res = sock->receive(buf.data(), size, size);
			receive_buf_used += size;

			if(!connected)
				break;

			buf.resize(size);

			if (res != sf::Socket::Status::Done)
				connected = false;

			makeRspReceive(mapSfStatus(res), buf);
		}
		});
}

void TCPSock::disconnect() {
	connected = false;
	connect_thread_cv.notify_one();
	send_buf_a.sdsa_close();
	sock->disconnect();
}

TCPSock::~TCPSock() {
	disconnect();
	if (connect_thread.joinable())
		connect_thread.join();
	if (send_thread.joinable())
		send_thread.join();
	if (receive_thread.joinable())
		receive_thread.join();
}

