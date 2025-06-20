#include "ReaderAgent.h"
#include "Reader.h"

namespace DataStream {

	void ReaderAgent::sdra_connect(Reader* sdra_p) {
		if (this->sdra_p)
			sdra_close();
		this->sdra_p = sdra_p;
	}

	void ReaderAgent::sdra_close(bool close_both) {
		if (sdra_p) {
			if (close_both)
				sdra_p->sdr_close(false);
			sdra_p = nullptr;
		}
	}

	ReaderAgent::~ReaderAgent() {
		sdra_close(true);
	}

};