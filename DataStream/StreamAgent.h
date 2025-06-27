#pragma once
#include "DataStream.h"

namespace DS {
	class StreamAgent {
		class Stream* sdsa_p = nullptr;
	public:
		bool sdsa_connect(class Stream* sdsa_p, bool connect_both = true);
		void sdsa_close(bool close_both = true);


		void write(void* buf, size_t len);
		void write(vec &buf);

		~StreamAgent();
	};

};