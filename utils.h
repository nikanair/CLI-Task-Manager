#pragma once
#include <string>
#include <vector>

namespace Utils {
    std::vector<std::string> splitCSV(const std::string& line);
    std::string escapeCSV(const std::string& s);
    std::string unescapeCSV(const std::string& s);
    int stoiSafe(const std::string& s, int fallback = 0);
    std::string trim(const std::string& s);
}
