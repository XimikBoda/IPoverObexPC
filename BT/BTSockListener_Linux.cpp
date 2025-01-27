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

auto global_dbus_connection = sdbus::createSystemBusConnection(sdbus::ServiceName("org.IPoverObex"));

static std::string makeUUIDfromId(uint16_t id) {
    char tmp[100] = {};
    sprintf(tmp, "0000%04hx-0000-1000-8000-00805f9b34fb", id);
    return tmp;
}

BTAddress objToBTAddr(const std::string &input) {
    size_t pos = input.find("dev_");
    if (pos == std::string::npos || pos + 4 >= input.length())
        return BTAddress();

    std::string macStr = input.substr(pos + 4);


    uint8_t mac[6];
    sscanf(macStr.c_str(), "%hhx_%hhx_%hhx_%hhx_%hhx_%hhx",
        &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5]);

    return BTAddress(mac);
}

BTSockListener::BTSockListener() {
    serviceId = 0;
}

BTSockListener::~BTSockListener() {
}

void BTSockListener::NewConnection(sdbus::MethodCall call) {
    sdbus::ObjectPath dev;
    sdbus::UnixFd fd;
    std::map<std::string, sdbus::Variant> fd_properties;

    call >> dev >> fd;
    {
        std::lock_guard lg(socks_queue_mutex);
        socks_queue.push({fd, objToBTAddr(dev)});
    }
    socks_queue_cv.notify_one();

    std::cout << "NewConnection " << dev << ' ' << fd.get() << std::endl;
}

void BTSockListener::bind(uint16_t id) {
    sdbus::ObjectPath myObjectPath{"/org/IPoverObex/"s + std::to_string(id)};
    IPoverObex_object = sdbus::createObject(*global_dbus_connection, myObjectPath);

    sdbus::InterfaceName interfaceName1{"org.bluez.Profile1"};
    IPoverObex_object->addVTable(sdbus::MethodVTableItem{
                sdbus::MethodName{"NewConnection"},
                sdbus::Signature{"oha{sv}"},
                {},
                sdbus::Signature{""},
                {},
                [=] (sdbus::MethodCall msg) {this->NewConnection(msg);},
                {}
            }).
            forInterface(interfaceName1);

    sdbus::ServiceName bluezServiceName{"org.bluez"};
    sdbus::ObjectPath bluezObjectPath{"/org/bluez"};
    auto bluezProxy = sdbus::createProxy(*global_dbus_connection, bluezServiceName, bluezObjectPath);

    sdbus::InterfaceName interfaceName{"org.bluez.ProfileManager1"};

    bluezProxy->callMethod("RegisterProfile")
            .onInterface("org.bluez.ProfileManager1")
            .withArguments(myObjectPath,
                           makeUUIDfromId(id),
                           std::map<std::string, sdbus::Variant>{
                               {"Role", sdbus::Variant("server")},
                               {"RequireAuthentication", sdbus::Variant(false)},
                               {"RequireAuthorization", sdbus::Variant(false)},
                           });

    global_dbus_connection->enterEventLoopAsync();
}

bool BTSockListener::accept(BTSock &btsock, bool block) {
    if (socks_queue.empty())
        if (block) {
            std::mutex socks_queue_cv_mutex;
            std::unique_lock lk(socks_queue_cv_mutex);
            socks_queue_cv.wait(lk, [=] { return !socks_queue.empty(); });
        }
        else
            return false;
    {
        std::lock_guard lg(socks_queue_mutex);
        auto fr = socks_queue.front();
        btsock = BTSock(fr.first, fr.second);
        socks_queue.pop();
    }
    return true;
}

uint16_t BTSockListener::getShortId() {
    return serviceId;
}

#endif // __unix__
