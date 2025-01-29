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

size_t BTSock::read(void* buf, size_t len) {
	size_t total_read = 0;
	while (total_read < len) {
		ssize_t bytes_read = ::read(socket_fd.get(), static_cast<uint8_t*>(buf) + total_read, len - total_read);
		if (bytes_read < 0) {
			throw std::runtime_error("Failed to read from socket");
		}
		else if (bytes_read == 0) {
			break; // End of stream
		}
		total_read += static_cast<size_t>(bytes_read);
	}
	return total_read;
}

std::vector<uint8_t> BTSock::read(size_t len) {
	if (len == 0) return {};
	std::vector<uint8_t> buf(len);
	size_t bytes_read = read(buf.data(), len);
	buf.resize(bytes_read);
	return buf;
}

size_t BTSock::write(void* buf, size_t len) {
	size_t total_written = 0;
	while (total_written < len) {
		ssize_t bytes_written = ::write(socket_fd.get(), static_cast<const uint8_t*>(buf) + total_written, len - total_written);
		if (bytes_written < 0) {
			throw std::runtime_error("Failed to write to socket");
		}
		total_written += static_cast<size_t>(bytes_written);
	}
	return total_written;
}

size_t BTSock::write(std::vector<uint8_t> buf) {
	return write(buf.data(), buf.size());
}

#endif // __unix__