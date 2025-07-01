#pragma once
#include <stdint.h>
#include <string.h>
#include <vector>

typedef std::vector<uint8_t> vec;

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