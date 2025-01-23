#include <iostream>
#include <BTSockListener.h>

int main() {
	BTSock btsocks;
	BTSockListener btsockl;
	btsockl.bind();
	btsockl.accept(btsocks, true);

	BTSock btsockc;
	btsockc.connect(btsockl.getShortId(), btsocks.getRemoteAddress());

	char c;
	std::cin >> c;
}