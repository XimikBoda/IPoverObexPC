#pragma once
#ifdef __unix__
#include <cstdint>
#include <map>
#include <queue>

#include <sdbus-c++/Message.h>
#include <sdbus-c++/IObject.h>

class BluezProfile {
    static std::map<uint16_t, std::shared_ptr<BluezProfile>> _bluezProfiles;
    static std::unique_ptr<sdbus::IConnection> service_connection;
    static std::unique_ptr<sdbus::IConnection> bus_connection;

    sdbus::ObjectPath profile_object_path;
    std::unique_ptr<sdbus::IObject> profile_object;


    uint16_t id;

    std::mutex main_mutex;

    bool connection = false;
    sdbus::UnixFd connect_fd;

    std::mutex clients_queue_mutex;
    std::condition_variable clients_queue_cv;
    std::queue<std::pair<sdbus::UnixFd, std::string>> clients_queue;

    void NewConnection(sdbus::MethodCall call);

    bool binded = false;
    bool profile_registered = false;
    void RegisterProfile(bool server);
    void UnregisterProfile();
    void ConnectProfile(std::string path);

    std::unique_ptr<sdbus::IConnection> getSystemBus();
public:
    BluezProfile(uint16_t id);
    static BluezProfile &getById(uint16_t id);

    void bind();
    void unbind();
    bool accept(std::string &dev, sdbus::UnixFd &fd, bool block = true);
    void connect(std::string dev, sdbus::UnixFd &fd);
};

#endif // __unix__
