#include "OBEXServer.h"
#include "OBEX.h"

void OBEXServer::getPacketType()
{
	reader.read_count = 0;
	current_pack = reader.readUInt8();
	current_pack_size = reader.readUInt16();
}

void OBEXServer::readPacket() {
	uint8_t hi_current_pack = current_pack | (1 << 7);
	switch (hi_current_pack) {
	case OBEX_Operations::Connect:
		readConnectPacket();
		break;
	case OBEX_Operations::Put:
		readPutPacket();
		break;
	case OBEX_Operations::Disconnect:
		readDisconnectPacket();
		break;
	default:
		skipPacketToEnd();
		break;
	}
}

void OBEXServer::readConnectPacket() {
	version = reader.readUInt8();
	flags = reader.readUInt8();
	max_pack_size = reader.readUInt16();
	readHeaders();

	makeConnectSuccessResponse();
	state = Connected;
}

void OBEXServer::readPutPacket() {
	readHeaders();

	bool is_final = (current_pack >> 7) & 1;
	if (is_final) {
		makePutFinalResponse();
		state = Connected;
	}
	else {
		makePutContinueResponse();
		state = GettingFile;
	}
}

void OBEXServer::readDisconnectPacket() {
	readHeaders();

	makeDisconnectSuccessResponse();

	state = Disconected;
}

void OBEXServer::readHeaders() {
	while (reader.read_count != current_pack_size) {
		readHeader();
	}
}

void OBEXServer::readHeader() {
	uint8_t header_id = reader.readUInt8();
	uint8_t headerType = ((header_id >> 6) & 0b11);

	if (header_id == Body || header_id == EndofBody)
		readStream();
	else
		switch (headerType) {
		case TextH: {
			uint16_t size = reader.readUInt16() - 3;
			vec str_buf = reader.readVecBlocking(size);
			break;
		}
		case BytesH: {
			uint16_t size = reader.readUInt16() - 3;
			vec buf = reader.readVecBlocking(size);
			break;
		}
		case ByteH: {
			uint8_t byte = reader.readUInt8();
			break;
		}
		case IntH: {
			uint32_t val = reader.readUInt32();
			break;
		}

		}
}

void OBEXServer::readStream() {
	uint16_t size = reader.readUInt16() - 3;

	for (uint16_t psize = 0; psize < size;) {
		const vec& buf = reader.readVecPartial(size);
		stream_writer.write(buf);
		psize += buf.size();
	}
}

void OBEXServer::makeConnectSuccessResponse() {
	writer.init(Success);
	writer.putUInt8(version);
	writer.putUInt8(0x00);
	writer.putUInt16(max_pack_size);
	writer.send();
}

void OBEXServer::makePutContinueResponse() {
	writer.init(Continue);
	writer.send();
}

void OBEXServer::makePutFinalResponse() {
	writer.init(Success);
	writer.send();
}

void OBEXServer::makeDisconnectSuccessResponse() {
	writer.init(Success);
	writer.send();
}

void OBEXServer::skipPacketToEnd() {
	reader.skipToEnd(current_pack_size);
	current_pack = 0;
}

void OBEXServer::worker() {
	try {
		while (state != Disconected) {
			getPacketType();
			readPacket();
		}
	}
	catch (const DS::DataException& err) {}

	stream_writer.sdsa_close();
	reader.sdra_close();
	writer.sdwa_close();
}

void OBEXServer::run() {
	if (!worker_thr)
		worker_thr = std::make_unique<std::thread>(&OBEXServer::worker, this);
}

void OBEXServer::wait() {
	if (worker_thr && worker_thr->joinable())
		worker_thr->join();
}

OBEXServer::~OBEXServer() {
	state = Disconected;
	wait();
}
