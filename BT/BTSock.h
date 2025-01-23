#pragma once
#include <queue>
#include "BTAddress.h"

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
	StreamSocket sock;
	BluetoothDevice device;
#endif // WIN32
public:
	BTSock();

#ifdef WIN32
	BTSock(StreamSocket& sock);
#endif // WIN32

	bool connect(uint16_t id, BTAddress addr);

	BTAddress getRemoteAddress();
};

