#include <error.hpp>
#include <xcb_window.hpp>
#include <vk.hpp>

#include <vulkan/vulkan.h>
#include <vulkan/vulkan_xcb.h>

#include <dlfcn.h>

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <optional>
#include <thread>

using namespace std::chrono_literals;
using rr::Error;

PFN_vkDebugUtilsMessengerCallbackEXT t;

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT severity,
    VkDebugUtilsMessageTypeFlagsEXT types,
    const VkDebugUtilsMessengerCallbackDataEXT* callbackData,
    [[maybe_unused]] void* userData)
{
    if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT) {
        std::cerr << "ERROR: ";
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT) {
        std::cerr << "WARNING: ";
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT) {
        std::cerr << "INFO: ";
    } else if (severity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) {
        std::cerr << "VERBOSE: ";
    } else {
        std::cerr << "UNKNOWN: ";
    }

    if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) {
        std::cerr << "GENERAL: ";
    } else if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT) {
        std::cerr << "VALIDATION: ";
    } else if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT) {
        std::cerr << "PERFORMANCE: ";
    } else if (types & VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT) {
        std::cerr << "BINDING: ";
    } else {
        std::cerr << "UNKNOWN: ";
    }

    std::cerr << callbackData->pMessage << "\n";
    return VK_FALSE;
}

int main()
{
    auto window = rr::XcbWindow{rr::WindowOptions{
        .x = 100,
        .y = 100,
        .w = 1000,
        .h = 500,
        .borderWidth = 10,
    }};

    auto vulkan = rr::vk::Vulkan{};

    auto layerProperties = rr::vk::enumerateInstanceLevelProperties();
    std::cout << "layers:\n";
    for (const auto& lp : layerProperties) {
        std::cout << "  * " << lp.layerName << " " <<
            lp.specVersion << " (" << lp.implementationVersion << ") " <<
            lp.description << "\n";
    }

    auto extensionProperties = rr::vk::enumerateInstanceExtensionProperties();
    std::cout << "extensions:\n";
    for (const auto& ep : extensionProperties) {
        std::cout << "  * " << ep.extensionName << " " << ep.specVersion << "\n";
    }

    auto enabledLayerNames = std::vector<const char*> {
        "VK_LAYER_KHRONOS_validation",
    };
    auto enabledExtensionNames = std::vector<const char*> {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        "VK_KHR_surface",
        "VK_KHR_xcb_surface",
    };
    auto applicationInfo = VkApplicationInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pNext = nullptr,
        .pApplicationName = "Wee-wee example",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "weewee",
        .engineVersion = VK_MAKE_VERSION(0, 1, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };
    auto instanceCreateInfo = VkInstanceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = (uint32_t)enabledLayerNames.size(),
        .ppEnabledLayerNames = enabledLayerNames.data(),
        .enabledExtensionCount = (uint32_t)enabledExtensionNames.size(),
        .ppEnabledExtensionNames = enabledExtensionNames.data(),
    };
    auto instance = rr::vk::Instance{&instanceCreateInfo, nullptr};

    auto messengerCreateInfo = VkDebugUtilsMessengerCreateInfoEXT{
        .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
        .pNext = nullptr,
        .flags = 0,
        .messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT,
        .messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };
    VkDebugUtilsMessengerEXT debugMessenger =
        instance.createDebugUtilsMessengerEXT(&messengerCreateInfo, nullptr);

    auto xcbSurfaceCreateInfo = VkXcbSurfaceCreateInfoKHR{
        .sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .connection = window.connection(),
        .window = window.window(),
    };
    auto surface = instance.createXcbSurfaceKHR(&xcbSurfaceCreateInfo, nullptr);

    auto deviceExtensionNames = std::vector<const char*>{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    auto physicalDevices = instance.enumeratePhysicalDevices();
    auto selectedPhysicalDevice = VkPhysicalDevice{};
    auto selectedQueueFamilies = std::vector<uint32_t>{};
    uint32_t selectedGraphicsQueueFamily = 0;
    uint32_t selectedPresentQueueFamily = 0;
    std::cout << "physical devices:\n";
    for (auto physicalDevice : physicalDevices) {
        auto dp = instance.getPhysicalDeviceProperties(physicalDevice);
        auto df = instance.getPhysicalDeviceFeatures(physicalDevice);

        std::cout << "  * " << dp.deviceName << "\n";

        auto extensionProperties = instance.enumerateDeviceExtensionProperties(
            physicalDevice, nullptr);
        std::cout << "    extensions:\n";
        for (const auto& ep : extensionProperties) {
            std::cout << "      * " << ep.extensionName <<
                " (" << ep.specVersion << ")\n";
        }

        const bool deviceIsSuitable =
            dp.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU &&
            df.geometryShader &&
            std::ranges::find_if(
                extensionProperties,
                [&deviceExtensionNames] (const VkExtensionProperties& ep) {
                    return std::ranges::all_of(
                        deviceExtensionNames,
                        [&ep] (const char* deviceExtensionName) {
                            return std::strcmp(
                                ep.extensionName, deviceExtensionName) == 0;
                        });
                }) != extensionProperties.end();

        auto queueFamilyProperties =
            instance.getPhysicalDeviceQueueFamilyProperties(physicalDevice);

        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        std::cout << "    queue families:\n";
        for (size_t i = 0; i < queueFamilyProperties.size(); i++) {
            const auto& qfp = queueFamilyProperties.at(i);
            std::cout << "      * " <<
                qfp.queueCount << " queues: " <<
                rr::vk::PrintVkQueueFlags{qfp.queueFlags} << "\n";

            if (!graphicsFamily) {
                if (qfp.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                    graphicsFamily = i;
                }
            }

            if (!presentFamily) {
                if (instance.getPhysicalDeviceSurfaceSupportKHR(
                        physicalDevice, i, surface)) {
                    presentFamily = i;
                }
            }
        }

        if (deviceIsSuitable && graphicsFamily && presentFamily) {
            selectedPhysicalDevice = physicalDevice;
            selectedGraphicsQueueFamily = *graphicsFamily;
            selectedPresentQueueFamily = *presentFamily;

            selectedQueueFamilies.push_back(*graphicsFamily);
            if (presentFamily != graphicsFamily) {
                selectedQueueFamilies.push_back(*presentFamily);
            }
        }
    }

    std::cout << "selected queue families:";
    for (uint32_t i : selectedQueueFamilies) {
        std::cout << " " << i;
    }
    std::cout << "\n";

    [[maybe_unused]] auto surfaceCapabilities = instance.getPhysicalDeviceSurfaceCapabilities(
        selectedPhysicalDevice, surface);

    auto queueCreateInfos = std::vector<VkDeviceQueueCreateInfo>{};
    float queuePriorities[] {1.f};
    for (uint32_t queueFamilyIndex : selectedQueueFamilies) {
        queueCreateInfos.push_back(VkDeviceQueueCreateInfo{
            .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .queueFamilyIndex = queueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities,
        });
    }
    auto deviceCreateInfo = VkDeviceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .queueCreateInfoCount = (uint32_t)queueCreateInfos.size(),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = (uint32_t)deviceExtensionNames.size(),
        .ppEnabledExtensionNames = deviceExtensionNames.data(),
        .pEnabledFeatures = nullptr,
    };
    auto device = instance.createDevice(
        selectedPhysicalDevice, &deviceCreateInfo, nullptr);

    [[maybe_unused]] VkQueue graphicsQueue =
        device.getDeviceQueue(selectedGraphicsQueueFamily, 0);
    [[maybe_unused]] VkQueue presentQueue =
        device.getDeviceQueue(selectedPresentQueueFamily, 0);

    //auto graphicsPipelineCreateInfos = std::vector<VkGraphicsPipelineCreateInfo>{
    //    VkGraphicsPipelineCreateInfo{
    //        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
    //        .pNext = nullptr,
    //        .flags = 0,
    //        .stageCount = 0,
    //        .pStages = nullptr,
    //        .pVertexInputState = nullptr,
    //        .pInputAssemblyState = nullptr,
    //        .pTessellationState = nullptr,
    //        .pViewportState = nullptr,
    //        .pRasterizationState = nullptr,
    //        .pMultisampleState = nullptr,
    //        .pDepthStencilState = nullptr,
    //        .pColorBlendState = nullptr,
    //        .pDynamicState = nullptr,
    //        .layout = nullptr,
    //        .renderPass = nullptr,
    //        .subpass = 0,
    //        .basePipelineHandle = nullptr,
    //        .basePipelineIndex = 0,
    //    },
    //};
    //device.createGraphicsPipelines(
    //    VK_NULL_HANDLE,
    //    graphicsPipelineCreateInfos.size(),
    //    graphicsPipelineCreateInfos.data(),
    //    nullptr);






    instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr);
    instance.destroySurfaceKHR(surface, nullptr);



    //for (;;) {
    //    bool done = false;
    //    while (auto e = window->poll()) {
    //        if (e->closeWindow()) {
    //            done = true;
    //            break;
    //        }
    //    }
    //    if (done) {
    //        break;
    //    }

    //    std::this_thread::sleep_for(1.0s / 30);
    //}

}
