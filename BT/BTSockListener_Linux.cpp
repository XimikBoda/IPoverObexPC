#ifdef __unix__
#include "BTSockListener.h"

#include <iostream>
#include <functional>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

BTSockListener::BTSockListener() {
	server_fd = -1;
	serviceId = 0;
}

BTSockListener::~BTSockListener() {
    if (server_fd >= 0) close(server_fd);
}

void BTSockListener::bind(uint16_t id) {
    struct sockaddr_rc local_addr = {0};
    serviceId = id;

    server_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (server_fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }


    local_addr.rc_family = AF_BLUETOOTH;
    local_addr.rc_bdaddr = *BDADDR_ANY;
    local_addr.rc_channel = find_server_rfcomm_channel(id, *BDADDR_LOCAL);

    if (::bind(server_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        close(server_fd);
        throw std::runtime_error("Failed to bind RFCOMM socket");
    }

    if (listen(server_fd, 1) < 0) {
        close(server_fd);
        throw std::runtime_error("Failed to listen on RFCOMM socket");
    }
}

bool BTSockListener::accept(BTSock& btsock, bool block) {
    struct sockaddr_rc client_addr = {0};
    socklen_t opt = sizeof(client_addr);

	int flags = fcntl(server_fd, F_GETFL, 0);
    if (block) {
        fcntl(server_fd, F_SETFL, flags & ~O_NONBLOCK);
    } else {
        fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
    }
    int client_fd = ::accept(server_fd, (struct sockaddr*)&client_addr, &opt);

    if (client_fd < 0) {
        if (block || errno != EAGAIN) {
            std::cerr << "Error accepting connection" << std::endl;
        }
        return false;
    }

    btsock = BTSock(client_fd, BTAddress(client_addr.rc_bdaddr));
    return true;
}

uint16_t BTSockListener::getShortId() {
	return serviceId;
}

#endif // __unix__

