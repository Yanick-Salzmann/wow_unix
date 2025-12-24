#ifndef GLAD_COMMON_UTILS_HPP
#define GLAD_COMMON_UTILS_HPP

#include <memory>

#define SHARED_PTR_FUNC_NAME(T) inline std::shared_ptr<T> make_##T() { return std::make_shared<T>(); }

#endif //GLAD_COMMON_UTILS_HPP