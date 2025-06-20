#pragma once
#include <vector>
#include <mutex>
#include <condition_variable>

namespace DataStream {
	typedef class StreamAgent;

	class Stream {
		StreamAgent* sds_agent = nullptr;

		std::mutex sds_mutex;
		std::condition_variable sds_cv;

		std::vector<uint8_t> sds_buf;

	public:
		bool sds_connect(StreamAgent* sds_agent, bool connect_both = false);
		void sds_close(bool close_both = true);

		void read(void* buf, size_t len, size_t& readed);
		void sds_write(void* buf, size_t len);

		~Stream();
	};

};