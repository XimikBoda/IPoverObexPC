#include "UDPSock.h"
#include "StreamToIP.h"
#include <iostream>

#include <SFML/Network/IpAddress.hpp>

UDPSock::RspStatus UDPSock::mapSfStatus(sf::Socket::Status status) {
	switch (status)
	{
	case sf::Socket::Status::Done:
		return UDPSock::Done;
		break;
	case sf::Socket::Status::NotReady:
		return UDPSock::NotReady;
		break;
	case sf::Socket::Status::Partial:
		return UDPSock::Error; //TODO
		break;
	case sf::Socket::Status::Disconnected:
		return UDPSock::Disconnected;
		break;
	case sf::Socket::Status::Error:
		return UDPSock::Error;
		break;
	default:
		return UDPSock::Error;
		break;
	}
}

void UDPSock::makeRspBind(RspStatus status, uint16_t port) {
	std::lock_guard lg(writer->mutex);
	writer->init(type_id);
	writer->putUInt8(UDPAct::Bind);
	writer->putUInt8(status);
	writer->putUInt16(port);
	writer->send();
}

void UDPSock::makeRspSend(RspStatus status, size_t sended) {
	std::lock_guard lg(writer->mutex);
	writer->init(type_id);
	writer->putUInt8(UDPAct::Send);
	writer->putUInt8(status);
	writer->putVarInt(sended);
	writer->send();
}

void UDPSock::makeRspReceive(RspStatus status, const Pack& pack) {
	std::lock_guard lg(writer->mutex);
	writer->init(type_id);
	writer->putUInt8(UDPAct::Receive);
	writer->putUInt8(status);

	writer->putUInt32(pack.ip.value().toInteger());
	writer->putUInt16(pack.port);
	writer->putBuf(pack.data);

	writer->send();
}

void UDPSock::bind(PacketMaker* writer, uint16_t type_id, size_t receive_buf, uint16_t port) {
	this->writer = writer;
	this->type_id = type_id;
	receive_buf_size = receive_buf;

	auto res = sock->bind(port);
	if (res == sf::Socket::Status::Done)
		binded = true;
	else
		binded = false;

	makeRspBind(mapSfStatus(res), sock->getLocalPort());
}

void UDPSock::send(const vec& buf, sf::IpAddress ip, uint16_t port) {
	send_pakets.push({buf, ip, port});

	if(send_thread_busy)
		return;

	if (send_thread.joinable())
		send_thread.join();

	send_thread_busy = true;

	send_thread = std::thread([this]() {
		while (binded) {
			Pack pack;

			bool ok = send_pakets.pop(pack);
			if (!ok)
				break;

			auto res = sock->send(pack.data.data(), pack.data.size(), pack.ip.value(), pack.port);
			if (res != sf::Socket::Status::Done)
				binded = false;

			//makeRspSend(mapSfStatus(res), pack.data.size());
		}
		send_thread_busy = false;
		});
}

void UDPSock::receive(size_t received) {
	if (receive_buf_used < received)
		throw std::runtime_error("Some went wrong"); //todo

	receive_buf_used -= received;

	receive_thread = std::thread([this]() {
		while (binded) {
			size_t size = UINT16_MAX;

			Pack pack = {vec(size)};

			auto res = sock->receive(pack.data.data(), size, size, pack.ip, pack.port);

			if (size > receive_max_packet_size || receive_buf_used + size + 4 + 2 > receive_max_packet_size)
				continue; // skip packet

			if(!binded)
				break;

			receive_buf_used += size + 4 + 2;

			pack.data.resize(size);

			if (res != sf::Socket::Status::Done)
				binded = false;

			makeRspReceive(mapSfStatus(res), pack);
		}
		});
}

void UDPSock::unbind() {
	binded = false;
	sock->unbind();
}

UDPSock::~UDPSock() {
	unbind();
	if (send_thread.joinable())
		send_thread.join();
	if (receive_thread.joinable())
		receive_thread.join();
}

