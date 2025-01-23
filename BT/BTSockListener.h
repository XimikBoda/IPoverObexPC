#pragma once
#include <queue>
#include "BTSock.h"

#ifdef WIN32
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Networking.Sockets.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Rfcomm;
using namespace Windows::Networking::Sockets;
#endif // WIN32


class BTSockListener {
#ifdef WIN32
	RfcommServiceId serviceId;
	RfcommServiceProvider serviceProvider;
	StreamSocketListener ssl;
	std::queue<StreamSocket> socks_queue;

	IAsyncAction OnConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args);
#endif // WIN32
public:
	BTSockListener();
	BTSockListener(uint16_t id);

	void bind();

	bool accept();
};

