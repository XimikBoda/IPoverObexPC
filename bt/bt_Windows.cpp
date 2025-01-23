#ifdef WIN32
#include "bt.h"

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


BTSock::BTSock() : 
	serviceId(RfcommServiceId::ObexObjectPush()),
	serviceProvider(RfcommServiceProvider::CreateAsync(serviceId).get())
{

}

BTSock::BTSock(uint16_t id) : 
	serviceId(RfcommServiceId::FromShortId(id)),
	serviceProvider(RfcommServiceProvider::CreateAsync(serviceId).get())
{

}

IAsyncAction BTSock::OnConnectionReceived(StreamSocketListener sender, StreamSocketListenerConnectionReceivedEventArgs args) {
	sock = args.Socket();

	std::wcout << "Conection: \n";
	std::wcout << sock.Information().RemoteHostName().ToString().c_str() << '\n';

	auto bdevice = BluetoothDevice::FromHostNameAsync(sock.Information().RemoteHostName()).get();
	std::wcout << bdevice.Name().c_str() << '\n';
	return 0;
}

void BTSock::bind() {
	ssl.ConnectionReceived({ this, &BTSock::OnConnectionReceived });
	ssl.BindServiceNameAsync(serviceId.AsString(), SocketProtectionLevel::BluetoothEncryptionAllowNullAuthentication).get();

	serviceProvider.StartAdvertising(ssl, true);
}

#endif // WIN32

