#pragma once
#include "DataStream.h"

namespace DS {
	class Reader {
		class ReaderAgent* sdr_agent = nullptr;
	public:
		bool sdr_connect(class ReaderAgent* sdr_agent, bool connect_both = false);
		void sdr_close(bool close_both = true);

		virtual bool sdr_read(void* buf, size_t len, size_t& readed) = 0;
		virtual vec sdr_read(size_t len) = 0;

		~Reader();
	};

};