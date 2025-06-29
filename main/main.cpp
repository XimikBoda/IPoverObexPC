#include <iostream>
#include <string>
#include <thread>
#include <BTSockListener.h>
#include <OBEXServer.h>
#include <OBEXClient.h>
#include <StreamToIP.h>

uint16_t obex_id = 0x1105;

void worker(BTSock btsocks, BTSock btsockc) { // TODO make class
	auto mac = btsocks.getRemoteAddress().toString();
	auto name = btsocks.getRemoteAddress().getWName();
	std::cout << "Connected: ";
	std::wcout << name;
	std::cout << " (" << mac << ")\n";

	OBEXServer OBEXs;
	OBEXs.reader.sdra_connect(&btsocks);
	OBEXs.writer.sdwa_connect(&btsocks);

	OBEXClient OBEXc;
	OBEXc.reader.sdra_connect(&btsockc);
	OBEXc.writer.sdwa_connect(&btsockc);

	StreamToIP stip;
	stip.reader.sds_connect(&OBEXs.stream_writer);
	stip.writer.sdsa_connect(&OBEXc.stream_reader);

	OBEXc.connet();
	OBEXc.initPutStream("IpOverObex.txt", 0x7FFFFFFF);

	OBEXs.run();
	OBEXc.run();
	stip.run();

	OBEXs.wait();
	OBEXc.wait();
	stip.wait();

	std::cout << "Disconnected: " << mac << '\n';
}

std::vector<std::thread> threads;

int main() {
	std::locale::global(std::locale(""));
	std::ios::sync_with_stdio(false);

	BTSockListener btsockl;
	btsockl.bind(obex_id);

	while (true) {
		BTSock btsocks, btsockc;
		btsockl.accept(btsocks, true);
		btsockc.connect(btsockl.getShortId(), btsocks.getRemoteAddress());

		threads.push_back(std::thread(worker, btsocks, btsockc));
	}

	char c;
	std::cin >> c;
}