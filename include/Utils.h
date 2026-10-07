#pragma once
// Utils.h - tiny string helpers used by the file parsers.
#include <algorithm>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

namespace util {
inline std::string trim(const std::string& s) {
    size_t a = 0, b = s.size();
    while (a < b && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    while (b > a && std::isspace(static_cast<unsigned char>(s[b - 1]))) --b;
    return s.substr(a, b - a);
}

inline std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

inline std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> out;
    std::string part;
    std::stringstream ss(s);
    while (std::getline(ss, part, delim)) {
        part = trim(part);
        if (!part.empty()) out.push_back(part);
    }
    return out;
}

// Lets level files write "\n" for a line break inside one line of text.
inline std::string unescapeNewlines(std::string s) {
    size_t pos = 0;
    while ((pos = s.find("\\n", pos)) != std::string::npos) {
        s.replace(pos, 2, "\n");
        pos += 1;
    }
    return s;
}
}  // namespace util
