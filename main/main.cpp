#ifdef WIN32
#include <windows.h>
#endif
#include <iostream>
#include <string>
#include <thread>
#include <BTSockListener.h>
#include <BTAdapter.h>
#include <OBEXServer.h>
#include <OBEXClient.h>
#include <StreamToIP.h>

uint16_t obex_id = 0x1105;

void worker(BTSock btsocks, BTSock btsockc) { // TODO make class
	auto mac = btsocks.getRemoteAddress().toString();
	auto name = btsocks.getRemoteAddress().getName();
	std::cout << "Connected: " << name << " (" << mac << ")\n";

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

void wait_any_key_to_exit() {
#ifdef WIN32
	char c;
	std::cout << "Press any key to exit\n";
	std::cin >> c;
#endif

	exit(0);
}

int main() {
#ifdef WIN32
	SetConsoleOutputCP(CP_UTF8);
#endif

	BTAdapter adapter;
	if (!adapter.isThere()) {
		std::cout << "BT adapter not found\n";
		wait_any_key_to_exit();
	}

	if (!adapter.isOn()) {
		std::cout << "BT is turn off, trying to turn on... ";

		if (adapter.setOn(true))
			std::cout << "done\n";
		else {
			std::cout << "not allowed\n";
			std::cout << "Turn on BT manually and try again\n";
			wait_any_key_to_exit();
		}
	}

	std::cout << "BT name: " << adapter.getName() << '\n';
	std::cout << "BT mac: " << adapter.getAddress().toString() << "\n";

	BTSockListener btsockl;
	btsockl.bind(obex_id);

	std::cout << "BT start listening\n";
	while (true) {
		BTSock btsocks, btsockc;
		btsockl.accept(btsocks, true);
		btsockc.connect(btsockl.getShortId(), btsocks.getRemoteAddress());

		threads.push_back(std::thread(worker, btsocks, btsockc));
	}

	wait_any_key_to_exit();
}