#include <vk/instance.hpp>

#include <vk/check.hpp>
#include <vk/global.hpp>
#include <vk/util.hpp>

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
    return getVkObject<VkDebugUtilsMessengerEXT>(
        vkCreateDebugUtilsMessengerEXT, _instance, createInfo, allocator);
}

void Instance::destroyDebugUtilsMessengerEXT(
    VkDebugUtilsMessengerEXT messenger,
    const VkAllocationCallbacks* allocator) const
{
    vkDestroyDebugUtilsMessengerEXT(_instance, messenger, allocator);
}

std::vector<VkPhysicalDevice> Instance::enumeratePhysicalDevices() const
{
    return getVkObjects<VkPhysicalDevice>(
        vkEnumeratePhysicalDevices, _instance);
}

VkPhysicalDeviceProperties Instance::getPhysicalDeviceProperties(
    VkPhysicalDevice physicalDevice) const
{
    return getVkObject<VkPhysicalDeviceProperties>(
        vkGetPhysicalDeviceProperties, physicalDevice);
}

VkPhysicalDeviceFeatures Instance::getPhysicalDeviceFeatures(
    VkPhysicalDevice physicalDevice) const
{
    return getVkObject<VkPhysicalDeviceFeatures>(
        vkGetPhysicalDeviceFeatures, physicalDevice);
}

std::vector<VkExtensionProperties> Instance::enumerateDeviceExtensionProperties(
    VkPhysicalDevice physicalDevice, const char* layerName) const
{
    return getVkObjects<VkExtensionProperties>(
        vkEnumerateDeviceExtensionProperties, physicalDevice, layerName);
}

std::vector<VkQueueFamilyProperties> Instance::getPhysicalDeviceQueueFamilyProperties(
    VkPhysicalDevice physicalDevice) const
{
    return getVkObjects<VkQueueFamilyProperties>(
        vkGetPhysicalDeviceQueueFamilyProperties, physicalDevice);
}

VkBool32 Instance::getPhysicalDeviceSurfaceSupportKHR(
    VkPhysicalDevice physicalDevice,
    uint32_t queueFamilyIndex,
    VkSurfaceKHR surface) const
{
    return getVkObject<VkBool32>(
        vkGetPhysicalDeviceSurfaceSupportKHR,
        physicalDevice,
        queueFamilyIndex,
        surface);
}

VkSurfaceCapabilitiesKHR Instance::getPhysicalDeviceSurfaceCapabilitiesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const
{
    return getVkObject<VkSurfaceCapabilitiesKHR>(
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR, physicalDevice, surface);
}

std::vector<VkSurfaceFormatKHR> Instance::getPhysicalDeviceSurfaceFormatsKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const
{
    return getVkObjects<VkSurfaceFormatKHR>(
        vkGetPhysicalDeviceSurfaceFormatsKHR, physicalDevice, surface);
}

std::vector<VkPresentModeKHR> Instance::getPhysicalDeviceSurfacePresentModesKHR(
    VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const
{
    return getVkObjects<VkPresentModeKHR>(
        vkGetPhysicalDeviceSurfacePresentModesKHR, physicalDevice, surface);
}

Device Instance::createDevice(
    VkPhysicalDevice physicalDevice,
    const VkDeviceCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    auto vkDevice = getVkObject<VkDevice>(
        vkCreateDevice, physicalDevice, createInfo, allocator);
    return Device{vkDevice, vkGetDeviceProcAddr};
}

VkSurfaceKHR Instance::createXcbSurfaceKHR(
    const VkXcbSurfaceCreateInfoKHR* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkSurfaceKHR>(
        vkCreateXcbSurfaceKHR, _instance, createInfo, allocator);
}

void Instance::destroySurfaceKHR(
    VkSurfaceKHR surface, const VkAllocationCallbacks* allocator) const
{
    vkDestroySurfaceKHR(_instance, surface, allocator);
}

} // namespace rr::vk
