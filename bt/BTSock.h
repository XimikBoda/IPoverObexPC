#pragma once
#include <queue>

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
#ifdef WIN32W
	StreamSocket sock;

#endif // WIN32
public:
};

