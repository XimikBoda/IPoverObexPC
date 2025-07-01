#include "ReaderAgent.h"
#include "Reader.h"

namespace DS {
	const vec null_vec = {};

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

	void ReaderAgent::setReadBlocking(DS::AccessMode mode) {
		if (!sdra_p)
			return;

		sdra_p->setReadBlocking(mode);
	}

	size_t ReaderAgent::read(void* buf, size_t len) {
		if (!sdra_p)
			throw DS::DataException();

		return sdra_p->read(buf, len);
	}

	const vec& ReaderAgent::read(size_t len) {
		if (!sdra_p)
			throw DS::DataException();

		return sdra_p->read(len);
	}

	ReaderAgent::~ReaderAgent() {
		sdra_close(true);
	}

};