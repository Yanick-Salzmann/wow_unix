#ifndef WOW_UNIX_LOG_UTILS_H
#define WOW_UNIX_LOG_UTILS_H

#include <spdlog/fmt/bundled/format.h>
#include <glm/vec3.hpp>
#include "glm/vec2.hpp"

template<>
struct fmt::formatter<glm::vec3> {
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const glm::vec3& input, FormatContext &ctx) const -> decltype(ctx.out()) {
        return format_to(ctx.out(),
                         "(x={}, y={}, z={})",
                         input.x, input.y, input.z);
    }
};

template<>
struct fmt::formatter<glm::vec2> {
    constexpr auto parse(format_parse_context &ctx) -> decltype(ctx.begin()) {
        return ctx.end();
    }

    template<typename FormatContext>
    auto format(const glm::vec2& input, FormatContext &ctx) const -> decltype(ctx.out()) {
        return format_to(ctx.out(),
                         "(x={}, y={})",
                         input.x, input.y);
    }
};

#endif //WOW_UNIX_LOG_UTILS_H
