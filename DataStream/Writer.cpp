#include "Writer.h"
#include "WriterAgent.h"

namespace DS {

	bool Writer::sdw_connect(WriterAgent* sdw_agent, bool connect_both) {
		if (this->sdw_agent)
			return false;

		if (connect_both)
			if (!sdw_agent->sdwa_connect(this, false))
				return false;

		this->sdw_agent = sdw_agent;
		return true;
	}

	void Writer::sdw_close(bool close_both) {
		if (sdw_agent) {
			if (close_both)
				sdw_agent->sdwa_close(false);
			sdw_agent = nullptr;
		}
	}

	Writer::~Writer() {
		sdw_close();
	}

	void Writer::setWriteBlocking(DS::AccessMode mode) {}
}