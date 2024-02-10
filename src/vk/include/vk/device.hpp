#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace rr::vk {

class Device {
public:
    explicit Device(
        VkDevice device, PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr);
    ~Device();

    Device(const Device&) = delete;
    Device(Device&&) = delete;
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&) = delete;

    VkQueue getDeviceQueue(uint32_t queueFamilyIndex, uint32_t queueIndex) const;

    VkSwapchainKHR createSwapchainKHR(
        const VkSwapchainCreateInfoKHR* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroySwapchainKHR(
        VkSwapchainKHR swapchain, const VkAllocationCallbacks* allocator) const;
    std::vector<VkImage> getSwapchainImagesKHR(VkSwapchainKHR swapchain) const;

    VkImageView createImageView(
        const VkImageViewCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroyImageView(
        VkImageView imageView, const VkAllocationCallbacks* allocator) const;

    VkShaderModule createShaderModule(
        const VkShaderModuleCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroyShaderModule(
        VkShaderModule shaderModule,
        const VkAllocationCallbacks* allocator) const;

    VkPipelineLayout createPipelineLayout(
        const VkPipelineLayoutCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroyPipelineLayout(
        VkPipelineLayout pipelineLayout,
        const VkAllocationCallbacks* allocator) const;

    VkRenderPass createRenderPass(
        const VkRenderPassCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroyRenderPass(
        VkRenderPass renderPass,
        const VkAllocationCallbacks* allocator) const;

    std::vector<VkPipeline> createGraphicsPipelines(
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const VkGraphicsPipelineCreateInfo* createInfos,
        const VkAllocationCallbacks* allocator) const;
    void destroyPipeline(
        VkPipeline pipeline, const VkAllocationCallbacks* allocator) const;

private:
    [[maybe_unused]] VkDevice _device {};

    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;

#define DEVICE(NAME) PFN_##NAME NAME;
#include <vk/vulkan-functions.hpp>
};

} // namespace rr::vk
