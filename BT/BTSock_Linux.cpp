#ifdef __unix__
#include "BTSock.h"

#include "BTSockListener.h"
#include <iostream>
#include <functional>
#include <unistd.h>
#include <fcntl.h>
#include "BluezProfile.h"

using namespace std::string_literals;

static void set_blocking(int fd, bool blocking) {
	int saved_flags = fcntl(fd, F_GETFL);
	if (blocking)
		fcntl(fd, F_SETFL, saved_flags & ~O_NONBLOCK);
	else
		fcntl(fd, F_SETFL, saved_flags | O_NONBLOCK);
}

BTSock::BTSock() {}

BTSock::BTSock(sdbus::UnixFd socket_fd, const BTAddress& addr) {
	this->socket_fd = socket_fd;
	remote_addr = addr;

	set_blocking(this->socket_fd.get(), true);
}

bool BTSock::connect(uint16_t id, BTAddress addr) {
	sdbus::UnixFd fd;
	BluezProfile::getById(id).connect("/org/bluez/hci0/"s + addr.toDBusString(), fd);

	*this = BTSock(fd, addr);
	return true;
}

BTAddress BTSock::getRemoteAddress() {
	return remote_addr;
}

void BTSock::setReadBlocking(DS::AccessMode mode) {
	//set_blocking(this->socket_fd.get(), mode != DS::NonBlocking);
	if(mode == DS::NonBlocking)
		throw std::runtime_error("NonBlocking not implemented yet");
	read_mode = mode;
}

void BTSock::setWriteBlocking(DS::AccessMode mode) {
	//set_blocking(this->socket_fd.get(), mode != DS::NonBlocking);
	if (mode == DS::NonBlocking)
		throw std::runtime_error("NonBlocking not implemented yet");
	write_mode = mode;
}

size_t BTSock::read(void* buf, size_t len) {
	size_t total_read = 0;
	while (total_read < len) {
		size_t bytes_read = ::read(socket_fd.get(), (uint8_t*)(buf) + total_read, len - total_read);
		if (bytes_read <= 0) 
			throw DS::DataException();
		
		total_read += bytes_read;

		if (read_mode == DS::BlockingPartial)
			break;
	}
	return total_read;
}

const vec& BTSock::read(size_t len) {
	static thread_local vec res;

	res.resize(len);
	if (len) {
		auto readed = read(res.data(), len);
		res.resize(readed);
	}
	return res;
}

size_t BTSock::write(const void* buf, size_t len) {
	size_t total_written = 0;
	while (total_written < len) {
		size_t bytes_written = ::write(socket_fd.get(), (const uint8_t*)buf + total_written, len - total_written);
		if (bytes_written < 0)
			throw DS::DataException();

		total_written += bytes_written;

		if (write_mode == DS::BlockingPartial)
			break;
	}
	return total_written;
}

size_t BTSock::write(const vec& buf) {
	return write(buf.data(), buf.size());
}

#endif // __unix__