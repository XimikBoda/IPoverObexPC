#ifdef WIN32
#include "BTAdapter.h"
#include <codecvt>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Radios.h>
#include <winrt/Windows.System.h>

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Radios;

BTAdapter::BTAdapter() : adapter(BluetoothAdapter::GetDefaultAsync().get()) {
	if (adapter)
		radio = std::make_shared<Radio>(adapter.GetRadioAsync().get());
}

bool BTAdapter::isThere() {
	return (bool)adapter && radio && (bool)*radio;
}

bool BTAdapter::isOn() {
	return  radio->State() == RadioState::On;
}

bool BTAdapter::setOn(bool on) {
	auto accesss = radio->SetStateAsync(on ? RadioState::On : RadioState::Off).get();

	return accesss == RadioAccessStatus::Allowed;
}

BTAddress BTAdapter::getAddress() {
	return BTAddress(adapter.BluetoothAddress());
}

std::string BTAdapter::getName() {
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;

	return converter.to_bytes(radio->Name().c_str());
}

#endif