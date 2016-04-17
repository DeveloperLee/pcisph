#pragma once

#include <string>
#include <cstring>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <vector>

#include "Exception.h"

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
    if (*end_ptr != '\0')
        throw Exception("Could not parse integer value \"%s\"", str);
    return result;
}

// Converts millisecond into a readable time string.
static std::string timeString(double time, bool precise) {
    if (std::isnan(time) || std::isinf(time))
        return "inf";

    std::string suffix = "ms";
    if (time > 1000) {
        time /= 1000; suffix = "s";
        if (time > 60) {
            time /= 60; suffix = "m";
            if (time > 60) {
                time /= 60; suffix = "h";
                if (time > 12) {
                    time /= 12; suffix = "d";
                }
            }
        }
    }

    std::ostringstream os;
    os << std::setprecision(precise ? 4 : 1)
       << std::fixed << time << suffix;

    return os.str();
}

static std::string indent(const std::string &string, int amount = 2) {

    std::istringstream iss(string);
    std::ostringstream oss;
    std::string spacer(amount, ' ');
    bool firstLine = true;
    for (std::string line; std::getline(iss, line); ) {
        if (!firstLine)
            oss << spacer;
        oss << line;
        if (!iss.eof())
            oss << std::endl;
        firstLine = false;
    }
    return oss.str();
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
