#pragma once
#include "DataStream.h"

namespace DataStream {
	typedef class WriterAgent;

	class Writer {
		WriterAgent* sdw_agent = nullptr;
	public:
		bool sdw_connect(WriterAgent* sdr_agent, bool connect_both = false);
		void sdw_close(bool close_both = true);

		virtual bool sdw_write(void* buf, size_t len, size_t& writed) = 0;
		virtual void sdw_write(vec buf) = 0;

		~Writer();
	};

};