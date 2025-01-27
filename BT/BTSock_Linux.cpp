#ifdef __unix__
#include "BTSock.h"

#include <iostream>
#include <functional>
#include <unistd.h>
#include <fcntl.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>

uint8_t find_server_rfcomm_channel(uint16_t service_uuid, const bdaddr_t &addr) {
    sdp_list_t *response_list, *search_list, *attrid_list;
    sdp_session_t *session;
    uint32_t range = 0x0000ffff;
    uint8_t channel = 0;

    session = sdp_connect(BDADDR_LOCAL, &addr, SDP_RETRY_IF_BUSY);
    if (!session) {
        perror("Failed to connect to the local SDP server");
        return -1;
    }

    uuid_t svc_uuid;
    sdp_uuid16_create(&svc_uuid, service_uuid);
    search_list = sdp_list_append(NULL, &svc_uuid);
    attrid_list = sdp_list_append(NULL, &range);

    if (sdp_service_search_attr_req(session, search_list, SDP_ATTR_REQ_RANGE, attrid_list, &response_list) != 0) {
        perror("SDP query failed");
        sdp_close(session);
        return -1;
    }

    sdp_list_t *proto_list = NULL;
	sdp_list_t *r = response_list;
	for (; r; r = r->next) {
		sdp_record_t *rec = (sdp_record_t*) r->data;
		if(sdp_get_access_protos(rec, &proto_list) == 0 ) {
			channel = sdp_get_proto_port(proto_list, RFCOMM_UUID);
			sdp_list_free(proto_list, 0);
		}
		sdp_record_free(rec);
	}

    sdp_list_free(response_list, 0);
    sdp_list_free(attrid_list, 0);
    sdp_list_free(search_list, 0);
    sdp_close(session);

    if (channel == 0) {
        fprintf(stderr, "No RFCOMM channel found for service UUID 0x%04x\n", service_uuid);
        return -1;
    }

    return channel;
}

BTSock::BTSock() {}

BTSock::BTSock(int socket_fd, const BTAddress& addr) {
	this->socket_fd = socket_fd;
	remote_addr = addr;
}

BTSock::~BTSock() {
    if (socket_fd >= 0) close(socket_fd);
}

bool BTSock::connect(uint16_t id, BTAddress addr) {
	struct sockaddr_rc remote_addr_struct = {0};

    socket_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (socket_fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }

    remote_addr_struct.rc_family = AF_BLUETOOTH;
    remote_addr_struct.rc_bdaddr = addr.to_bdaddr();
    remote_addr_struct.rc_channel = find_server_rfcomm_channel(id, addr.to_bdaddr());

    if (::connect(socket_fd, (struct sockaddr*)&remote_addr_struct, sizeof(remote_addr_struct)) < 0) {
        close(socket_fd);
        socket_fd = -1;
        return false;
    }

    remote_addr = addr;
    return true;
}

BTAddress BTSock::getRemoteAddress() {
	return remote_addr;
}

size_t BTSock::read(void* buf, size_t len) {
	std::cout << "Try to read\n";

    size_t total_read = 0;
    while (total_read < len) {
        ssize_t bytes_read = ::read(socket_fd, static_cast<uint8_t*>(buf) + total_read, len - total_read);
        if (bytes_read < 0) {
			std::cout << "Failed "<<bytes_read<<" "<<strerror(errno)<< " "<< socket_fd <<"\n";
            throw std::runtime_error("Failed to read from socket");
        } else if (bytes_read == 0) {
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
        ssize_t bytes_written = ::write(socket_fd, static_cast<const uint8_t*>(buf) + total_written, len - total_written);
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