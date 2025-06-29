#pragma once
#include "BTAddress.h"

#ifdef WIN32
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Bluetooth.h>
#include <winrt/Windows.Devices.Radios.h>

using namespace winrt;
using namespace Windows::Devices::Bluetooth;
using namespace Windows::Devices::Radios;
#endif

class BTAdapter {
#ifdef WIN32
	BluetoothAdapter adapter;
	std::shared_ptr<Radio> radio;
#endif
public:
	BTAdapter();

	bool isThere();
	bool isOn();
	bool setOn(bool on);

	BTAddress getAddress();
	std::string getName();
	std::wstring getWName();
};