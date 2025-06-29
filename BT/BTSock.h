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

class BTSock : public DS::Reader, public DS::Writer {
#ifdef WIN32
	StreamSocket sock;
	BluetoothDevice device;

	DataReader reader;
	DataWriter writer;

	ssize_t readReadyData(void* buf, size_t len);
#elif __unix__
	sdbus::UnixFd socket_fd;
	BTAddress remote_addr;
#endif 

	DS::AccessMode read_mode = DS::Blocking;
	DS::AccessMode write_mode = DS::Blocking;

	//ssize_t readNotBlocking(void* buf, size_t len);

	//std::shared_ptr<DataReaderLoadOperation> read_operation;

public:
	BTSock();

#ifdef WIN32
	BTSock(StreamSocket& sock);
#elif __unix__
	BTSock(sdbus::UnixFd socket_fd, const BTAddress& addr);
#endif 

	bool connect(uint16_t id, BTAddress addr);

	BTAddress getRemoteAddress();

	void setReadBlocking(DS::AccessMode mode) override;
	void setWriteBlocking(DS::AccessMode mode) override;

	ssize_t read(void* buf, size_t len) override;
	const vec& read(size_t len) override;
	ssize_t write(const void* buf, size_t len) override;
	ssize_t write(const vec& buf) override;
};

