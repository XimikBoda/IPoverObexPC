#pragma once

namespace DataStream {
	typedef class Reader;

	class ReaderAgent {
		Reader* sdra_p = nullptr;
	public:
		void sdra_connect(Reader* sdra_p);
		void sdra_close(bool close_both = true);

		~ReaderAgent();
	};

};