#pragma once

#include <string>
#include <cstring>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <vector>

namespace cs224 {
namespace StringUtils {

// Converts a string into lower case.
static std::string lower(const std::string &str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Converts a string into upper case.
static std::string upper(const std::string &str) {
    std::string result(str);
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Converts a string into an unsigned int value.
// If the input string isn't an unsigned int value, throws an exception.
static unsigned int toUInt(const std::string &str) {
    char *end_ptr = nullptr;
    unsigned int result = (int) strtoul(str.c_str(), &end_ptr, 10);
    return result;
}

static std::vector<std::string> tokenize(const std::string &string, const std::string &delim, bool includeEmpty) {
    std::string::size_type lastPos = 0, pos = string.find_first_of(delim, lastPos);
    std::vector<std::string> tokens;

    while (lastPos != std::string::npos) {
        if (pos != lastPos || includeEmpty)
            tokens.emplace_back(string.substr(lastPos, pos - lastPos));
        lastPos = pos;
        if (lastPos != std::string::npos) {
            lastPos += 1;
            pos = string.find_first_of(delim, lastPos);
        }
    }

    return tokens;
}

} 
} 
