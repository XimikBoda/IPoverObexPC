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

BTSock::BTSock() :
	device(BluetoothDevice::FromBluetoothAddressAsync(0).get()),
	writer(sock.OutputStream()), reader(sock.InputStream())
{
	reader.InputStreamOptions(InputStreamOptions::ReadAhead);
}

BTSock::BTSock(StreamSocket& sock) :
	device(GetBluetoothDeviceFromSocket(sock)),
	writer(sock.OutputStream()), reader(sock.InputStream())
{
	this->sock = sock;
	reader.InputStreamOptions(InputStreamOptions::ReadAhead);
}

bool BTSock::connect(uint16_t id, BTAddress addr) {
	RfcommServiceId serviceId = RfcommServiceId::FromShortId(id);
	device = BluetoothDevice::FromBluetoothAddressAsync(addr.toUInt64()).get();
	if (!device)
		return false;

	auto rfcommServiceResult = device.GetRfcommServicesForIdAsync(serviceId).get();
	if (!rfcommServiceResult.Services().Size())
		return false;

	auto rfcommService = rfcommServiceResult.Services().GetAt(0);

	StreamSocket sock;
	sock.ConnectAsync(rfcommService.ConnectionHostName(), rfcommService.ConnectionServiceName(), SocketProtectionLevel::BluetoothEncryptionAllowNullAuthentication).get();

	*this = BTSock(sock);
}

BTAddress BTSock::getRemoteAddress() {
	return BTAddress(device.BluetoothAddress());
}

void BTSock::setReadBlocking(DS::AccessMode mode) {
	if (read_mode == DS::AccessMode::NonBlocking)
		if (mode != DS::AccessMode::NonBlocking)
			if (read_operation) {
				read_operation->get(); //TODO
				read_operation.reset();
			}

	reader.InputStreamOptions(mode == DS::Blocking ?
		InputStreamOptions::ReadAhead : InputStreamOptions::Partial);

	read_mode = mode;
}

void BTSock::setWriteBlocking(DS::AccessMode mode) {
	write_mode = mode;
}

ssize_t BTSock::readReadyData(void* buf, size_t len) {
	auto readed = reader.ReadBuffer(len);
	memcpy(buf, readed.data(), readed.Length());
	return readed.Length();
}

ssize_t BTSock::readNotBlocking(void* buf, size_t len) {
	if (read_operation)
		if (read_operation->Status() == AsyncStatus::Completed)
			read_operation.reset();

	size_t available_size = reader.UnconsumedBufferLength();
	size_t lack_of_size = len > available_size ? len - available_size : 0;

	if (available_size) {
		if (len > available_size)
			len = available_size;

		return readReadyData(buf, len);
	}

	if (!read_operation)
		read_operation = std::make_shared<DataReaderLoadOperation>(reader.LoadAsync(lack_of_size));

	return 0;
}

ssize_t BTSock::read(void* buf, size_t len) {
	if (read_mode == DS::NonBlocking)
		return readNotBlocking(buf, len);

	size_t available_size = reader.UnconsumedBufferLength();
	size_t lack_of_size = len > available_size ? len - available_size : 0;

	if (lack_of_size && (read_mode == DS::Blocking || available_size == 0))
		reader.LoadAsync(lack_of_size).get();

	available_size = reader.UnconsumedBufferLength();

	if (len > available_size)
		len = available_size;

	return readReadyData(buf, len);
}

const vec& BTSock::read(size_t len) {
	static thread_local vec res;

	res.resize(len);
	if (len)
		read(res.data(), len);

	return res;
}

ssize_t BTSock::write(const void* buf, size_t len) {
	if (!len)
		return 0;

	return write(vec((uint8_t*)buf, (uint8_t*)buf + len));
}

ssize_t BTSock::write(const vec& buf) {
	if (!buf.size())
		return 0;

	writer.WriteBytes(buf);

	writer.StoreAsync().get(); //TODO: add notblocking
	return buf.size();
}

#endif // WIN32