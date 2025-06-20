#pragma once

namespace DataStream {
	typedef class ReaderAgent;

	class Reader {
		ReaderAgent* sdr_agent = nullptr;
	public:
		void sdr_connect(ReaderAgent* sdr_agent);
		void sdr_close(bool close_both = true);

		virtual bool sdr_read(void* buf, size_t len, size_t& readed);

		~Reader();
	};

};