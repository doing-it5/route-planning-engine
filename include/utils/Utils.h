#ifndef UTILS_H
#define UTILS_H

#include <string>
#include <vector>
#include <cstdint>

namespace utils {

/// Split @p s on @p delimiter and return the tokens.
std::vector<std::string> split(const std::string& s, char delimiter);

/// Trim leading and trailing whitespace from @p s.
std::string trim(const std::string& s);

/// Parse a comma-separated list of int64 node IDs.
/// e.g. "123,456,789" → {123, 456, 789}
std::vector<int64_t> parseNodeIdList(const std::string& s);

/// Format a distance in metres as a human-readable string (m or km).
std::string formatDistance(double metres);

/// Format a duration in seconds as a human-readable string (s, min, h).
std::string formatDuration(double seconds);

} // namespace utils

#endif // UTILS_H
