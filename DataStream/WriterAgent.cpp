#include "WriterAgent.h"
#include "Writer.h"

namespace DS {

	bool WriterAgent::sdwa_connect(Writer* sdwa_p, bool connect_both) {
		if (this->sdwa_p)
			return false;

		if (connect_both)
			if (!sdwa_p->sdw_connect(this, false))
				return false;

		this->sdwa_p = sdwa_p;
		return true;
	}

	void WriterAgent::sdwa_close(bool close_both) {
		if (sdwa_p) {
			if (close_both)
				sdwa_p->sdw_close(false);
			sdwa_p = nullptr;
		}
	}

	void WriterAgent::setWriteBlocking(DS::AccessMode mode) {
		if (!sdwa_p)
			return;

		sdwa_p->setWriteBlocking(mode);
	}

	size_t WriterAgent::write(const void* buf, size_t len) {
		if (!sdwa_p)
			throw DS::DataException();

		return sdwa_p->write(buf, len);
	}

	size_t WriterAgent::write(const vec& buf) {
		if (!sdwa_p)
			throw DS::DataException();

		return sdwa_p->write(buf);
	}

	WriterAgent::~WriterAgent() {
		sdwa_close(true);
	}

};