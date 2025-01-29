#ifdef __unix__
#include "BluezProfile.h"
#include <sdbus-c++/sdbus-c++.h>
#include <utility>
#include <vector>
#include <iostream>

using namespace std::string_literals;

std::map<uint16_t, std::shared_ptr<BluezProfile>> BluezProfile::_bluezProfiles;
std::unique_ptr<sdbus::IConnection> BluezProfile::service_connection;
std::unique_ptr<sdbus::IConnection> BluezProfile::bus_connection;

static std::string makeUUIDfromId(uint16_t id) {
    char tmp[100] = {};
    sprintf(tmp, "0000%04hx-0000-1000-8000-00805f9b34fb", id);
    return tmp;
}

static std::string to_stringHex02(uint16_t id) {
  char tmp[20] = {};
  sprintf(tmp, "%02x", id);
  return tmp;
}

BluezProfile &BluezProfile::getById(uint16_t id){
    auto &up = _bluezProfiles[id];
    if(up == nullptr)
        up = std::make_shared<BluezProfile>(id);
    return *up;
}

BluezProfile::BluezProfile(uint16_t id){
    if(!service_connection){
        service_connection = sdbus::createSystemBusConnection(sdbus::ServiceName("org.IPoverObex"));
        //service_connection->enterEventLoopAsync();
        std::thread([&](){service_connection->enterEventLoop(); std::cout<<"Done?\n";}).detach();
        bus_connection = sdbus::createSystemBusConnection();
    }
    profile_object_path = sdbus::ObjectPath("/org/IPoverObex/"s + to_stringHex02(id));
    profile_object = sdbus::createObject(*service_connection, profile_object_path);
    profile_object->addVTable(sdbus::MethodVTableItem{
                sdbus::MethodName{"NewConnection"},
                sdbus::Signature{"oha{sv}"},
                {},
                sdbus::Signature{""},
                {},
                [=] (sdbus::MethodCall msg) {this->NewConnection(msg);},
                {}
            }).
            forInterface(sdbus::InterfaceName("org.bluez.Profile1"));
    this->id = id;
}

void BluezProfile::RegisterProfile(bool server){
    auto bluezProxy = sdbus::createProxy(*service_connection,
                                       sdbus::ServiceName("org.bluez"),
                                       sdbus::ObjectPath("/org/bluez"));

    bluezProxy->callMethod("RegisterProfile")
            .onInterface("org.bluez.ProfileManager1")
            .withArguments(profile_object_path,
                           makeUUIDfromId(id),
                           std::map<std::string, sdbus::Variant>{
                               {"Role", sdbus::Variant(std::string(server ? "server"s : "client"s))},
                               {"RequireAuthentication", sdbus::Variant(false)},
                               {"RequireAuthorization", sdbus::Variant(false)},
                           });

    profile_registered = true;
}

void BluezProfile::UnregisterProfile(){
    auto bluezProxy = sdbus::createProxy(*service_connection,
                                       sdbus::ServiceName("org.bluez"),
                                       sdbus::ObjectPath("/org/bluez"));

    bluezProxy->callMethod("UnregisterProfile")
            .onInterface("org.bluez.ProfileManager1")
            .withArguments(profile_object_path);

    profile_registered = false;
}


void BluezProfile::ConnectProfile(std::string path){
    auto bluezProxy = sdbus::createProxy(*bus_connection,
                                       sdbus::ServiceName("org.bluez"),
                                       sdbus::ObjectPath(path));

    bluezProxy->callMethod("ConnectProfile")
        .onInterface("org.bluez.Device1")
        .withArguments(makeUUIDfromId(id));
}

void BluezProfile::NewConnection(sdbus::MethodCall call){
    sdbus::ObjectPath dev;
    sdbus::UnixFd fd;
    std::map<std::string, sdbus::Variant> fd_properties;

    call >> dev >> fd;

    if(connection){
    	//std::cout << "NewConnection (client) " << dev << ' ' << fd.get() << std::endl;

        connect_fd = fd;
        connection = false;
    }else{
		//std::cout << "NewConnection " << dev << ' ' << fd.get() << std::endl;
		{
			std::lock_guard lg(clients_queue_mutex);
			clients_queue.push({fd, dev});
    	}
    	clients_queue_cv.notify_one();
    }
    return call.createReply().send();
}

void BluezProfile::bind(){
    std::lock_guard lg(main_mutex);
    if(profile_registered)
        UnregisterProfile();
    RegisterProfile(true);
    binded = true;
}

void BluezProfile::unbind(){
	std::lock_guard lg(main_mutex);
    if(profile_registered)
        UnregisterProfile();
    binded = false;
}

bool BluezProfile::accept(std::string &dev, sdbus::UnixFd &fd, bool block){
    if (clients_queue.empty())
        if (block) {
            std::mutex socks_queue_cv_mutex;
            std::unique_lock lk(socks_queue_cv_mutex);
            clients_queue_cv.wait(lk, [=] { return !clients_queue.empty(); });
        }
        else
            return false;
    {
        std::lock_guard lg(clients_queue_mutex);
        auto fr = clients_queue.front();
        fd = fr.first, dev = fr.second;
        clients_queue.pop();
    }
    return true;
}

void BluezProfile::connect(std::string dev, sdbus::UnixFd &fd){
    std::lock_guard lg(main_mutex);
    if(profile_registered)
        UnregisterProfile();
    RegisterProfile(false);

	connection = true;
    connect_fd = sdbus::UnixFd();
    ConnectProfile(dev);
    fd = connect_fd;
    //connect_fd.release();

    UnregisterProfile();
    if(binded)
    	RegisterProfile(true);
}

#endif // __unix__