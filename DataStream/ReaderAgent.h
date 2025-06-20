#pragma once
#include <vector>

namespace DataStream {
	typedef class Reader;

	class ReaderAgent {
		Reader* sdra_p = nullptr;
	public:
		bool sdra_connect(Reader* sdra_p, bool connect_both = false);
		void sdra_close(bool close_both = true);

		bool read(void* buf, size_t len, size_t& readed);
		std::vector<uint8_t> read(size_t len);

		~ReaderAgent();
	};

};