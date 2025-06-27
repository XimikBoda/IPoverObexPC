#pragma once
#include <vector>

typedef std::vector<uint8_t> vec;

#ifdef _WIN64
typedef int64_t ssize_t;
#else
typedef int32_t ssize_t;
#endif

namespace DS {
	enum AccessMode : uint8_t {
		NonBlocking,
		BlockingPartial,
		Blocking
	};
};