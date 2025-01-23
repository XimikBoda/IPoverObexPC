#pragma once
#include <vector>

#ifdef WIN32
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Networking.Sockets.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Rfcomm;
using namespace Windows::Networking::Sockets;
#endif // WIN32


class BTSock {
#ifdef WIN32
	RfcommServiceId serviceId;
	RfcommServiceProvider serviceProvider;
	StreamSocketListener ssl;
	StreamSocket sock;

	IAsyncAction OnConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args);
#endif // WIN32
public:
	BTSock();
	BTSock(uint16_t id);

	void bind();
};

