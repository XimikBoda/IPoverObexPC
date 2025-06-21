#include <iostream>
#include <string>
#include <thread>
#include <BTSockListener.h>
#include <OBEXServer.h>
#include <OBEXClient.h>

uint16_t obex_id = 0x1105;

void workerIn(BTSock btsocks) {
	std::cout << "Connected: " << btsocks.getRemoteAddress().toString() << '\n';

	OBEXServer OBEXs;
	OBEXs.reader.sdra_connect(&btsocks);
	OBEXs.writer.sdwa_connect(&btsocks);
	OBEXs.stream_writer = [=, &btsocks](vec buf) { std::cout << std::string(buf.begin(), buf.end()); };
	OBEXs.run();
}

void workerOut(BTSock btsockc) {
	OBEXClient OBEXc;
	OBEXc.reader.sdra_connect(&btsockc);
	OBEXc.writer.sdwa_connect(&btsockc);
	OBEXc.connet();
	OBEXc.initPutStream("IpOverObex.txt", 0x7FFFFFFF);

	std::string str = "Hi from pc\n";
	OBEXc.PutStreamData(vec(str.begin(), str.end()));
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