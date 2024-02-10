#include <vk/instance.hpp>

#include <vk/check.hpp>
#include <vk/global.hpp>

#include <error.hpp>

namespace rr::vk {

Instance::Instance(
    const VkInstanceCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator)
{
    check << vkCreateInstance(createInfo, allocator, &_instance);

#define INSTANCE(NAME) \
    NAME = reinterpret_cast<PFN_##NAME>(vkGetInstanceProcAddr(_instance, #NAME)); \
    if (!(NAME)) { \
        throw Error{} << "could not load vulkan function: " #NAME; \
    }
#include <vk/vulkan-functions.hpp>
}

Instance::~Instance()
{
    vkDestroyInstance(_instance, nullptr);
}

VkDebugUtilsMessengerEXT Instance::createDebugUtilsMessengerEXT(
    const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    auto messenger = VkDebugUtilsMessengerEXT{};
    check <<vkCreateDebugUtilsMessengerEXT(
        _instance, createInfo, allocator, &messenger);
    return messenger;
}

void Instance::destroyDebugUtilsMessengerEXT(
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks* allocator) const
{
    vkDestroyDebugUtilsMessengerEXT(_instance, messenger, allocator);
}

std::vector<VkPhysicalDevice> Instance::enumeratePhysicalDevices() const
{
    uint32_t physicalDeviceCount = 0;
    check << vkEnumeratePhysicalDevices(
        _instance, &physicalDeviceCount, nullptr);
    auto physicalDevices = std::vector<VkPhysicalDevice>(physicalDeviceCount);
    check << vkEnumeratePhysicalDevices(
        _instance, &physicalDeviceCount, physicalDevices.data());
    return physicalDevices;
}

VkPhysicalDeviceProperties Instance::getPhysicalDeviceProperties(
    VkPhysicalDevice physicalDevice) const
{
    auto properties = VkPhysicalDeviceProperties{};
    vkGetPhysicalDeviceProperties(physicalDevice, &properties);
    return properties;
}

VkPhysicalDeviceFeatures Instance::getPhysicalDeviceFeatures(
    VkPhysicalDevice physicalDevice) const
{
    auto features = VkPhysicalDeviceFeatures{};
    vkGetPhysicalDeviceFeatures(physicalDevice, &features);
    return features;
}

std::vector<VkExtensionProperties> Instance::enumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char* layerName) const
{
    uint32_t extensionCount = 0;
    check << vkEnumerateDeviceExtensionProperties(
        physicalDevice, layerName, &extensionCount, nullptr);
    auto extensionProperties =
        std::vector<VkExtensionProperties>(extensionCount);
    check << vkEnumerateDeviceExtensionProperties(
        physicalDevice, layerName, &extensionCount, extensionProperties.data());
    return extensionProperties;
}

std::vector<VkQueueFamilyProperties> Instance::getPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice physicalDevice) const
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queueFamilyCount, nullptr);
    auto queueFamilyProperties =
        std::vector<VkQueueFamilyProperties>(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        physicalDevice, &queueFamilyCount, queueFamilyProperties.data());
    return queueFamilyProperties;
}

VkBool32 Instance::getPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    VkSurfaceKHR surface) const
{
    auto result = VkBool32{};
    check << vkGetPhysicalDeviceSurfaceSupportKHR(
        physicalDevice, queueFamilyIndex, surface, &result);
    return result;
}

VkSurfaceCapabilitiesKHR Instance::getPhysicalDeviceSurfaceCapabilities(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const
{
    auto capabilities = VkSurfaceCapabilitiesKHR{};
    check << vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        physicalDevice, surface, &capabilities);
    return capabilities;
}

Device Instance::createDevice(
    VkPhysicalDevice physicalDevice,
    const VkDeviceCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    auto vkDevice = VkDevice{};
    check << vkCreateDevice(physicalDevice, createInfo, allocator, &vkDevice);
    return Device{vkDevice, vkGetDeviceProcAddr};
}

VkSurfaceKHR Instance::createXcbSurfaceKHR(
    const VkXcbSurfaceCreateInfoKHR* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    auto surface = VkSurfaceKHR{};
    check << vkCreateXcbSurfaceKHR(_instance, createInfo, allocator, &surface);
    return surface;
}

void Instance::destroySurfaceKHR(
    VkSurfaceKHR surface, const VkAllocationCallbacks* allocator) const
{
    vkDestroySurfaceKHR(_instance, surface, allocator);
}

} // namespace rr::vk
