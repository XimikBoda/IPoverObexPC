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

#include <sdbus-c++/sdbus-c++.h>

using namespace std::string_literals;

static std::string makeUUIDfromId(uint16_t id) {
    char tmp[100] = {};
    sprintf(tmp, "0000%04hx-0000-1000-8000-00805f9b34fb", id);
    return tmp;
}

BTSockListener::BTSockListener() {
    server_fd = -1;
    serviceId = 0;
}

BTSockListener::~BTSockListener() {
    if (server_fd >= 0) close(server_fd);
}

void NewConnection(sdbus::MethodCall call) {
    sdbus::ObjectPath dev;
    sdbus::UnixFd sock_fd;
    std::map<std::string, sdbus::Variant> fd_properties;

    call >> dev;
    call >> sock_fd;

    std::cout << "NewConnection " << dev << ' ' << sock_fd.get() << std::endl;
}

void BTSockListener::bind(uint16_t id) {
    sdbus::ServiceName myServiceName{"org.IPoverObex"};
    auto connection = sdbus::createSystemBusConnection(myServiceName);

    sdbus::ObjectPath myObjectPath{"/org/IPoverObex"};
    auto IPoverObex = sdbus::createObject(*connection, myObjectPath);

    sdbus::InterfaceName interfaceName1{"org.bluez.Profile1"};
    IPoverObex->addVTable(sdbus::MethodVTableItem{
                sdbus::MethodName{"NewConnection"},
                sdbus::Signature{"oha{sv}"},
                {},
                sdbus::Signature{""},
                {},
                &NewConnection,
                {}
            }).
            forInterface(interfaceName1);

    //connection->enterEventLoopAsync();

    sdbus::ServiceName serviceName{"org.bluez"};
    sdbus::ObjectPath objectPath{"/org/bluez"};
    auto bluezProxy = sdbus::createProxy(*connection, serviceName, objectPath);

    sdbus::InterfaceName interfaceName{"org.bluez.ProfileManager1"};

    sdbus::MethodName mnRegister{"RegisterProfile"};

    bluezProxy->callMethod("RegisterProfile")
            .onInterface("org.bluez.ProfileManager1")
            .withArguments(myObjectPath,
                           makeUUIDfromId(id),
                           std::map<std::string, sdbus::Variant>{
                               {"Role", sdbus::Variant("server")},
                               {"RequireAuthentication", sdbus::Variant(false)},
                               {"RequireAuthorization", sdbus::Variant(false)},
                           });

    connection->enterEventLoop();

    /*auto Register = bluezProxy->createMethodCall(interfaceName, mnRegister);

    std::string uuid = makeUUIDfromId(id);
    Register << uuid << myObjectPath;
    auto reply = bluezProxy->callMethod(Register);*/

    while (1);

    /*struct sockaddr_rc local_addr = {0};
    serviceId = id;

    server_fd = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    if (server_fd < 0) {
        throw std::runtime_error("Failed to create socket");
    }


    local_addr.rc_family = AF_BLUETOOTH;
    local_addr.rc_bdaddr = *BDADDR_ANY;
    //local_addr.rc_channel = register_service(id);

	std::cout<< local_addr.rc_channel <<'\n';

    if (::bind(server_fd, (struct sockaddr*)&local_addr, sizeof(local_addr)) < 0) {
        close(server_fd);
        throw std::runtime_error("Failed to bind RFCOMM socket");
    }

    if (listen(server_fd, 10) < 0) {
        close(server_fd);
        throw std::runtime_error("Failed to listen on RFCOMM socket");
    }*/
}

bool BTSockListener::accept(BTSock &btsock, bool block) {
    struct sockaddr_rc client_addr = {0};
    socklen_t opt = sizeof(client_addr);

    int flags = fcntl(server_fd, F_GETFL, 0);
    if (block) {
        fcntl(server_fd, F_SETFL, flags & ~O_NONBLOCK);
    } else {
        fcntl(server_fd, F_SETFL, flags | O_NONBLOCK);
    }
    int client_fd = ::accept(server_fd, (struct sockaddr *) &client_addr, &opt);

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
