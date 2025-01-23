#ifdef WIN32
#include "BTSockListener.h"

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

BTSockListener::BTSockListener() :
	serviceId(RfcommServiceId::ObexObjectPush()),
	serviceProvider(RfcommServiceProvider::CreateAsync(serviceId).get())
{
}

IAsyncAction BTSockListener::OnConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args) {
	StreamSocket sock = args.Socket();
	{
		std::lock_guard lg(socks_queue_mutex);
		socks_queue.push(sock);
	}
	socks_queue_cv.notify_one();
	return 0;
}

void BTSockListener::bind(uint16_t id) {
	serviceId = RfcommServiceId::FromShortId(id);
	serviceProvider = RfcommServiceProvider::CreateAsync(serviceId).get();
	ssl.ConnectionReceived({ this, &BTSockListener::OnConnectionReceived });
	ssl.BindServiceNameAsync(serviceId.AsString(), SocketProtectionLevel::BluetoothEncryptionAllowNullAuthentication).get();

	serviceProvider.StartAdvertising(ssl, true);
}

bool BTSockListener::accept(BTSock& btsock, bool block) {
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
		btsock = BTSock(socks_queue.front());
		socks_queue.pop();
	}
	return true;
}

uint16_t BTSockListener::getShortId()
{
	return serviceId.AsShortId();
}

#endif // WIN32

