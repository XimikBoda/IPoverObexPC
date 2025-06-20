#pragma once
#include <queue>
#include "BTAddress.h"

#include <Reader.h>
#include <Writer.h>

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
#include "sdbus-c++/Types.h"
#endif

class BTSock : public DataStream::Reader, public DataStream::Writer {
#ifdef WIN32
	StreamSocket sock;
	BluetoothDevice device;

	DataWriter writer;
	DataReader reader;
#elif __unix__
	sdbus::UnixFd socket_fd;
	BTAddress remote_addr;
#endif 
public:
	BTSock();

#ifdef WIN32
	BTSock(StreamSocket& sock);
#elif __unix__
	BTSock(sdbus::UnixFd socket_fd, const BTAddress& addr);
#endif 

	bool connect(uint16_t id, BTAddress addr);

	BTAddress getRemoteAddress();

	size_t read(void* buf, size_t len);
	vec read(size_t len);
	size_t write(void* buf, size_t len);
	size_t write(vec buf);

	bool BTSock::sdr_read(void* buf, size_t len, size_t& readed) override;
	vec sdr_read(size_t len) override;
	bool sdw_write(void* buf, size_t len, size_t& writed) override;
	void sdw_write(vec buf) override;
};

