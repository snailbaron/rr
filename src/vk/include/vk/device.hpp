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

    VkFramebuffer createFramebuffer(
        const VkFramebufferCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroyFramebuffer(
        VkFramebuffer framebuffer,
        const VkAllocationCallbacks* allocator) const;

    VkCommandPool createCommandPool(
        const VkCommandPoolCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroyCommandPool(
        VkCommandPool commandPool,
        const VkAllocationCallbacks* allocator) const;

    std::vector<VkCommandBuffer> allocateCommandBuffers(
        const VkCommandBufferAllocateInfo* allocateInfo) const;
    void resetCommandBuffer(
        VkCommandBuffer commandBuffer,
        VkCommandBufferResetFlags flags) const;

    void beginCommandBuffer(
        VkCommandBuffer commandBuffer,
        const VkCommandBufferBeginInfo* beginInfo) const;
    void endCommandBuffer(VkCommandBuffer commandBuffer) const;

    void cmdBeginRenderPass(
        VkCommandBuffer commandBuffer,
        const VkRenderPassBeginInfo* renderPassBegin,
        VkSubpassContents contents) const;
    void cmdBindPipeline(
        VkCommandBuffer commandBuffer,
        VkPipelineBindPoint pipelineBindPoint,
        VkPipeline pipeline) const;
    void cmdSetViewport(
        VkCommandBuffer commandBuffer,
        uint32_t firstViewport,
        uint32_t viewportCount,
        const VkViewport* viewports) const;
    void cmdSetScissor(
        VkCommandBuffer commandBuffer,
        uint32_t firstScissor,
        uint32_t scissorCount,
        const VkRect2D* scissors) const;
    void cmdDraw(
        VkCommandBuffer commandBuffer,
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance) const;
    void cmdEndRenderPass(VkCommandBuffer commandBuffer) const;

    VkSemaphore createSemaphore(
        const VkSemaphoreCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroySemaphore(
        VkSemaphore semaphore,
        const VkAllocationCallbacks* allocator) const;

    VkFence createFence(
        const VkFenceCreateInfo* createInfo,
        const VkAllocationCallbacks* allocator) const;
    void destroyFence(
        VkFence fence,
        const VkAllocationCallbacks* allocator) const;

    void waitForFences(
        uint32_t fenceCount,
        const VkFence* fences,
        VkBool32 waitAll,
        uint64_t timeout) const;
    void resetFences(
        uint32_t fenceCount,
        const VkFence* fences) const;

    uint32_t acquireNextImageKHR(
        VkSwapchainKHR swapchain,
        uint64_t timeout,
        VkSemaphore semaphore,
        VkFence fence) const;

    void queueSubmit(
        VkQueue queue,
        uint32_t submitCount,
        const VkSubmitInfo* submits,
        VkFence fence) const;

    void queuePresentKHR(
        VkQueue queue, const VkPresentInfoKHR* presentInfo) const;

    void waitIdle() const;

private:
    [[maybe_unused]] VkDevice _device {};

    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;

#define DEVICE(NAME) PFN_##NAME NAME;
#include <vk/vulkan-functions.hpp>
};

} // namespace rr::vk
