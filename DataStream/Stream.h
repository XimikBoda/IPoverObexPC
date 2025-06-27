#pragma once
#include "DataStream.h"
#include <mutex>
#include <condition_variable>

namespace DS {
	class Stream {
		class StreamAgent* sds_agent = nullptr;

		std::mutex sds_mutex;
		std::condition_variable sds_cv;

		vec sds_buf;

	public:
		bool sds_connect(class StreamAgent* sds_agent, bool connect_both = true);
		void sds_close(bool close_both = true);

		ssize_t read(void* buf, size_t len, DS::AccessMode mode);
		const vec& read(size_t len, DS::AccessMode mode);
		vec readAll();
		void sds_write(const void* buf, size_t len);

		~Stream();
	};

};