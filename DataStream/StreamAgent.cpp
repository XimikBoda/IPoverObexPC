#include "StreamAgent.h"
#include "Stream.h"

namespace DS {

	bool StreamAgent::sdsa_connect(Stream* sdsa_p, bool connect_both) {
		if (this->sdsa_p)
			return false;

		if (connect_both)
			if (!sdsa_p->sds_connect(this, false))
				return false;

		this->sdsa_p = sdsa_p;
		return true;
	}

	void StreamAgent::sdsa_close(bool close_both) {
		if (sdsa_p) {
			if (close_both)
				sdsa_p->sds_close(false);
			sdsa_p = nullptr;
		}
	}

	void StreamAgent::write(const void* buf, size_t len) {
		if (!sdsa_p)
			return;

		return sdsa_p->sds_write(buf, len);
	}

	void StreamAgent::write(const vec &buf) {
		write(buf.data(), buf.size());
	}

	StreamAgent::~StreamAgent() {
		sdsa_close();
	}

}