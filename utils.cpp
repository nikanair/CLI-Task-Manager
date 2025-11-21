#include "Utils.h"
#include <sstream>
#include <algorithm>
#include <cctype>

namespace Utils {

std::string trim(const std::string& s) {
    size_t a = 0;
    while (a < s.size() && std::isspace(static_cast<unsigned char>(s[a]))) ++a;
    size_t b = s.size();
    while (b > a && std::isspace(static_cast<unsigned char>(s[b-1]))) --b;
    return s.substr(a, b - a);
}

int stoiSafe(const std::string& s, int fallback) {
    try {
        return std::stoi(s);
    } catch (...) {
        return fallback;
    }
}

std::string escapeCSV(const std::string& s) {
    bool needQuotes = s.find(',') != std::string::npos || s.find('"') != std::string::npos;
    std::string out = s;
    // double quotes escape
    size_t pos = 0;
    while ((pos = out.find('"', pos)) != std::string::npos) {
        out.replace(pos, 1, "\"\"");
        pos += 2;
    }
    if (needQuotes) {
        out = "\"" + out + "\"";
    }
    return out;
}

// Very small CSV parser for known format
std::vector<std::string> splitCSV(const std::string& line) {
    std::vector<std::string> result;
    std::string cur;
    bool inQuotes = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (inQuotes) {
            if (c == '"') {
                if (i + 1 < line.size() && line[i+1] == '"') {
                    cur.push_back('"'); // escaped quote
                    ++i;
                } else {
                    inQuotes = false; // closing quote
                }
            } else {
                cur.push_back(c);
            }
        } else {
            if (c == ',') {
                result.push_back(cur);
                cur.clear();
            } else if (c == '"') {
                inQuotes = true;
            } else {
                cur.push_back(c);
            }
        }
    }
    result.push_back(cur);
    return result;
}

std::string unescapeCSV(const std::string& s) {
    // Our splitCSV already returns unescaped content for quotes,
    // but ensure trimming
    return trim(s);
}

} // namespace Utils
