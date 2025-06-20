#include "Stream.h"
#include "StreamAgent.h"

namespace DataStream {

	bool Stream::sds_connect(StreamAgent* sds_agent, bool connect_both) {
		if (this->sds_agent)
			return false;

		if (connect_both)
			if (!sds_agent->sdsa_connect(this, false))
				return false;

		this->sds_agent = sds_agent;
		return true;
	}

	void Stream::sds_close(bool close_both) {
		if (sds_agent) {
			if (close_both)
				sds_agent->sdsa_close(false);
			sds_agent = nullptr;
		}
	}

	void Stream::read(void* buf, size_t len, size_t& readed) {
		std::unique_lock lk(sds_mutex);
		lk.lock();
		if (sds_buf.size() < len) {
			lk.unlock();
			sds_cv.wait(lk, [&] { return sds_buf.size() >= len; });
			lk.lock();
		}
		readed = len;
		memcpy(buf, sds_buf.data(), len);
		memmove(sds_buf.data(), sds_buf.data() + len, sds_buf.size() - len);
		sds_buf.resize(sds_buf.size() - len);
		lk.unlock();
	}


	void Stream::sds_write(void* buf, size_t len) {
		std::unique_lock lk(sds_mutex);
		lk.lock();

		size_t old_len = sds_buf.size();
		sds_buf.resize(old_len + len);
		memcpy(sds_buf.data() + old_len, buf, len);

		lk.unlock();
		sds_cv.notify_one();
	}

	Stream::~Stream() {
		sds_close(true);
	}
};