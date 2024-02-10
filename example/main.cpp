#include "build-info.hpp"

#include <error.hpp>
#include <mm.hpp>
#include <vk.hpp>
#include <xcb_window.hpp>

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
    auto availableSurfaceFormats = std::vector<VkSurfaceFormatKHR>{};
    auto availablePresentModes = std::vector<VkPresentModeKHR>{};
    auto availableSurfaceCapabilities = VkSurfaceCapabilitiesKHR{};
    uint32_t selectedGraphicsQueueFamily = 0;
    uint32_t selectedPresentQueueFamily = 0;
    std::cout << "physical devices:\n";
    for (auto physicalDevice : physicalDevices) {
        auto dp = instance.getPhysicalDeviceProperties(physicalDevice);
        auto df = instance.getPhysicalDeviceFeatures(physicalDevice);

        std::cout << "  * " << dp.deviceName << "\n";

        auto extensionProperties = instance.enumerateDeviceExtensionProperties(
            physicalDevice, nullptr);
        //std::cout << "    extensions:\n";
        //for (const auto& ep : extensionProperties) {
        //    std::cout << "      * " << ep.extensionName <<
        //        " (" << ep.specVersion << ")\n";
        //}

        if (dp.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
            std::cout << "device is not a discrete GPU\n";
            continue;
        }

        if (!df.geometryShader) {
            std::cout << "device is lacking geometry shader\n";
            continue;
        }

        if (std::ranges::any_of(
                deviceExtensionNames,
                [&extensionProperties] (const char* desiredExtension) {
                    return std::ranges::none_of(
                        extensionProperties,
                        [desiredExtension] (const VkExtensionProperties& p) {
                            return std::strcmp(
                                p.extensionName, desiredExtension) == 0;
                        });
                })) {
            std::cout << "device does not support all desired extensions\n";
            continue;
        }

        auto surfaceCapabilities = instance.getPhysicalDeviceSurfaceCapabilitiesKHR(
            physicalDevice, surface);
        auto surfaceFormats = instance.getPhysicalDeviceSurfaceFormatsKHR(
            physicalDevice, surface);
        auto presentModes = instance.getPhysicalDeviceSurfacePresentModesKHR(
            physicalDevice, surface);

        if (surfaceFormats.empty()) {
            std::cout << "device does not have any surface formats\n";
            continue;
        }

        if (presentModes.empty()) {
            std::cout << "device does not have any present modes\n";
            continue;
        }

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

    VkSurfaceFormatKHR selectedSurfaceFormat = availableSurfaceFormats.front();
    for (const auto& format : availableSurfaceFormats) {
        if (format.format == VK_FORMAT_B8G8R8A8_SRGB &&
                format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
            selectedSurfaceFormat = format;
            break;
        }
    }

    VkPresentModeKHR selectedPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    for (const auto& mode : availablePresentModes) {
        if (mode == VK_PRESENT_MODE_MAILBOX_KHR) {
            selectedPresentMode = mode;
            break;
        }
    }

    auto [windowWidth, windowHeight] = window.size();

    uint32_t swapChainImageCount = availableSurfaceCapabilities.maxImageCount;
    if (swapChainImageCount == 0) {
        swapChainImageCount = availableSurfaceCapabilities.minImageCount + 1;
    }

    [[maybe_unused]] VkQueue graphicsQueue =
        device.getDeviceQueue(selectedGraphicsQueueFamily, 0);
    [[maybe_unused]] VkQueue presentQueue =
        device.getDeviceQueue(selectedPresentQueueFamily, 0);

    auto swapchainExtent = VkExtent2D{
        .width = (uint32_t)windowWidth,
        .height = (uint32_t)windowHeight,
    };

    auto swapchainCreateInfo = VkSwapchainCreateInfoKHR{
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = surface,
        .minImageCount = swapChainImageCount,
        .imageFormat = selectedSurfaceFormat.format,
        .imageColorSpace = selectedSurfaceFormat.colorSpace,
        .imageExtent = swapchainExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = graphicsPresentSameFamily ?
            VK_SHARING_MODE_EXCLUSIVE : VK_SHARING_MODE_CONCURRENT,
        .queueFamilyIndexCount = graphicsPresentSameFamily ? 0u : 2u,
        .pQueueFamilyIndices = graphicsPresentSameFamily ?
            nullptr : selectedQueueFamilies.data(),
        .preTransform = availableSurfaceCapabilities.currentTransform,
        .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        .presentMode = selectedPresentMode,
        .clipped = VK_TRUE,
        .oldSwapchain = VK_NULL_HANDLE,
    };
    VkSwapchainKHR swapchain =
        device.createSwapchainKHR(&swapchainCreateInfo, nullptr);

    auto swapchainImages = device.getSwapchainImagesKHR(swapchain);

    auto swapchainImageViews = std::vector<VkImageView>{};
    swapchainImageViews.reserve(swapchainImages.size());
    for (const auto& image : swapchainImages) {
        auto imageViewCreateInfo = VkImageViewCreateInfo{
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
            .image = image,
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = selectedSurfaceFormat.format,
            .components = VkComponentMapping {
                .r = VK_COMPONENT_SWIZZLE_IDENTITY,
                .g = VK_COMPONENT_SWIZZLE_IDENTITY,
                .b = VK_COMPONENT_SWIZZLE_IDENTITY,
                .a = VK_COMPONENT_SWIZZLE_IDENTITY,
            },
            .subresourceRange = VkImageSubresourceRange{
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1,
            },
        };
        swapchainImageViews.push_back(
            device.createImageView(&imageViewCreateInfo, nullptr));
    }

    auto vertShaderFile = rr::MemoryMap{SHADER_DIR / "vert.spv"};
    auto fragShaderFile = rr::MemoryMap{SHADER_DIR / "frag.spv"};

    auto vertShaderInfo = VkShaderModuleCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = vertShaderFile.size(),
        .pCode = reinterpret_cast<uint32_t*>(vertShaderFile.addr()),
    };
    auto fragShaderInfo = VkShaderModuleCreateInfo{
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = fragShaderFile.size(),
        .pCode = reinterpret_cast<uint32_t*>(fragShaderFile.addr()),
    };

    VkShaderModule vertShaderModule =
        device.createShaderModule(&vertShaderInfo, nullptr);
    VkShaderModule fragShaderModule =
        device.createShaderModule(&fragShaderInfo, nullptr);

    auto vertShaderStageCreateInfo = VkPipelineShaderStageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_VERTEX_BIT,
        .module = vertShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };
    auto fragShaderStageCreateInfo = VkPipelineShaderStageCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_FRAGMENT_BIT,
        .module = fragShaderModule,
        .pName = "main",
        .pSpecializationInfo = nullptr,
    };
    VkPipelineShaderStageCreateInfo shaderStages[] {
        vertShaderStageCreateInfo,
        fragShaderStageCreateInfo,
    };

    auto dynamicStateValues = std::vector<VkDynamicState>{
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    auto dynamicState = VkPipelineDynamicStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .dynamicStateCount = (uint32_t)dynamicStateValues.size(),
        .pDynamicStates = dynamicStateValues.data(),
    };

    auto vertexInputState = VkPipelineVertexInputStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .vertexBindingDescriptionCount = 0,
        .pVertexBindingDescriptions = nullptr,
        .vertexAttributeDescriptionCount = 0,
        .pVertexAttributeDescriptions = nullptr,
    };

    auto inputAssemblyState = VkPipelineInputAssemblyStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
        .primitiveRestartEnable = VK_FALSE,
    };

    auto viewport = VkViewport{
        .x = 0.f,
        .y = 0.f,
        .width = (float)windowWidth,
        .height = (float)windowHeight,
        .minDepth = 0.f,
        .maxDepth = 1.f,
    };

    auto scissor = VkRect2D {
        .offset = VkOffset2D{.x = 0, .y = 0},
        .extent = swapchainExtent,
    };

    auto viewportState = VkPipelineViewportStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .viewportCount = 1,
        .pViewports = &viewport,
        .scissorCount = 1,
        .pScissors = &scissor,
    };

    auto rasterizationState = VkPipelineRasterizationStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .depthClampEnable = VK_FALSE,
        .rasterizerDiscardEnable = VK_FALSE,
        .polygonMode = VK_POLYGON_MODE_FILL,
        .cullMode = VK_CULL_MODE_BACK_BIT,
        .frontFace = VK_FRONT_FACE_CLOCKWISE,
        .depthBiasEnable = VK_FALSE,
        .depthBiasConstantFactor = 0.f,
        .depthBiasClamp = 0.f,
        .depthBiasSlopeFactor = 0.f,
        .lineWidth = 1.f,
    };

    auto multisampleState = VkPipelineMultisampleStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
        .sampleShadingEnable = VK_FALSE,
        .minSampleShading = 1.f,
        .pSampleMask = nullptr,
        .alphaToCoverageEnable = VK_FALSE,
        .alphaToOneEnable = VK_FALSE,
    };

    auto colorBlendAttachmentState = VkPipelineColorBlendAttachmentState{
        .blendEnable = VK_FALSE,
        .srcColorBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstColorBlendFactor = VK_BLEND_FACTOR_ZERO,
        .colorBlendOp = VK_BLEND_OP_ADD,
        .srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
        .dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
        .alphaBlendOp = VK_BLEND_OP_ADD,
        .colorWriteMask =
            VK_COLOR_COMPONENT_R_BIT |
            VK_COLOR_COMPONENT_G_BIT |
            VK_COLOR_COMPONENT_B_BIT |
            VK_COLOR_COMPONENT_A_BIT,
    };

    auto colorBlendState = VkPipelineColorBlendStateCreateInfo{
        .sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .logicOpEnable = VK_FALSE,
        .logicOp = VK_LOGIC_OP_COPY,
        .attachmentCount = 1,
        .pAttachments = &colorBlendAttachmentState,
        .blendConstants = {0.f, 0.f, 0.f, 0.f},
    };

    auto pipelineLayoutInfo = VkPipelineLayoutCreateInfo {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = 0,
        .pSetLayouts = nullptr,
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };
    VkPipelineLayout pipelineLayout =
        device.createPipelineLayout(&pipelineLayoutInfo, nullptr);

    auto colorAttachmentDescription = VkAttachmentDescription{
        .flags = 0,
        .format = selectedSurfaceFormat.format,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
        .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
        .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    };

    auto colorAttachmentReference = VkAttachmentReference{
        .attachment = 0,
        .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
    };

    auto subpassDescription = VkSubpassDescription{
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachmentReference,
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment = nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr,
    };

    auto renderPassInfo = VkRenderPassCreateInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = 1,
        .pAttachments = &colorAttachmentDescription,
        .subpassCount = 1,
        .pSubpasses = &subpassDescription,
        .dependencyCount = 0,
        .pDependencies = nullptr,
    };
    VkRenderPass renderPass = device.createRenderPass(&renderPassInfo, nullptr);

    auto pipelineInfo = VkGraphicsPipelineCreateInfo{
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
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

    std::vector<VkPipeline> pipelines = device.createGraphicsPipelines(
        VK_NULL_HANDLE, 1, &pipelineInfo, nullptr);
    VkPipeline pipeline = pipelines.front();






    device.destroyPipeline(pipeline, nullptr);

    device.destroyRenderPass(renderPass, nullptr);
    device.destroyPipelineLayout(pipelineLayout, nullptr);

    device.destroyShaderModule(fragShaderModule, nullptr);
    device.destroyShaderModule(vertShaderModule, nullptr);

    for (const auto& imageView : swapchainImageViews) {
        device.destroyImageView(imageView, nullptr);
    }
    device.destroySwapchainKHR(swapchain, nullptr);

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
