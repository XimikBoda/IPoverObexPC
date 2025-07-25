#pragma once
#include "PacketReader.h"

#include <cstdint>
#include <memory>


class Log {
	friend class StreamToIP;

	enum class Act : uint8_t {
		Post,
	};

	enum class Level : uint8_t {
		None,
		Trace,
		Debug,
		Info,
		Warning,
		Error,
		Critical,
	};

	const char* LevelNames[7] = { "", "Trace", "Debug",
		"Info", "Warning", "Error", "Critical" };

	class StreamToIP& owner;
	PacketReader& reader;

	Log(class StreamToIP& owner_);

	void parsePacket();
	void parsePostPacket();

};