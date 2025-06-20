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

bool BTSock::connect(uint16_t id, BTAddress addr)
{
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

BTAddress BTSock::getRemoteAddress()
{
	return BTAddress(device.BluetoothAddress());
}

size_t BTSock::read(void* buf, size_t len)
{
	size_t available_size = reader.UnconsumedBufferLength();
	if (len > available_size)
		reader.LoadAsync(len - available_size).get();

	auto readed = reader.ReadBuffer(len);
	memcpy(buf, readed.data(), readed.Length());
	return size_t(readed.Length());
}

std::vector<uint8_t> BTSock::read(size_t len)
{
	if (len == 0)
		return {};
	std::vector<uint8_t> res;
	res.resize(len);
	read(res.data(), len);;
	return res;
}

size_t BTSock::write(void* buf, size_t len)
{
	if (!len)
		return 0;
	return write(std::vector<uint8_t>((uint8_t*)buf, (uint8_t*)buf + len));
}

size_t BTSock::write(std::vector<uint8_t> buf)
{
	writer.WriteBytes(buf);
	if (!buf.size())
		return 0;

	writer.StoreAsync().get();
	return buf.size();
}

bool BTSock::sdr_read(void* buf, size_t len, size_t& readed) {
	readed = read(buf, len);
	return true;
}

std::vector<uint8_t> BTSock::sdr_read(size_t len) {
	return read(len);
}

bool BTSock::sdw_write(void* buf, size_t len, size_t& writed) {
	writed = write(buf, len);
	return true;
}
void BTSock::sdw_write(std::vector<uint8_t> buf) {
	write(buf);
}

#endif // WIN32

