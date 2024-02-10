#pragma once

#include <ostream>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

namespace rr::vk {

class Vulkan {
public:
    Vulkan();
    ~Vulkan();

    Vulkan(const Vulkan&) = delete;
    Vulkan(Vulkan&&) = delete;
    Vulkan& operator=(const Vulkan&) = delete;
    Vulkan& operator=(Vulkan&&) = delete;
};

std::vector<VkLayerProperties> enumerateInstanceLevelProperties();
std::vector<VkExtensionProperties> enumerateInstanceExtensionProperties();

void printVkQueueFlags(std::ostream&, VkQueueFlags);

template <class T, void printer(std::ostream&, T)>
class Print {
public:
    Print(T value)
        : _value(value)
    { }

    friend std::ostream& operator<<(std::ostream& output, const Print& p)
    {
        printer(output, p._value);
        return output;
    }

private:
    T _value;
};

using PrintVkQueueFlags = Print<VkQueueFlags, printVkQueueFlags>;

inline PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr;

#define GLOBAL(NAME) inline PFN_##NAME NAME;
#include <vk/vulkan-functions.hpp>

} // namespace rr::vk
