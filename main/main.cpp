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
#include <SFML/System/Sleep.hpp>


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
	stip.name = name;

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

void workerTCP(std::shared_ptr<sf::TcpSocket> sock) { // TODO make class
	auto name = sock->getRemoteAddress()->toString() + ":" 
		+ std::to_string(sock->getRemotePort());
	std::cout << "Connected TCP: " << name << '\n';

	DS::StreamAgent writer;
	DS::Stream reader;

	std::thread in([&]() {
		uint8_t buf[64 * 1024];
		size_t recived = 0;
		while (true) {
			auto res = sock->receive(buf, 64 * 1024, recived);
			if (res != sf::Socket::Status::Done)
				break;
			writer.write(buf, recived);
		}
		writer.sdsa_close();
		});

	std::thread out([&]() {
		while (true) {
			auto buf = reader.readAll(DS::BlockingPartial);
			if (!buf.size())
				break;
			auto res = sock->send(buf.data(), buf.size());
			if (res != sf::Socket::Status::Done)
				break;
		}
		reader.sds_close();
		});

	StreamToIP stip;
	stip.reader.sds_connect(&writer);
	stip.writer.sdsa_connect(&reader);
	stip.name = name;

	stip.run();

	stip.wait();

	sock->disconnect();
	if (in.joinable())
		in.join();
	if (out.joinable())
		out.join();

	std::cout << "Disconnected: " << name << '\n';
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

	std::thread tcp([&]() {
		sf::TcpListener tcp_listener;
		if (tcp_listener.listen(obex_id) == sf::Socket::Status::Done)
			std::cout << "TCP bind " << obex_id << " port\n";
		else {
			std::cout << "TCP can`t bind " << obex_id << " port\n";
			return;
		}
		while (true) {
			std::shared_ptr<sf::TcpSocket> sock = std::make_shared<sf::TcpSocket>();
			auto res = tcp_listener.accept(*sock);
			if (res == sf::Socket::Status::Done)
				threads.push_back(std::thread(workerTCP, sock));
			else
				return;
		}
		});

	std::thread bt([&]() {
		BTAdapter adapter;
		if (!adapter.isThere())

			if (!adapter.isOn()) {
				std::cout << "BT is turn off, trying to turn on... ";

				if (adapter.setOn(true)) {
					std::cout << "done\n";
					std::cout << "Waiting 5 seconds for full BT initialization...\n";
					sf::sleep(sf::seconds(5));
				}
				else {
					std::cout << "not allowed\n";
					std::cout << "Turn on BT manually and try again\n";
					return;
				}
			}

		std::cout << "BT name: " << adapter.getName() << '\n';
		std::cout << "BT mac: " << adapter.getAddress().toString() << "\n";

		BTSockListener btsockl;
		if (!btsockl.bind(obex_id)) {
			std::cout << "Failed to bind BT service, please close other application "
				<< "that may be using this service (OBEX/File transfer) and try again.\n";
			return;
		}


		std::cout << "BT start listening\n";
		while (true) {
			BTSock btsocks, btsockc;
			btsockl.accept(btsocks, true);
			btsockc.connect(btsockl.getShortId(), btsocks.getRemoteAddress());

			threads.push_back(std::thread(worker, btsocks, btsockc));
		}
		});

	tcp.join();
	bt.join();

	wait_any_key_to_exit();
}