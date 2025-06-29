#include "Stream.h"
#include "StreamAgent.h"

namespace DS {

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
			sds_cv.notify_one();
		}
	}

	ssize_t Stream::read(void* buf, size_t len, DS::AccessMode mode) {
		std::unique_lock lk(sds_mutex);

		size_t available_size = sds_buf.size();
		size_t lack_of_size = len > available_size ? len - available_size : 0;

		if (mode == DS::Blocking) {
			if (lack_of_size) sds_cv.wait(lk, [&] { return sds_buf.size() >= len || !sds_agent; });
		}
		else if (mode == DS::BlockingPartial) {
			if (!available_size) sds_cv.wait(lk, [&] { return sds_buf.size() || !sds_agent; });
		}

		if (!sds_agent)
			throw DS::DataException();

		available_size = sds_buf.size();

		if (len > available_size)
			len = available_size;

		if (available_size) {
			memcpy(buf, sds_buf.data(), len);
			memmove(sds_buf.data(), sds_buf.data() + len, sds_buf.size() - len);

			sds_buf.resize(sds_buf.size() - len);
		}

		return available_size;
	}

	const vec& Stream::read(size_t len, DS::AccessMode mode) {
		static thread_local vec res;

		res.resize(len);

		if (len) {
			ssize_t readed = read(res.data(), len, mode);
			res.resize(readed);
		}

		return res;
	}

	vec Stream::readAll() {
		std::unique_lock lk(sds_mutex);

		std::vector<uint8_t> res = sds_buf;
		sds_buf.resize(0);

		return res;
	}

	void Stream::sds_write(const void* buf, size_t len) {
		std::unique_lock lk(sds_mutex);

		size_t old_len = sds_buf.size();
		sds_buf.resize(old_len + len);
		memcpy(sds_buf.data() + old_len, buf, len);

		sds_cv.notify_one();
	}

	Stream::~Stream() {
		sds_close(true);
	}
};