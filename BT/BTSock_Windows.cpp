#ifdef WIN32
#include "BTSock.h"

#include <iostream>
#include <functional>

#include <winrt/base.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.ApplicationModel.Background.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Bluetooth.Rfcomm.h>
#include <winrt/Windows.Devices.Bluetooth.Background.h>
#include <winrt/Windows.Devices.Bluetooth.Advertisement.h>
#include <winrt/Windows.Devices.Bluetooth.GenericAttributeProfile.h>
#include <winrt/Windows.Devices.Enumeration.h>

using namespace Windows::Networking::Sockets;

BluetoothDevice GetBluetoothDeviceFromSocket(const StreamSocket& sock) {
	return BluetoothDevice::FromHostNameAsync(sock.Information().RemoteAddress()).get();
}

BTSock::BTSock(): device(BluetoothDevice::FromBluetoothAddressAsync(0).get()){
}

BTSock::BTSock(StreamSocket& sock): device(GetBluetoothDeviceFromSocket(sock)){
	this->sock = sock;
}

bool BTSock::connect(uint16_t id, BTAddress addr)
{
	RfcommServiceId serviceId = RfcommServiceId::FromShortId(id);
	device = BluetoothDevice::FromBluetoothAddressAsync(addr.toUInt64()).get();
	if(!device)
		return false;

	auto rfcommServiceResult = device.GetRfcommServicesForIdAsync(serviceId).get();
	if (!rfcommServiceResult.Services().Size())
		return false;

	auto rfcommService = rfcommServiceResult.Services().GetAt(0);
	sock.ConnectAsync(rfcommService.ConnectionHostName(), rfcommService.ConnectionServiceName()).get();
}

#endif // WIN32

