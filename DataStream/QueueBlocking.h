#pragma once
#include <queue>
#include <mutex>
#include <condition_variable>

namespace DS {

	template<typename T>
	class QueueBlocking {
		std::queue<T> data;
		std::mutex data_mutex;
		std::condition_variable data_cv;

		bool end = false;
	public:
		void push(const T& v) {
			{
				std::lock_guard lk(data_mutex);
				data.push(v);
			}
			data_cv.notify_one();
		}

		bool pop(T& v) {
			std::unique_lock lk(data_mutex);

			data_cv.wait(lk, [&] { return !data.empty() || end; });

			if (end)
				return false;

			v = std::move(data.front());
			data.pop();

			return true;
		}

		void close() {
			std::lock_guard lk(data_mutex);

			end = true;
			data_cv.notify_all();
		}

		~QueueBlocking() {
			close();
		}
	};

}