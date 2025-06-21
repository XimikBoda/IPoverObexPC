#pragma once
#include "DataStream.h"

namespace DataStream {
	typedef class Stream;

	class StreamAgent {
		Stream* sdsa_p = nullptr;
	public:
		bool sdsa_connect(Stream* sdsa_p, bool connect_both = false);
		void sdsa_close(bool close_both = true);


		void write(void* buf, size_t len);

		~StreamAgent();
	};

};