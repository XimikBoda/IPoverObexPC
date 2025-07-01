#pragma once
#include "DataStream.h"

namespace DS {
	class Reader {
		class ReaderAgent* sdr_agent = nullptr;
	public:
		bool sdr_connect(class ReaderAgent* sdr_agent, bool connect_both = false);
		void sdr_close(bool close_both = true);

		virtual void setReadBlocking(DS::AccessMode mode) = 0;

		virtual size_t read(void* buf, size_t len) = 0;
		virtual const vec& read(size_t len) = 0;

		~Reader();
	};
};