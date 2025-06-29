#ifdef __unix__
#include "BTAdapter.h"
#include "BluezProfile.h"

#include <sdbus-c++/sdbus-c++.h>
#include <iostream>
#include <codecvt>
#include <locale>


BTAdapter::BTAdapter() {
	adapterProxy = sdbus::createProxy(BluezProfile::getBusConnection(),
		sdbus::ServiceName("org.bluez"),
		sdbus::ObjectPath("/org/bluez/hci0"));
}

bool BTAdapter::isThere() {
	return true;
}

bool BTAdapter::isOn() {
	bool powered = adapterProxy->getProperty("Powered").
		onInterface("org.bluez.Adapter1").get<bool>();

	return powered;
}

bool BTAdapter::setOn(bool on) {
	try {
		adapterProxy->setProperty("Powered").
			onInterface("org.bluez.Adapter1").toValue<bool>(on);
	}
	catch (const sdbus::Error& err) {
		std::cout << '(' << err.getMessage() << ") ";
	}
	return isOn();
}

BTAddress BTAdapter::getAddress() {
}

std::string BTAdapter::getName() {
}

std::wstring BTAdapter::getWName() {
}

#endif