#include <iostream>
#include <BTSockListener.h>
#include "OBEXServer.h"

uint16_t obex_id = 0x1105;

int main() {
	BTSock btsocks;
	BTSockListener btsockl;
	btsockl.bind(obex_id);
	btsockl.accept(btsocks, true);

	ReaderFromFunc reader([=, &btsocks](size_t len, bool partial){return btsocks.read(len); });
	OBEXServer OBEXs;
	OBEXs.reader = reader;
	OBEXs.writer = [=, &btsocks](vec len) { btsocks.write(len); };
	OBEXs.run();

	//BTSock btsockc;
	//btsockc.connect(btsockl.getShortId(), btsocks.getRemoteAddress());

	char c;
	std::cin >> c;
}