#include "WriterAgent.h"
#include "Writer.h"

namespace DataStream {

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

	bool WriterAgent::write(void* buf, size_t len, size_t& writed){
		if (!sdwa_p)
			return false;

		return sdwa_p->sdw_write(buf, len, writed);
	}

	void WriterAgent::write(std::vector<uint8_t> buf) {
		if (!sdwa_p)
			return;

		return sdwa_p->sdw_write(buf);
	}

	WriterAgent::~WriterAgent() {
		sdwa_close(true);
	}

};