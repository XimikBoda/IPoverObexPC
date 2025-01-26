#pragma once
#include <queue>
#include "BTAddress.h"

#ifdef WIN32
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Networking.Sockets.h>
#include <winrt/Windows.Storage.Streams.h>

using namespace winrt;
using namespace Windows::Foundation;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Bluetooth::Rfcomm;
using namespace Windows::Networking::Sockets;
using namespace Windows::Storage::Streams;
#elif __unix__
#include <bluetooth/bluetooth.h>
uint8_t find_server_rfcomm_channel(uint16_t service_uuid, const bdaddr_t &addr);
#endif

class BTSock {
#ifdef WIN32
	StreamSocket sock;
	BluetoothDevice device;

	DataWriter writer;
	DataReader reader;
#elif __unix__
	int socket_fd = -1;
    BTAddress remote_addr;
#endif 
public:
	BTSock();

#ifdef WIN32
	BTSock(StreamSocket& sock);
#elif __unix__
	BTSock(int socket_fd, const BTAddress& addr);
	~BTSock();
#endif 

	bool connect(uint16_t id, BTAddress addr);

	BTAddress getRemoteAddress();

	size_t read(void* buf, size_t len);
	std::vector<uint8_t> read(size_t len);
	size_t write(void* buf, size_t len);
	size_t write(std::vector<uint8_t> buf);
};

