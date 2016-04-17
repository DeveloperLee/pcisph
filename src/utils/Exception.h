#pragma once

#include <exception>
#include <tinyformat.h>

namespace cs224 {

class Exception : public std::runtime_error {
public:
    template<typename... Args>
    Exception(const char *fmt, const Args &... args) :
        std::runtime_error(tfm::format(fmt, args...))
    {}
};
}
