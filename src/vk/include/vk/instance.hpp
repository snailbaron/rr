#pragma once

#include <vk/device.hpp>

#include <vulkan/vulkan.h>

#include <vector>

namespace rr::vk {

class Instance {
public:
    Instance(
        const VkInstanceCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator);
    ~Instance();

    Instance(const Instance&) = delete;
    Instance(Instance&&) = delete;
    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&) = delete;

    VkDebugUtilsMessengerEXT createDebugUtilsMessengerEXT(
        const VkDebugUtilsMessengerCreateInfoEXT* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroyDebugUtilsMessengerEXT(
        VkDebugUtilsMessengerEXT messenger,
        const VkAllocationCallbacks* allocator) const;

    std::vector<VkPhysicalDevice> enumeratePhysicalDevices() const;
    VkPhysicalDeviceProperties getPhysicalDeviceProperties(
        VkPhysicalDevice physicalDevice) const;
    VkPhysicalDeviceFeatures getPhysicalDeviceFeatures(
        VkPhysicalDevice physicalDevice) const;
    std::vector<VkExtensionProperties> enumerateDeviceExtensionProperties(
        VkPhysicalDevice physicalDevice, const char* layerName) const;
    std::vector<VkQueueFamilyProperties> getPhysicalDeviceQueueFamilyProperties(
        VkPhysicalDevice physicalDevice) const;
    VkBool32 getPhysicalDeviceSurfaceSupportKHR(
        VkPhysicalDevice physicalDevice,
        uint32_t queueFamilyIndex,
        VkSurfaceKHR surface) const;
    VkSurfaceCapabilitiesKHR getPhysicalDeviceSurfaceCapabilities(
        VkPhysicalDevice physicalDevice, VkSurfaceKHR surface) const;

    Device createDevice(
        VkPhysicalDevice physicalDevice,
        const VkDeviceCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator) const;

    VkSurfaceKHR createXcbSurfaceKHR(
        const VkXcbSurfaceCreateInfoKHR* createInfo,
        const VkAllocationCallbacks* allocator) const;

    void destroySurfaceKHR(
        VkSurfaceKHR surface, const VkAllocationCallbacks* allocator) const;

private:
    VkInstance _instance {};

#define INSTANCE(NAME) PFN_##NAME NAME;
#include <vk/vulkan-functions.hpp>
};

} // namespace rr::vk
