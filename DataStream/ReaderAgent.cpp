#include "ReaderAgent.h"
#include "Reader.h"

namespace DS {

	bool ReaderAgent::sdra_connect(Reader* sdra_p, bool connect_both) {
		if (this->sdra_p)
			return false;

		if (connect_both)
			if (!sdra_p->sdr_connect(this, false))
				return false;

		this->sdra_p = sdra_p;
		return true;
	}

	void ReaderAgent::sdra_close(bool close_both) {
		if (sdra_p) {
			if (close_both)
				sdra_p->sdr_close(false);
			sdra_p = nullptr;
		}
	}

	bool ReaderAgent::read(void* buf, size_t len, size_t& readed){
		if (!sdra_p)
			return false;

		return sdra_p->sdr_read(buf, len, readed);
	}

	std::vector<uint8_t> ReaderAgent::read(size_t len) {
		if (!sdra_p)
			return {};

		return sdra_p->sdr_read(len);
	}

	ReaderAgent::~ReaderAgent() {
		sdra_close(true);
	}

};