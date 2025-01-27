#pragma once
#include "BTSock.h"

#ifdef WIN32
#include <queue>
#include <mutex>
#include <condition_variable>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Networking.Sockets.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Rfcomm;
using namespace Windows::Networking::Sockets;
#elif __unix__
#include "sdbus-c++/Message.h"
#include "sdbus-c++/IObject.h"
#endif 


class BTSockListener {
#ifdef WIN32
	RfcommServiceId serviceId;
	RfcommServiceProvider serviceProvider;
	StreamSocketListener ssl;

	std::mutex socks_queue_mutex;
	std::condition_variable socks_queue_cv;
	std::queue<StreamSocket> socks_queue;

	IAsyncAction OnConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args);
#elif __unix__
    uint16_t serviceId;

	std::mutex socks_queue_mutex;
	std::condition_variable socks_queue_cv;
	std::queue<std::pair<sdbus::UnixFd, BTAddress>> socks_queue;

	std::unique_ptr<sdbus::IObject> IPoverObex_object;

	void NewConnection(sdbus::MethodCall call);
#endif 
public:
	BTSockListener();
    ~BTSockListener();

	void bind(uint16_t id);

	bool accept(BTSock &btsock, bool block = false);

	uint16_t getShortId();
};

