#include <iostream>
#include <BTSockListener.h>

int main() {
	BTSock btsock;
	BTSockListener btsockl;
	btsockl.bind();
	btsockl.accept(btsock, true);
	char c;
	std::cin >> c;
}