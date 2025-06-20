#pragma once
#include "DataStream.h"

namespace DataStream {
	typedef class Writer;

	class WriterAgent {
		Writer* sdwa_p = nullptr;
	public:
		bool sdwa_connect(Writer* sdwa_p, bool connect_both = false);
		void sdwa_close(bool close_both = true);

		bool write(void* buf, size_t len, size_t& writed);
		void write(vec buf);

		~WriterAgent();
	};

};