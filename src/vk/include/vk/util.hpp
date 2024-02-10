#pragma once

#include <vk/check.hpp>

#include <concepts>
#include <cstdint>
#include <type_traits>
#include <utility>
#include <vector>

namespace rr::vk {

template <class Result, class Function, class... Args>
[[nodiscard]] Result getVkObject(Function function, Args&&... args)
requires
    std::is_pointer_v<Function> &&
    std::invocable<Function, Args..., Result*>
{
    using CallResult = std::invoke_result_t<Function, Args..., Result*>;
    constexpr bool needsCheck = !std::is_void<CallResult>();

    auto result = Result{};

    if constexpr (needsCheck) {
        check << function(std::forward<Args>(args)..., &result);
    } else {
        function(std::forward<Args>(args)..., &result);
    }

    return result;
}

template <class Result, class Function, class... Args>
requires
    std::is_pointer_v<Function> &&
    std::invocable<Function, Args..., uint32_t*, Result*>
[[nodiscard]] std::vector<Result> getVkObjects(
    Function function, Args&&... args)
{
    using CallResult =
        std::invoke_result_t<Function, Args..., uint32_t*, Result*>;
    constexpr bool needsCheck = !std::is_void<CallResult>();

    uint32_t count = 0;
    if constexpr (needsCheck) {
        check << function(std::forward<Args>(args)..., &count, nullptr);
    } else {
        function(std::forward<Args>(args)..., &count, nullptr);
    }

    auto results = std::vector<Result>(count);
    if constexpr (needsCheck) {
        check << function(std::forward<Args>(args)..., &count, results.data());
    } else {
        function(std::forward<Args>(args)..., &count, results.data());
    }

    return results;
}

} // namespace rr::vk
