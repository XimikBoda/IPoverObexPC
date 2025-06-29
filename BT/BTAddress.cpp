#include "BTAddress.h"
#include <locale>
#include <codecvt>
#include <string>

#ifdef WIN32
#include <winrt/base.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Bluetooth.h>

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
#endif 

BTAddress::BTAddress(uint64_t val) {
	fromUInt64(val);
}

BTAddress::BTAddress(uint8_t mac[MAC_LEN]) {
	fromArray(mac);
}

uint64_t BTAddress::toUInt64() {
	uint64_t res = 0;
	uint8_t* res8 = (uint8_t*)&res;
	for (int i = 0; i < MAC_LEN; ++i)
		res8[MAC_LEN - 1 - i] = mac[i];
	return res;
}

std::string BTAddress::toString() {
	char str[100] = {};
	sprintf(str, "%02X:%02X:%02X:%02X:%02X:%02X",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return std::string(str);
}

void BTAddress::fromUInt64(uint64_t val) {
	uint8_t* val8 = (uint8_t*)&val;
	for (int i = 0; i < MAC_LEN; ++i)
		mac[i] = val8[MAC_LEN - 1 - i];
}

void BTAddress::fromArray(uint8_t mac[MAC_LEN]) {
	for (int i = 0; i < MAC_LEN; ++i)
		this->mac[i] = mac[i];
}

#ifdef WIN32
std::string BTAddress::getName() {
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	return converter.to_bytes(getWName());
}

std::wstring BTAddress::getWName() {
	auto device = BluetoothDevice::FromBluetoothAddressAsync(toUInt64()).get();
	if (!device)
		return L"No name";

	return device.Name().c_str();
}
#endif 

#ifdef __unix__
BTAddress::BTAddress(bdaddr_t adr) {
	from_bdaddr(adr);
}

bdaddr_t BTAddress::to_bdaddr() {
	return *(bdaddr_t*)&mac;
}
void BTAddress::from_bdaddr(bdaddr_t adr) {
	*(bdaddr_t*)&mac = adr;
}

std::string BTAddress::toDBusString() {
	char str[100] = {};
	sprintf(str, "dev_%02X_%02X_%02X_%02X_%02X_%02X",
		mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
	return std::string(str);
}
#endif
