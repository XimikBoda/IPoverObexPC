#include <iostream>
#include <thread>
#include <BTSockListener.h>
#include <OBEXServer.h>
#include <OBEXClient.h>

uint16_t obex_id = 0x1105;

void workerIn(BTSock btsocks) {
	std::cout << "Connected: " << btsocks.getRemoteAddress().toString() << '\n';
	ReaderFromFunc reader([=, &btsocks](size_t len, bool partial){return btsocks.read(len); });
	OBEXServer OBEXs;
	OBEXs.reader = reader;
	OBEXs.writer = [=, &btsocks](vec len) { btsocks.write(len); };
	//OBEXs.run();
}

void workerOut(BTSock btsockc) {
	OBEXClient OBEXc;
	ReaderFromFunc reader([=, &btsockc](size_t len, bool partial) {return btsockc.read(len); });
	OBEXc.reader = reader;
	OBEXc.writer = [=, &btsockc](vec len) { btsockc.write(len); };
	OBEXc.connet();
}

std::vector<std::thread> threads;

int main() {
	BTSockListener btsockl;
	btsockl.bind(obex_id);

	while (true) {
		BTSock btsocks, btsockc;
		btsockl.accept(btsocks, true);
		threads.push_back(std::thread(workerIn, btsocks));

		btsockc.connect(btsockl.getShortId(), btsocks.getRemoteAddress());
		threads.push_back(std::thread(workerOut, btsockc));
	}

	char c;
	std::cin >> c;
}