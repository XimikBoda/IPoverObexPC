#pragma once
#include <vector>

namespace DataStream {
	typedef class ReaderAgent;

	class Reader {
		ReaderAgent* sdr_agent = nullptr;
	public:
		bool sdr_connect(ReaderAgent* sdr_agent, bool connect_both = false);
		void sdr_close(bool close_both = true);

		virtual bool sdr_read(void* buf, size_t len, size_t& readed) = 0;
		virtual std::vector<uint8_t> sdr_read(size_t len) = 0;

		~Reader();
	};

};