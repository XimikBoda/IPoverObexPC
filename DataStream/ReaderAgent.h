#pragma once
#include "DataStream.h"

namespace DS {
	class ReaderAgent {
		class Reader* sdra_p = nullptr;

	public:
		bool sdra_connect(class Reader* sdra_p, bool connect_both = false);
		void sdra_close(bool close_both = true);

		void setReadBlocking(DS::AccessMode mode);

		size_t read(void* buf, size_t len);
		const vec& read(size_t len);

		~ReaderAgent();
	};
};