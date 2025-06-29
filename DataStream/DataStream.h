#pragma once
#include <stdint.h>
#include <string.h>
#include <vector>

typedef std::vector<uint8_t> vec;

#if __x86_64__ || _WIN64
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

    class DataException : public std::exception {
    private:
    public:
        DataException() = default;
    };
};