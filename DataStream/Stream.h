#pragma once
#include "DataStream.h"
#include <mutex>
#include <condition_variable>

namespace DataStream {
	typedef class StreamAgent;

	class Stream {
		StreamAgent* sds_agent = nullptr;

		std::mutex sds_mutex;
		std::condition_variable sds_cv;

		vec sds_buf;

	public:
		bool sds_connect(StreamAgent* sds_agent, bool connect_both = true);
		void sds_close(bool close_both = true);

		void read(void* buf, size_t len, size_t& readed);
		vec read(size_t len);
		vec readAll();
		void sds_write(const void* buf, size_t len);

		~Stream();
	};

};