#include "string_utils.h"

namespace wow::utils {
    std::string to_lower(const std::string &str) {
        std::string result{str};
        std::ranges::transform(result, result.begin(), tolower);
        return result;
    }

    std::string to_upper(const std::string &str) {
        std::string result{str};
        std::ranges::transform(result, result.begin(), toupper);
        return result;
    }
}
