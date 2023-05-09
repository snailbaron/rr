#include <rr.hpp>

#include <dlfcn.h>
#include <link.h>
#include <xcb/xcb.h>

#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xcb.h>
#undef VK_NO_PROTOTYPES

#include <array>
#include <cstdlib>
#include <iostream>
#include <optional>
#include <ostream>
#include <set>
#include <string>
#include <vector>

using namespace rr;

using namespace std::string_literals;

extern "C" {

PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr = nullptr;

#define VULKAN_FUNCTION(NAME) \
    PFN_ ## NAME NAME = nullptr;
#define VULKAN_INSTANCE_COMMAND(NAME) \
    PFN_ ## NAME NAME = nullptr;
#define VULKAN_DEVICE_COMMAND(NAME) \
    PFN_ ## NAME NAME = nullptr;
#define VULKAN_XCB_FUNCTION(NAME) \
    PFN_ ## NAME NAME = nullptr;
#include "vulkan-functions.hpp"

}

class PrintQueueFlags {
public:
    PrintQueueFlags(VkQueueFlags flags)
        : _flags(flags)
    { }

    friend std::ostream& operator<<(
        std::ostream& output, const PrintQueueFlags& p)
    {
        static const auto mapping = std::vector<std::pair<VkQueueFlagBits, std::string>>{
            {VK_QUEUE_GRAPHICS_BIT, "graphics"},
            {VK_QUEUE_COMPUTE_BIT, "compute"},
            {VK_QUEUE_TRANSFER_BIT, "transfer"},
            {VK_QUEUE_SPARSE_BINDING_BIT, "sparse binding"},
            {VK_QUEUE_PROTECTED_BIT, "protected"},
            {VK_QUEUE_VIDEO_DECODE_BIT_KHR, "video decode"},
            {VK_QUEUE_OPTICAL_FLOW_BIT_NV, "optical flow"},
        };

        for (auto it = mapping.begin(); it != mapping.end();) {
            if (p._flags & it->first) {
                output << it++->second;
                for (; it != mapping.end(); ++it) {
                    if (p._flags & it->first) {
                        output << "|" << it->second;
                    }
                }
            } else {
                ++it;
            }
        }

        return output;
    }

private:
    VkQueueFlags _flags = 0;
};

class Allocator {
private:
    static void* allocate(
        [[maybe_unused]] void* pUserData,
        size_t size,
        size_t alignment,
        [[maybe_unused]] VkSystemAllocationScope allocationScope)
    {
        return std::aligned_alloc(alignment, size);
    }

    static void* reallocate(
        [[maybe_unused]] void* pUserData,
        void* pOriginal,
        size_t size,
        [[maybe_unused]] size_t alignment,
        [[maybe_unused]] VkSystemAllocationScope allocationScope)
    {
        return std::realloc(pOriginal, size);
    }

    static void free([[maybe_unused]] void* pUserData, void* pMemory)
    {
        std::free(pMemory);
    }

    static void internalAllocationNotification(
        void* pUserData,
        size_t size,
        VkInternalAllocationType allocationType,
        VkSystemAllocationScope allocationScope)
    { }

    static void internalFreeNotification(
        void* pUserData,
        size_t size,
        VkInternalAllocationType allocationType,
        VkSystemAllocationScope allocationScope)
    { }

public:
    static constexpr VkAllocationCallbacks callbacks {
        .pUserData = nullptr,
        .pfnAllocation = Allocator::allocate,
        .pfnReallocation = Allocator::reallocate,
        .pfnFree = Allocator::free,
        .pfnInternalAllocation = Allocator::internalAllocationNotification,
        .pfnInternalFree = Allocator::internalFreeNotification,
    };
};

int main()
{
    void* vulkanLoaderLibrary = dlopen("libvulkan.so", RTLD_LAZY);
    if (!vulkanLoaderLibrary) {
        throw Error{"could not load vulkan loader library: "s + dlerror()};
    }
    dlerror();

    struct link_map* linkMap;
    if (dlinfo(vulkanLoaderLibrary, RTLD_DI_LINKMAP, &linkMap) == -1) {
        throw Error{"could not get vulkan loader library linkmap: "s + dlerror()};
    }
    std::cout << "using vulkan loader at: " << linkMap->l_name << "\n";

    vkGetInstanceProcAddr = reinterpret_cast<PFN_vkGetInstanceProcAddr>(
        dlsym(vulkanLoaderLibrary, "vkGetInstanceProcAddr"));
    if (char* error = dlerror(); error != nullptr) {
        throw Error{"could not load vkGetInstanceProcAddr: "s + error};
    }

#define VULKAN_FUNCTION(NAME) \
    NAME = reinterpret_cast<PFN_ ## NAME>(vkGetInstanceProcAddr(nullptr, #NAME));
#include "vulkan-functions.hpp"

    uint32_t vulkanApiVersion = 0;
    vkEnumerateInstanceVersion(&vulkanApiVersion);
    std::cout << "Vulkan API version: " << vulkanApiVersion << "\n";

    std::vector<VkLayerProperties> layerProperties;
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        layerProperties.resize(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());
    }

    std::cout << "layers:\n";
    for (const auto& lp : layerProperties) {
        std::cout << lp.layerName << " : " <<
            lp.specVersion << " : " << lp.implementationVersion << " : " <<
            lp.description << "\n";
    }

    auto availableLayers = std::set<std::string>{};
    for (const auto& lp : layerProperties) {
        availableLayers.emplace(lp.layerName);
    }

    std::vector<VkExtensionProperties> extensionProperties;
    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties(
        nullptr, // layer name
        &extensionCount,
        nullptr);
    extensionProperties.resize(extensionCount);
    vkEnumerateInstanceExtensionProperties(
        nullptr, // layer name
        &extensionCount,
        extensionProperties.data());

    std::cout << "extensions:\n";
    for (const auto& ep : extensionProperties) {
        std::cout << ep.extensionName << " : " << ep.specVersion << "\n";
    }

    auto availableExtensionNames = std::set<std::string>{};
    for (const auto& ep : extensionProperties) {
        availableExtensionNames.emplace(ep.extensionName);
    }

    auto vkInstance = VkInstance{};
    {
        auto layerNames = std::vector<const char*>{};
        static const char* validationLayerName = "VK_LAYER_KHRONOS_validation";
        if (availableLayers.contains(validationLayerName)) {
            layerNames.push_back(validationLayerName);
        }

        auto extensionNames = std::vector<const char*>{};
        static const char* surfaceExtension = "VK_KHR_surface";
        static const char* xcbExtension = "VK_KHR_xcb_surface";
        if (availableExtensionNames.contains(surfaceExtension) &&
                availableExtensionNames.contains(xcbExtension)) {
            extensionNames.push_back(surfaceExtension);
            extensionNames.push_back(xcbExtension);
        }
        auto instanceCreateInfo = VkInstanceCreateInfo {
            .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .pApplicationInfo = nullptr,
            .enabledLayerCount = static_cast<uint32_t>(layerNames.size()),
            .ppEnabledLayerNames = layerNames.data(),
            .enabledExtensionCount = static_cast<uint32_t>(extensionNames.size()),
            .ppEnabledExtensionNames = extensionNames.data(),
        };
        if (vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance) != VK_SUCCESS) {
            throw Error{"vkCreate instance failed"};
        }
    }

#define VULKAN_INSTANCE_COMMAND(NAME) \
    NAME = reinterpret_cast<PFN_ ## NAME>(vkGetInstanceProcAddr(vkInstance, #NAME));
#define VULKAN_XCB_FUNCTION(NAME) \
    NAME = reinterpret_cast<PFN_ ## NAME>(vkGetInstanceProcAddr(vkInstance, #NAME));
#include "vulkan-functions.hpp"

    std::vector<VkPhysicalDevice> physicalDevices;
    {
        uint32_t physicalDeviceCount = 0;
        vkEnumeratePhysicalDevices(vkInstance, &physicalDeviceCount, nullptr);
        physicalDevices.resize(physicalDeviceCount);
        vkEnumeratePhysicalDevices(
            vkInstance, &physicalDeviceCount, physicalDevices.data());
    }

    std::cout << "devices:\n";
    auto selectedPhysicalDevice = VkPhysicalDevice{};
    for (const auto& pd : physicalDevices) {
        auto p = VkPhysicalDeviceProperties{};
        vkGetPhysicalDeviceProperties(pd, &p);

        // just select the first discrete GPU
        if (p.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            selectedPhysicalDevice = pd;
            std::cout << "  * ";
        } else {
            std::cout << "    ";
        }

        std::cout << p.apiVersion << " : " << p.driverVersion << " : " <<
            p.vendorID << " : " << p.deviceID << " : " <<
            p.deviceType << " : " << p.deviceName << " : ";
        for (uint8_t x : p.pipelineCacheUUID) {
            std::cout << std::hex << (int)x;
        }
        std::cout << std::dec << " : " <<
            "(skipping limits and sparse properties)\n";
    }

    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    {
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(
            selectedPhysicalDevice, &queueFamilyCount, nullptr);
        queueFamilyProperties.resize(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(
            selectedPhysicalDevice, &queueFamilyCount, queueFamilyProperties.data());
    }

    std::cout << "queue families:\n";
    std::optional<uint32_t> selectedQueueFamilyIndex = 0;
    for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
        const auto& p = queueFamilyProperties.at(i);
        if (p.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            selectedQueueFamilyIndex = i;
            std::cout << "  * ";
        } else {
            std::cout << "    ";
        }
        std::cout <<
            PrintQueueFlags{p.queueFlags} << " : " <<
            p.queueCount << " : " <<
            p.timestampValidBits << " : " <<
            p.minImageTransferGranularity.width << "x" <<
            p.minImageTransferGranularity.height << "x" <<
            p.minImageTransferGranularity.depth << "\n";
    }

    if (!selectedQueueFamilyIndex) {
        throw Error{"could not select queue family"};
    }

    auto queuePriorities = std::array{1.f};
    auto queueCreateInfos = std::array{
        VkDeviceQueueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = *selectedQueueFamilyIndex,
            .queueCount = queuePriorities.size(),
            .pQueuePriorities = queuePriorities.data(),
        }
    };

    auto deviceCreateInfo = VkDeviceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = queueCreateInfos.size(),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0, // deprecated
        .ppEnabledLayerNames = nullptr, // deprecated
        .enabledExtensionCount = 0,
        .ppEnabledExtensionNames = nullptr,
        .pEnabledFeatures = nullptr,
    };
    auto device = VkDevice{};
    vkCreateDevice(
        selectedPhysicalDevice,
        &deviceCreateInfo,
        nullptr, // allocator
        &device);

#define VULKAN_DEVICE_COMMAND(NAME) \
    NAME = reinterpret_cast<PFN_ ## NAME>(vkGetDeviceProcAddr(device, #NAME));
#include "vulkan-functions.hpp"

    auto commandPoolCreateInfo = VkCommandPoolCreateInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueFamilyIndex = *selectedQueueFamilyIndex,
    };
    auto commandPool = VkCommandPool{};
    vkCreateCommandPool(
        device,
        &commandPoolCreateInfo,
        nullptr, // allocator
        &commandPool);




    xcb_connection_t* xcbConnection = xcb_connect(nullptr, nullptr);

    const xcb_setup_t* setup = xcb_get_setup(xcbConnection);
    xcb_screen_iterator_t screenIterator = xcb_setup_roots_iterator(setup);
    // NOTE: just taking the first screen
    xcb_screen_t* screen = screenIterator.data;

    xcb_window_t window = xcb_generate_id(xcbConnection);
    xcb_create_window(
        xcbConnection,
        XCB_COPY_FROM_PARENT, // depth
        window,
        screen->root,
        0, // x
        0, // y
        200, // width
        200, // height
        10, // border width
        XCB_WINDOW_CLASS_INPUT_OUTPUT,
        screen->root_visual,
        0, // value mask
        nullptr); // value list

    xcb_map_window(xcbConnection, window);

    xcb_flush(xcbConnection);

    auto surface = VkSurfaceKHR{};
    auto xcbSurfaceCreateInfo = VkXcbSurfaceCreateInfoKHR{
        .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .connection = xcbConnection,
        .window = window,
    };
    vkCreateXcbSurfaceKHR(
        vkInstance,
        &xcbSurfaceCreateInfo,
        nullptr, // allocator
        &surface);


    std::cin.get();


    xcb_disconnect(xcbConnection);

    dlclose(vulkanLoaderLibrary);
}
