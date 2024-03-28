#include "build-info.hpp"

#include <error.hpp>
#include <li.hpp>
#include <mm.hpp>
#include <xcb_window.hpp>
//#include <windows_window.hpp>

#include <vulkan/vulkan_raii.hpp>
//#include <vulkan/vulkan_xcb.h>

#include <algorithm>
#include <array>
#include <cstring>
#include <iostream>
#include <optional>

using namespace std::chrono_literals;

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

// TODO: guarantee all flags are checked
void print(std::ostream& output, vk::QueueFlags vkQueueFlags)
{
    bool started = false;
    auto append = [&output, &started, &vkQueueFlags](
        vk::QueueFlagBits bit, const char* name) {
        if (vkQueueFlags & bit) {
            if (started) {
                output << "|";
            } else {
                started = true;
            }
            output << name;
        }
    };

    append(vk::QueueFlagBits::eGraphics, "VK_QUEUE_GRAPHICS_BIT");
    append(vk::QueueFlagBits::eCompute, "VK_QUEUE_COMPUTE_BIT");
    append(vk::QueueFlagBits::eTransfer, "VK_QUEUE_TRANSFER_BIT");
    append(vk::QueueFlagBits::eSparseBinding, "VK_QUEUE_SPARSE_BINDING_BIT");
    append(vk::QueueFlagBits::eProtected, "VK_QUEUE_PROTECTED_BIT");
    append(vk::QueueFlagBits::eVideoDecodeKHR, "VK_QUEUE_VIDEO_DECODE_BIT_KHR");
    //append(vk::QueueFlagBits::VK_QUEUE_VIDEO_ENCODE_BIT_KHR, "VK_QUEUE_VIDEO_ENCODE_BIT_KHR");
    append(vk::QueueFlagBits::eOpticalFlowNV, "VK_QUEUE_OPTICAL_FLOW_BIT_NV");
    //append(vk::QueueFlagBits::, "VK_QUEUE_FLAG_BITS_MAX_ENUM");
}

template <class T>
class Print {
public:
    Print(const T& value)
        : _value(value)
    { }

    friend std::ostream& operator<<(std::ostream& output, const Print& p)
    {
        print(output, p._value);
        return output;
    }

private:
    const T& _value;
};

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

int main()
{
    auto vulkanContext = vk::raii::Context{};

    auto vulkanLibrary = rr::DynamicLibrary{"libvulkan.so"};
    //auto vulkanLibrary = rr::DynamicLibrary{"vulkan-1.dll"};

    auto dynamicGetProcAddr =
        vulkanLibrary.getProcAddress<PFN_vkGetInstanceProcAddr>(
            "vkGetInstanceProcAddr");
    VULKAN_HPP_DEFAULT_DISPATCHER.init(dynamicGetProcAddr);

    const auto windowOptions = rr::WindowOptions{
        .x = 100,
        .y = 100,
        .w = 1000,
        .h = 500,
        .borderWidth = 10,
    };
#if defined(__linux__)
    auto window = rr::Window::create(rr::Api::XCB, windowOptions);
#elif defined(_WIN32)
    auto window = rr::Window::create(rr::Api::Win32, windowOptions);
#endif

    auto layerProperties = vk::enumerateInstanceLayerProperties();
    std::cout << "layers:\n";
    for (const auto& lp : layerProperties) {
        std::cout << "  * " << lp.layerName << " " <<
            lp.specVersion << " (" << lp.implementationVersion << ") " <<
            lp.description << "\n";
    }

    auto instanceExtensionProperties = vk::enumerateInstanceExtensionProperties();
    std::cout << "extensions:\n";
    for (const auto& ep : instanceExtensionProperties) {
        std::cout << "  * " << ep.extensionName << " " << ep.specVersion << "\n";
    }

    auto enabledLayerNames = std::vector<const char*> {
        "VK_LAYER_KHRONOS_validation",
    };
    auto enabledExtensionNames = std::vector<const char*> {
        VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        "VK_KHR_surface",
        "VK_KHR_xcb_surface",
        //"VK_KHR_win32_surface",
    };
    auto applicationInfo = vk::ApplicationInfo{
        .pNext = nullptr,
        .pApplicationName = "Wee-wee example",
        .applicationVersion = VK_MAKE_VERSION(0, 1, 0),
        .pEngineName = "weewee",
        .engineVersion = VK_MAKE_VERSION(0, 1, 0),
        .apiVersion = VK_API_VERSION_1_0,
    };
    auto instanceCreateInfo = vk::InstanceCreateInfo{
        .pNext = nullptr,
        .flags = vk::InstanceCreateFlags{},
        .pApplicationInfo = &applicationInfo,
        .enabledLayerCount = (uint32_t)enabledLayerNames.size(),
        .ppEnabledLayerNames = enabledLayerNames.data(),
        .enabledExtensionCount = (uint32_t)enabledExtensionNames.size(),
        .ppEnabledExtensionNames = enabledExtensionNames.data(),
    };
    auto instance = vk::raii::Instance{vulkanContext, instanceCreateInfo};
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*instance);

    auto messengerCreateInfo = vk::DebugUtilsMessengerCreateInfoEXT{
        .pNext = nullptr,
        .flags = vk::DebugUtilsMessengerCreateFlagsEXT{},
        .messageSeverity =
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose,
        .messageType =
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding,
        .pfnUserCallback = debugCallback,
        .pUserData = nullptr,
    };
    vk::raii::DebugUtilsMessengerEXT debugMessenger =
        instance.createDebugUtilsMessengerEXT(messengerCreateInfo);

    auto surface = window->createVulkanSurface(instance);
    //auto win32SurfaceCreateInfo = vk::Win32SurfaceCreateInfoKHR{
    //    .pNext = nullptr,
    //    .flags = vk::Win32SurfaceCreateFlagsKHR{},
    //    .hinstance = window.hinstance(),
    //    .hwnd = window.hwnd(),
    //};
    //auto surface = instance.createWin32SurfaceKHR(win32SurfaceCreateInfo);

    auto deviceExtensionNames = std::vector<const char*>{
        VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    };

    auto physicalDevices = instance.enumeratePhysicalDevices();
    auto selectedPhysicalDevice = vk::raii::PhysicalDevice{nullptr};
    auto selectedQueueFamilies = std::vector<uint32_t>{};
    auto availableSurfaceFormats = std::vector<vk::SurfaceFormatKHR>{};
    auto availablePresentModes = std::vector<vk::PresentModeKHR>{};
    auto availableSurfaceCapabilities = vk::SurfaceCapabilitiesKHR{};
    uint32_t selectedGraphicsQueueFamily = 0;
    uint32_t selectedPresentQueueFamily = 0;
    std::cout << "physical devices:\n";
    for (const auto& physicalDevice : physicalDevices) {
        auto dp = physicalDevice.getProperties();
        auto df = physicalDevice.getFeatures();

        std::cout << "  * " << dp.deviceName << "\n";

        auto deviceExtensionProperties =
            physicalDevice.enumerateDeviceExtensionProperties();
        //std::cout << "    extensions:\n";
        //for (const auto& ep : extensionProperties) {
        //    std::cout << "      * " << ep.extensionName <<
        //        " (" << ep.specVersion << ")\n";
        //}

        if (dp.deviceType != vk::PhysicalDeviceType::eDiscreteGpu) {
            std::cout << "device is not a discrete GPU\n";
            continue;
        }

        if (!df.geometryShader) {
            std::cout << "device is lacking geometry shader\n";
            continue;
        }

        if (std::ranges::any_of(
                deviceExtensionNames,
                [&deviceExtensionProperties] (const char* desiredExtension) {
                    return std::ranges::none_of(
                        deviceExtensionProperties,
                        [desiredExtension] (const vk::ExtensionProperties& p) {
                            return std::strcmp(
                                p.extensionName, desiredExtension) == 0;
                        });
                })) {
            std::cout << "device does not support all desired extensions\n";
            continue;
        }

        vk::SurfaceCapabilitiesKHR surfaceCapabilities =
            physicalDevice.getSurfaceCapabilitiesKHR(surface);
        auto surfaceFormats = physicalDevice.getSurfaceFormatsKHR(surface);
        auto presentModes = physicalDevice.getSurfacePresentModesKHR(surface);

        if (surfaceFormats.empty()) {
            std::cout << "device does not have any surface formats\n";
            continue;
        }

        if (presentModes.empty()) {
            std::cout << "device does not have any present modes\n";
            continue;
        }

        auto queueFamilyProperties = physicalDevice.getQueueFamilyProperties();

        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;

        std::cout << "    queue families:\n";
        for (uint32_t i = 0; i < queueFamilyProperties.size(); i++) {
            const vk::QueueFamilyProperties& qfp = queueFamilyProperties.at(i);
            std::cout << "      * " <<
                qfp.queueCount << " queues: " << Print{qfp.queueFlags} << "\n";

            if (!graphicsFamily) {
                if (qfp.queueFlags & vk::QueueFlagBits::eGraphics) {
                    graphicsFamily = i;
                }
            }

            if (!presentFamily) {
                if (physicalDevice.getSurfaceSupportKHR(i, surface)) {
                    presentFamily = i;
                }
            }
        }

        if (graphicsFamily && presentFamily) {
            selectedPhysicalDevice = physicalDevice;
            selectedGraphicsQueueFamily = *graphicsFamily;
            selectedPresentQueueFamily = *presentFamily;
            availableSurfaceFormats = std::move(surfaceFormats);
            availablePresentModes = std::move(presentModes);
            availableSurfaceCapabilities = surfaceCapabilities;

            selectedQueueFamilies.push_back(*graphicsFamily);
            if (presentFamily != graphicsFamily) {
                selectedQueueFamilies.push_back(*presentFamily);
            }
        }
    }

    bool graphicsPresentSameFamily =
        (selectedGraphicsQueueFamily == selectedPresentQueueFamily);

    std::cout << "selected queue families:";
    for (uint32_t i : selectedQueueFamilies) {
        std::cout << " " << i;
    }
    std::cout << "\n";

    auto queueCreateInfos = std::vector<vk::DeviceQueueCreateInfo>{};
    float queuePriorities[] {1.f};
    for (uint32_t queueFamilyIndex : selectedQueueFamilies) {
        queueCreateInfos.push_back(vk::DeviceQueueCreateInfo{
            .pNext = nullptr,
            .flags = vk::DeviceQueueCreateFlags{},
            .queueFamilyIndex = queueFamilyIndex,
            .queueCount = 1,
            .pQueuePriorities = queuePriorities,
        });
    }
    auto deviceCreateInfo = vk::DeviceCreateInfo{
        .pNext = nullptr,
        .flags = vk::DeviceCreateFlags{},
        .queueCreateInfoCount = (uint32_t)queueCreateInfos.size(),
        .pQueueCreateInfos = queueCreateInfos.data(),
        .enabledLayerCount = 0,
        .ppEnabledLayerNames = nullptr,
        .enabledExtensionCount = (uint32_t)deviceExtensionNames.size(),
        .ppEnabledExtensionNames = deviceExtensionNames.data(),
        .pEnabledFeatures = nullptr,
    };
    auto device = selectedPhysicalDevice.createDevice(deviceCreateInfo);
    VULKAN_HPP_DEFAULT_DISPATCHER.init(*device);

    vk::SurfaceFormatKHR selectedSurfaceFormat = availableSurfaceFormats.front();
    for (const auto& format : availableSurfaceFormats) {
        if (format.format == vk::Format::eB8G8R8A8Srgb &&
                format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
            selectedSurfaceFormat = format;
            break;
        }
    }

    vk::PresentModeKHR selectedPresentMode = vk::PresentModeKHR::eFifo;
    for (const auto& mode : availablePresentModes) {
        if (mode == vk::PresentModeKHR::eMailbox) {
            selectedPresentMode = mode;
            break;
        }
    }

    auto [windowWidth, windowHeight] = window->size();

    uint32_t swapChainImageCount = availableSurfaceCapabilities.maxImageCount;
    if (swapChainImageCount == 0) {
        swapChainImageCount = availableSurfaceCapabilities.minImageCount + 1;
    }

    vk::Queue graphicsQueue = device.getQueue(selectedGraphicsQueueFamily, 0);
    vk::Queue presentQueue = device.getQueue(selectedPresentQueueFamily, 0);

    auto swapchainExtent = vk::Extent2D{
        .width = (uint32_t)windowWidth,
        .height = (uint32_t)windowHeight,
    };

    auto swapchainCreateInfo = vk::SwapchainCreateInfoKHR{
        .pNext = nullptr,
        .flags = vk::SwapchainCreateFlagsKHR{},
        .surface = surface,
        .minImageCount = swapChainImageCount,
        .imageFormat = selectedSurfaceFormat.format,
        .imageColorSpace = selectedSurfaceFormat.colorSpace,
        .imageExtent = swapchainExtent,
        .imageArrayLayers = 1,
        .imageUsage = vk::ImageUsageFlagBits::eColorAttachment,
        .imageSharingMode = graphicsPresentSameFamily ?
            vk::SharingMode::eExclusive : vk::SharingMode::eConcurrent,
        .queueFamilyIndexCount = graphicsPresentSameFamily ? 0u : 2u,
        .pQueueFamilyIndices = graphicsPresentSameFamily ?
            nullptr : selectedQueueFamilies.data(),
        .preTransform = availableSurfaceCapabilities.currentTransform,
        .compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque,
        .presentMode = selectedPresentMode,
        .clipped = vk::True,
        .oldSwapchain = VK_NULL_HANDLE,
    };
    vk::raii::SwapchainKHR swapchain = device.createSwapchainKHR(swapchainCreateInfo);

    auto swapchainImages = swapchain.getImages();

    auto swapchainImageViews = std::vector<vk::raii::ImageView>{};
    swapchainImageViews.reserve(swapchainImages.size());
    for (const vk::Image& image : swapchainImages) {
        auto imageViewCreateInfo = vk::ImageViewCreateInfo{
            .pNext = nullptr,
            .flags = vk::ImageViewCreateFlags{},
            .image = image,
            .viewType = vk::ImageViewType::e2D,
            .format = selectedSurfaceFormat.format,
            .components = vk::ComponentMapping {
                .r = vk::ComponentSwizzle::eIdentity,
                .g = vk::ComponentSwizzle::eIdentity,
                .b = vk::ComponentSwizzle::eIdentity,
                .a = vk::ComponentSwizzle::eIdentity,
            },
            .subresourceRange = vk::ImageSubresourceRange{
                .aspectMask = vk::ImageAspectFlagBits::eColor,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        swapchainImageViews.push_back(
            device.createImageView(imageViewCreateInfo));
    }

    auto vertShaderFile = rr::MemoryMap{SHADER_DIR / "vert.spv"};
    auto fragShaderFile = rr::MemoryMap{SHADER_DIR / "frag.spv"};

    auto vertShaderInfo = vk::ShaderModuleCreateInfo{
        .pNext = nullptr,
        .flags = vk::ShaderModuleCreateFlags{},
        .codeSize = vertShaderFile.size(),
        .pCode = reinterpret_cast<uint32_t*>(vertShaderFile.addr()),
    };
    auto fragShaderInfo = vk::ShaderModuleCreateInfo{
        .pNext = nullptr,
        .flags = vk::ShaderModuleCreateFlags{},
        .codeSize = fragShaderFile.size(),
        .pCode = reinterpret_cast<uint32_t*>(fragShaderFile.addr()),
    };

    vk::raii::ShaderModule vertShaderModule =
        device.createShaderModule(vertShaderInfo);
    vk::raii::ShaderModule fragShaderModule =
        device.createShaderModule(fragShaderInfo);

    auto vertShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo {
        .pNext = nullptr,
        .flags = vk::PipelineShaderStageCreateFlags{},
        .stage = vk::ShaderStageFlagBits::eVertex,
        .module = vertShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };
    auto fragShaderStageCreateInfo = vk::PipelineShaderStageCreateInfo {
        .pNext = nullptr,
        .flags = vk::PipelineShaderStageCreateFlags{},
        .stage = vk::ShaderStageFlagBits::eFragment,
        .module = fragShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };
    vk::PipelineShaderStageCreateInfo shaderStages[] {
        vertShaderStageCreateInfo,
        fragShaderStageCreateInfo,
    };

    auto dynamicStateValues = std::vector<vk::DynamicState>{
        vk::DynamicState::eViewport,
        vk::DynamicState::eScissor,
    };
    auto dynamicState = vk::PipelineDynamicStateCreateInfo{
        .pNext = nullptr,
        .flags = vk::PipelineDynamicStateCreateFlags{},
        .dynamicStateCount = (uint32_t)dynamicStateValues.size(),
        .pDynamicStates = dynamicStateValues.data(),
    };

    auto vertexInputState = vk::PipelineVertexInputStateCreateInfo{
        .pNext = nullptr,
        .flags = vk::PipelineVertexInputStateCreateFlags{},
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };

    auto inputAssemblyState = vk::PipelineInputAssemblyStateCreateInfo{
        .pNext = nullptr,
        .flags = vk::PipelineInputAssemblyStateCreateFlags{},
        .topology = vk::PrimitiveTopology::eTriangleList,
        .primitiveRestartEnable = vk::False,
    };

    auto viewport = vk::Viewport{
        .x = 0.f,
        .y = 0.f,
        .width = (float)windowWidth,
        .height = (float)windowHeight,
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };

    auto scissor = vk::Rect2D {
        .offset = vk::Offset2D{.x = 0, .y = 0},
        .extent = swapchainExtent,
    };

    auto viewportState = vk::PipelineViewportStateCreateInfo{
        .pNext = nullptr,
        .flags = vk::PipelineViewportStateCreateFlags{},
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    auto rasterizationState = vk::PipelineRasterizationStateCreateInfo{
        .pNext = nullptr,
        .flags = vk::PipelineRasterizationStateCreateFlags{},
        .depthClampEnable = vk::False,
        .rasterizerDiscardEnable = vk::False,
        .polygonMode = vk::PolygonMode::eFill,
        .cullMode = vk::CullModeFlagBits::eBack,
        .frontFace = vk::FrontFace::eClockwise,
        .depthBiasEnable = vk::False,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp = 0.f,
        .depthBiasSlopeFactor = 0.f,
        .lineWidth = 1.f,
    };

    auto multisampleState = vk::PipelineMultisampleStateCreateInfo{
        .pNext = nullptr,
        .flags = vk::PipelineMultisampleStateCreateFlags{},
        .rasterizationSamples = vk::SampleCountFlagBits::e1,
        .sampleShadingEnable = vk::False,
        .minSampleShading = 1.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = vk::False,
        .alphaToOneEnable = vk::False,
    };

    auto colorBlendAttachmentState = vk::PipelineColorBlendAttachmentState{
        .blendEnable = vk::False,
        .srcColorBlendFactor = vk::BlendFactor::eOne,
        .dstColorBlendFactor = vk::BlendFactor::eZero,
        .colorBlendOp = vk::BlendOp::eAdd,
        .srcAlphaBlendFactor = vk::BlendFactor::eOne,
        .dstAlphaBlendFactor = vk::BlendFactor::eZero,
        .alphaBlendOp = vk::BlendOp::eAdd,
        .colorWriteMask =
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB |
            vk::ColorComponentFlagBits::eA,
    };

    auto colorBlendState = vk::PipelineColorBlendStateCreateInfo{
        .pNext = nullptr,
        .flags = vk::PipelineColorBlendStateCreateFlags{},
        .logicOpEnable = vk::False,
        .logicOp = vk::LogicOp::eCopy,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
        .blendConstants = std::array{0.f, 0.f, 0.f, 0.f},
    };

    auto pipelineLayoutInfo = vk::PipelineLayoutCreateInfo {
        .pNext = nullptr,
        .flags = vk::PipelineLayoutCreateFlags{},
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };
    vk::raii::PipelineLayout pipelineLayout =
        device.createPipelineLayout(pipelineLayoutInfo);

    auto colorAttachmentDescription = vk::AttachmentDescription{
        .flags = vk::AttachmentDescriptionFlags{},
        .format = selectedSurfaceFormat.format,
        .samples = vk::SampleCountFlagBits::e1,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .stencilLoadOp = vk::AttachmentLoadOp::eDontCare,
        .stencilStoreOp = vk::AttachmentStoreOp::eDontCare,
        .initialLayout = vk::ImageLayout::eUndefined,
        .finalLayout = vk::ImageLayout::ePresentSrcKHR,
    };

    auto colorAttachmentReference = vk::AttachmentReference{
        .attachment = 0,
        .layout = vk::ImageLayout::eColorAttachmentOptimal,
    };

    auto subpassDescription = vk::SubpassDescription{
        .flags = vk::SubpassDescriptionFlags{},
        .pipelineBindPoint = vk::PipelineBindPoint::eGraphics,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentReference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    auto subpassDependency = vk::SubpassDependency{
        .srcSubpass = vk::SubpassExternal,
        .dstSubpass = 0,
        .srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput,
        .srcAccessMask = vk::AccessFlags{},
        .dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite,
        .dependencyFlags = vk::DependencyFlags{},
    };
    auto renderPassInfo = vk::RenderPassCreateInfo{
        .pNext = nullptr,
        .flags = vk::RenderPassCreateFlags{},
        .attachmentCount = 1,
        .pAttachments = &colorAttachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 1,
        .pDependencies = &subpassDependency,
    };
    vk::raii::RenderPass renderPass = device.createRenderPass(renderPassInfo);

    auto pipelineInfo = vk::GraphicsPipelineCreateInfo{
        .pNext = nullptr,
        .flags = vk::PipelineCreateFlags{},
        .stageCount = 2,
        .pStages = shaderStages,
        .pVertexInputState = &vertexInputState,
        .pInputAssemblyState = &inputAssemblyState,
        .pTessellationState = nullptr,
        .pViewportState = &viewportState,
        .pRasterizationState = &rasterizationState,
        .pMultisampleState = &multisampleState,
        .pDepthStencilState = nullptr,
        .pColorBlendState = &colorBlendState,
        .pDynamicState = &dynamicState,
        .layout = pipelineLayout,
        .renderPass = renderPass,
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1,
    };

    auto graphicsPipeline =
        device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo);

    auto swapchainFramebuffers = std::vector<vk::raii::Framebuffer>{};
    swapchainFramebuffers.reserve(swapchainImageViews.size());
    for (const auto& swapchainImageView : swapchainImageViews) {
        auto framebufferInfo = vk::FramebufferCreateInfo{
            .pNext = nullptr,
            .flags = vk::FramebufferCreateFlags{},
            .renderPass = renderPass,
            .attachmentCount = 1,
            .pAttachments = &*swapchainImageView,
            .width = swapchainExtent.width,
            .height = swapchainExtent.height,
            .layers = 1,
        };
        swapchainFramebuffers.push_back(
            device.createFramebuffer(framebufferInfo));
    }

    auto commandPoolInfo = vk::CommandPoolCreateInfo{
        .pNext = nullptr,
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = selectedGraphicsQueueFamily,
    };
    vk::raii::CommandPool commandPool = device.createCommandPool(commandPoolInfo);

    auto commandBufferInfo = vk::CommandBufferAllocateInfo{
        .pNext = nullptr,
        .commandPool = commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1,
    };
    auto commandBuffers = device.allocateCommandBuffers(commandBufferInfo);
    vk::CommandBuffer commandBuffer = commandBuffers.front();

    auto semaphoreInfo = vk::SemaphoreCreateInfo{
        .pNext = nullptr,
        .flags = vk::SemaphoreCreateFlags{},
    };
    vk::raii::Semaphore imageAvailableSemaphore =
        device.createSemaphore(semaphoreInfo);
    vk::raii::Semaphore renderFinishedSemaphore =
        device.createSemaphore(semaphoreInfo);

    auto fenceInfo = vk::FenceCreateInfo{
        .pNext = nullptr,
        .flags = vk::FenceCreateFlagBits::eSignaled,
    };
    vk::raii::Fence inFlightFence = device.createFence(fenceInfo);

    for (;;) {
        bool done = false;
        while (auto e = window->poll()) {
            if (e->closeWindow()) {
                done = true;
                break;
            }
        }
        if (done) {
            break;
        }

        (void)device.waitForFences(*inFlightFence, vk::True, UINT64_MAX);
        (void)device.resetFences(*inFlightFence);

        auto [acquireImageResult, imageIndex] = swapchain.acquireNextImage(
            UINT64_MAX, imageAvailableSemaphore, VK_NULL_HANDLE);

        commandBuffer.reset();

        auto beginInfo = vk::CommandBufferBeginInfo{
            .pNext = nullptr,
            .flags = vk::CommandBufferUsageFlags{},
            .pInheritanceInfo = nullptr,
        };
        commandBuffer.begin(beginInfo);

        vk::Framebuffer framebuffer = swapchainFramebuffers.at(imageIndex);

        auto clearColor = vk::ClearValue{
            .color = vk::ClearColorValue{std::array{0.f, 0.f, 0.f, 1.f}}
        };
        auto renderPassBeginInfo = vk::RenderPassBeginInfo{
            .pNext = nullptr,
            .renderPass = renderPass,
            .framebuffer = framebuffer,
            .renderArea = vk::Rect2D{
                .offset = vk::Offset2D{.x = 0, .y = 0},
                .extent = swapchainExtent,
            },
            .clearValueCount = 1,
            .pClearValues = &clearColor,
        };
        commandBuffer.beginRenderPass(
            renderPassBeginInfo, vk::SubpassContents::eInline);

        commandBuffer.bindPipeline(
            vk::PipelineBindPoint::eGraphics, graphicsPipeline);

        auto vp = vk::Viewport{
            .x = 0.f,
            .y = 0.f,
            .width = (float)swapchainExtent.width,
            .height = (float)swapchainExtent.height,
            .minDepth = 0.f,
            .maxDepth = 1.f,
        };
        commandBuffer.setViewport(0, vp);

        auto sc = vk::Rect2D{
            .offset = {0, 0},
            .extent = swapchainExtent,
        };
        commandBuffer.setScissor(0, sc);

        commandBuffer.draw(3, 1, 0, 0);

        commandBuffer.endRenderPass();
        commandBuffer.end();

        vk::PipelineStageFlags waitStages[] {
            vk::PipelineStageFlagBits::eColorAttachmentOutput,
        };
        auto submitInfo = vk::SubmitInfo{
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*imageAvailableSemaphore,
            .pWaitDstStageMask = waitStages,
            .commandBufferCount = 1,
            .pCommandBuffers = &commandBuffer,
            .signalSemaphoreCount = 1,
            .pSignalSemaphores = &*renderFinishedSemaphore,
        };
        graphicsQueue.submit(submitInfo, inFlightFence);

        auto presentInfo = vk::PresentInfoKHR{
            .pNext = nullptr,
            .waitSemaphoreCount = 1,
            .pWaitSemaphores = &*renderFinishedSemaphore,
            .swapchainCount = 1,
            .pSwapchains = &*swapchain,
            .pImageIndices = &imageIndex,
            .pResults = nullptr,
        };
        (void)presentQueue.presentKHR(presentInfo);

        //std::this_thread::sleep_for(1.0s / 30);
    }

    device.waitIdle();
}
