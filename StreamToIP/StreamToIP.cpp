#include "StreamToIP.h"

uint16_t StreamToIP::getType(uint16_t type_id) {
	return type_id >> bits_for_type;
}

uint16_t StreamToIP::getId(uint16_t type_id) {
	return type_id & ((1 << bits_for_type) - 1);
}

uint16_t StreamToIP::makeTypeId(uint16_t type, uint16_t id) {
	return (type << bits_for_type) | (id & ((1 << bits_for_type) - 1));
}

void StreamToIP::makeRspGeneral(uint16_t type_id, uint8_t act, uint8_t rsp) { //Do I need this
	std::lock_guard lg(writer.mutex);
	writer.init(type_id);
	writer.putUInt8(act);
	writer.putUInt8(rsp);
	writer.send();
}

void StreamToIP::parsePacket() {
	reader.readBegin();
	uint16_t type_id = reader.readUInt16();
	type = getType(type_id);
	id = getId(type_id);

	switch (type) {
	case LOG_T:
		log.parsePacket();
		break;
	case TCP_SOCK_T:
		tcp.parseTCPPacket();
		break;
	case TCP_LISTENER_T:
		tcp.parseTCPLPacket();
		break;
	case UDP_SOCK_T:
		udp.parseUDPPacket();
		break;
	default:
		break;
	}
}

void StreamToIP::worker() {
	try {
		while (running) {
			parsePacket();
		}
	}
	catch (const DS::DataException& err) {}

	reader.sds_close();
	writer.sdsa_close();
}

StreamToIP::StreamToIP() : tcp(*this), log(*this), udp(*this) {}

void StreamToIP::run() {
	if (!worker_thr)
		worker_thr = std::make_unique<std::thread>(&StreamToIP::worker, this);
}

void StreamToIP::wait() {
	if (worker_thr && worker_thr->joinable())
		worker_thr->join();
}

StreamToIP::~StreamToIP() {
	running = false;
	wait();
}