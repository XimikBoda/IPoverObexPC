#include <iostream>
#include <BTSockListener.h>

uint16_t obex_id = 0x1105;

int main() {
	BTSock btsocks;
	BTSockListener btsockl;
	btsockl.bind(obex_id);
	btsockl.accept(btsocks, true);

	BTSock btsockc;
	btsockc.connect(btsockl.getShortId(), btsocks.getRemoteAddress());

	char c;
	std::cin >> c;
}