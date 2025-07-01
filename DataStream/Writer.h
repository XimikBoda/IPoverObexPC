#pragma once
#include "DataStream.h"

namespace DS {
	class Writer {
		class WriterAgent* sdw_agent = nullptr;
	public:
		bool sdw_connect(class WriterAgent* sdr_agent, bool connect_both = false);
		void sdw_close(bool close_both = true);

		virtual void setWriteBlocking(DS::AccessMode mode);

		virtual size_t write(const void* buf, size_t len) = 0;
		virtual size_t write(const vec &buf) = 0;

		~Writer();
	};

};