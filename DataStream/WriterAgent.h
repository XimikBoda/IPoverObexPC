#pragma once
#include "DataStream.h"

namespace DS {
	class WriterAgent {
		class Writer* sdwa_p = nullptr;
	public:
		bool sdwa_connect(class Writer* sdwa_p, bool connect_both = false);
		void sdwa_close(bool close_both = true);

		void setWriteBlocking(DS::AccessMode mode);

		ssize_t write(const void* buf, size_t len);
		ssize_t write(const vec &buf);

		~WriterAgent();
	};

};