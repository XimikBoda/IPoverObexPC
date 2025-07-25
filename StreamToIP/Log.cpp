#include "Log.h"
#include "StreamToIP.h"
#include <iostream>

using namespace std::string_literals;

Log::Log(StreamToIP& owner_) : owner(owner_), reader(owner.reader) {}

void Log::parsePacket() {
	Act act = (Act)reader.readUInt8();

	switch (act) {
	case Act::Post:
		parsePostPacket();
		break;
	}
}

static std::string add_sep(const std::string& str) {
	if (str.length())
		return str + ":"s;
}

void Log::parsePostPacket() {
	uint8_t level = reader.readUInt8();
	std::string level_name = LevelNames[level];

	std::string app_name = reader.readString();
	std::string message = reader.readString();

	if (!message.size())
		return;
	if (message[message.size() - 1] == '\n')
		message.resize(message.size() - 1);

	std::cout 
		<< add_sep(owner.name)
		<< add_sep(app_name)
		<< add_sep(level_name) << " "
		<< message << '\n';
}