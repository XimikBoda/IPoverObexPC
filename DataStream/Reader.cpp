#include "Reader.h"
#include "ReaderAgent.h"

namespace DS {

	bool Reader::sdr_connect(ReaderAgent* sdr_agent, bool connect_both) {
		if (this->sdr_agent)
			return false;

		if (connect_both)
			if (!sdr_agent->sdra_connect(this, false))
				return false;

		this->sdr_agent = sdr_agent;
		return true;
	}

	void Reader::sdr_close(bool close_both) {
		if (sdr_agent) {
			if (close_both)
				sdr_agent->sdra_close(false);
			sdr_agent = nullptr;
		}
	}

	Reader::~Reader() {
		sdr_close();
	}

}