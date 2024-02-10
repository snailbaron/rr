#pragma once

#include <vulkan/vulkan.h>

#include <source_location>

namespace rr::vk {

class Check {
public:
    Check(std::source_location sl = std::source_location::current());

    void operator<<(VkResult vkResult);

private:
    std::source_location _sourceLocation;
};

#define check Check{}

} // namespace rr::vk
