#ifndef WOW_UNIX_STRING_UTILS_H
#define WOW_UNIX_STRING_UTILS_H

#include <algorithm>
#include <string>

namespace wow::utils {
    std::string to_lower(const std::string &str);

    std::string to_upper(const std::string &str);

    inline std::string ltrim(const std::string &str) {
        std::string result{str};
        result.erase(result.begin(), std::ranges::find_if(result, [](const auto ch) { return !std::isspace(ch); }));
        return result;
    }

    inline std::string rtrim(const std::string &str) {
        std::string result{str};
        result.erase(
            std::ranges::find_if(result.rbegin(), result.rend(), [](const auto ch) { return !std::isspace(ch); }).
            base(), result.end());
        return result;
    }

    inline std::string trim(const std::string &str) {
        return ltrim(rtrim(str));
    }

    inline bool starts_with(const std::string &str, const std::string &prefix) {
        if (str.size() < prefix.size()) {
            return false;
        }
        return str.substr(0, prefix.size()) == prefix;
    }

    inline std::string replace_all(std::string str, const std::string &from, const std::string &to) {
        size_t start_pos = 0;
        while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
            str.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return str;
    }
}

#endif //WOW_UNIX_STRING_UTILS_H
