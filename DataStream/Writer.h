#pragma once
#include "DataStream.h"

namespace DS {
	class Writer {
		class WriterAgent* sdw_agent = nullptr;
	public:
		bool sdw_connect(class WriterAgent* sdr_agent, bool connect_both = false);
		void sdw_close(bool close_both = true);

		virtual bool sdw_write(void* buf, size_t len, size_t& writed) = 0;
		virtual void sdw_write(vec buf) = 0;

		~Writer();
	};

};