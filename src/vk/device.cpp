#include <vk/device.hpp>

#include <vk/check.hpp>
#include <vk/util.hpp>

#include <error.hpp>

namespace rr::vk {

Device::Device(VkDevice device, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr)
    : _device(device)
    , vkGetDeviceProcAddr(vkGetDeviceProcAddr)
{
#define DEVICE(NAME) \
    NAME = reinterpret_cast<PFN_##NAME>(vkGetDeviceProcAddr( \
        device, #NAME)); \
    if (!(NAME)) { \
        throw Error{} << "could not load vulkan function: " #NAME; \
    }
#include <vk/vulkan-functions.hpp>
}

Device::~Device()
{
    vkDestroyDevice(_device, nullptr);
}

VkQueue Device::getDeviceQueue(
    uint32_t queueFamilyIndex, uint32_t queueIndex) const
{
    return getVkObject<VkQueue>(
        vkGetDeviceQueue, _device, queueFamilyIndex, queueIndex);
}

VkSwapchainKHR Device::createSwapchainKHR(
    const VkSwapchainCreateInfoKHR* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkSwapchainKHR>(
        vkCreateSwapchainKHR, _device, createInfo, allocator);
}

void Device::destroySwapchainKHR(
    VkSwapchainKHR swapchain, const VkAllocationCallbacks* allocator) const
{
    vkDestroySwapchainKHR(_device, swapchain, allocator);
}

std::vector<VkImage> Device::getSwapchainImagesKHR(
    VkSwapchainKHR swapchain) const
{
    return getVkObjects<VkImage>(
        vkGetSwapchainImagesKHR, _device, swapchain);
}

VkImageView Device::createImageView(
    const VkImageViewCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkImageView>(
        vkCreateImageView, _device, createInfo, allocator);
}

void Device::destroyImageView(
    VkImageView imageView, const VkAllocationCallbacks* allocator) const
{
    vkDestroyImageView(_device, imageView, allocator);
}

VkShaderModule Device::createShaderModule(
    const VkShaderModuleCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkShaderModule>(
        vkCreateShaderModule, _device, createInfo, allocator);
}

void Device::destroyShaderModule(
    VkShaderModule shaderModule,
    const VkAllocationCallbacks* allocator) const
{
    vkDestroyShaderModule(_device, shaderModule, allocator);
}

VkPipelineLayout Device::createPipelineLayout(
    const VkPipelineLayoutCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkPipelineLayout>(
        vkCreatePipelineLayout, _device, createInfo, allocator);
}

void Device::destroyPipelineLayout(
    VkPipelineLayout pipelineLayout,
    const VkAllocationCallbacks* allocator) const
{
    vkDestroyPipelineLayout(_device, pipelineLayout, allocator);
}

VkRenderPass Device::createRenderPass(
    const VkRenderPassCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkRenderPass>(
        vkCreateRenderPass, _device, createInfo, allocator);
}

void Device::destroyRenderPass(
    VkRenderPass renderPass,
    const VkAllocationCallbacks* allocator) const
{
    vkDestroyRenderPass(_device, renderPass, allocator);
}

std::vector<VkPipeline> Device::createGraphicsPipelines(
    VkPipelineCache pipelineCache,
    uint32_t createInfoCount,
    const VkGraphicsPipelineCreateInfo* createInfos,
    const VkAllocationCallbacks* allocator) const
{
    auto pipelines = std::vector<VkPipeline>(createInfoCount);
    check << vkCreateGraphicsPipelines(
        _device,
        pipelineCache,
        createInfoCount,
        createInfos,
        allocator,
        pipelines.data());
    return pipelines;
}

void Device::destroyPipeline(
    VkPipeline pipeline, const VkAllocationCallbacks* allocator) const
{
    vkDestroyPipeline(_device, pipeline, allocator);
}

} // namespace rr::vk
