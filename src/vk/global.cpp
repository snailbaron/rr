#include <vk/global.hpp>

#include <vk/check.hpp>

#include <error.hpp>

#include <dlfcn.h>

#include <cstdint>
#include <source_location>

namespace rr::vk {

namespace {

void* _vulkanLibrary = nullptr;

} // namespace

Vulkan::Vulkan()
{
    if (_vulkanLibrary) {
        return;
    }

    static const char* vulkanLibraryName = "libvulkan.so.1";
    _vulkanLibrary = dlopen(vulkanLibraryName, RTLD_LAZY);
    if (!_vulkanLibrary) {
        throw Error{} << "could not load vulkan library: " << vulkanLibraryName;
    }

    vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        dlsym(_vulkanLibrary, "vkGetInstanceProcAddr"));
    if (!vkGetInstanceProcAddr) {
        throw Error{} << "could not load vulkan function: vkGetInstanceProcAddr";
    }

#define GLOBAL(NAME) \
    NAME = reinterpret_cast<PFN_##NAME>(vkGetInstanceProcAddr(nullptr, #NAME)); \
    if (!(NAME)) { \
        throw Error{} << "could not load vulkan function: " #NAME; \
    }
#include <vk/vulkan-functions.hpp>

}

Vulkan::~Vulkan()
{
    if (_vulkanLibrary) {
        dlclose(_vulkanLibrary);
    }
}

std::vector<VkLayerProperties> enumerateInstanceLevelProperties()
{
    uint32_t layerCount = 0;
    check << vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    auto layerProperties = std::vector<VkLayerProperties>(layerCount);
    check << vkEnumerateInstanceLayerProperties(
        &layerCount, layerProperties.data());
    return layerProperties;
}

std::vector<VkExtensionProperties> enumerateInstanceExtensionProperties()
{
    uint32_t extensionCount = 0;
    check << vkEnumerateInstanceExtensionProperties(
        nullptr, &extensionCount, nullptr);
    auto extensionProperties =
        std::vector<VkExtensionProperties>(extensionCount);
    check << vkEnumerateInstanceExtensionProperties(
        nullptr, &extensionCount, extensionProperties.data());
    return extensionProperties;
}

void printVkQueueFlags(std::ostream& output, VkQueueFlags vkQueueFlags)
{
    bool started = false;
    auto append = [&output, &started, &vkQueueFlags] (
        VkQueueFlagBits bit, const char* name)
    {
        if (vkQueueFlags & bit) {
            if (started) {
                output << "|";
            } else {
                started = true;
            }
            output << name;
        }
    };

    append(VK_QUEUE_GRAPHICS_BIT, "VK_QUEUE_GRAPHICS_BIT");
    append(VK_QUEUE_COMPUTE_BIT, "VK_QUEUE_COMPUTE_BIT");
    append(VK_QUEUE_TRANSFER_BIT, "VK_QUEUE_TRANSFER_BIT");
    append(VK_QUEUE_SPARSE_BINDING_BIT, "VK_QUEUE_SPARSE_BINDING_BIT");
    append(VK_QUEUE_PROTECTED_BIT, "VK_QUEUE_PROTECTED_BIT");
    append(VK_QUEUE_VIDEO_DECODE_BIT_KHR, "VK_QUEUE_VIDEO_DECODE_BIT_KHR");
    append(VK_QUEUE_VIDEO_ENCODE_BIT_KHR, "VK_QUEUE_VIDEO_ENCODE_BIT_KHR");
    append(VK_QUEUE_OPTICAL_FLOW_BIT_NV, "VK_QUEUE_OPTICAL_FLOW_BIT_NV");
    append(VK_QUEUE_FLAG_BITS_MAX_ENUM, "VK_QUEUE_FLAG_BITS_MAX_ENUM");
}

} // namespace rr::vk
