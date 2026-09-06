#include "utils/Utils.h"

#include <sstream>
#include <algorithm>
#include <cctype>
#include <iomanip>

namespace utils {

std::vector<std::string> split(const std::string& s, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream ss(s);
    while (std::getline(ss, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

std::vector<int64_t> parseNodeIdList(const std::string& s) {
    std::vector<int64_t> ids;
    for (const auto& tok : split(s, ',')) {
        try {
            ids.push_back(std::stoll(trim(tok)));
        } catch (...) {}
    }
    return ids;
}

std::string formatDistance(double metres) {
    std::ostringstream oss;
    if (metres >= 1000.0) {
        oss << std::fixed << std::setprecision(2) << (metres / 1000.0) << " km";
    } else {
        oss << std::fixed << std::setprecision(1) << metres << " m";
    }
    return oss.str();
}

std::string formatDuration(double seconds) {
    std::ostringstream oss;
    if (seconds >= 3600.0) {
        oss << std::fixed << std::setprecision(2) << (seconds / 3600.0) << " h";
    } else if (seconds >= 60.0) {
        oss << std::fixed << std::setprecision(1) << (seconds / 60.0) << " min";
    } else {
        oss << std::fixed << std::setprecision(1) << seconds << " s";
    }
    return oss.str();
}

} // namespace utils
