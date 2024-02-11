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

VkFramebuffer Device::createFramebuffer(
    const VkFramebufferCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkFramebuffer>(
        vkCreateFramebuffer, _device, createInfo, allocator);
}

void Device::destroyFramebuffer(
    VkFramebuffer framebuffer,
    const VkAllocationCallbacks* allocator) const
{
    vkDestroyFramebuffer(_device, framebuffer, allocator);
}

VkCommandPool Device::createCommandPool(
    const VkCommandPoolCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkCommandPool>(
        vkCreateCommandPool, _device, createInfo, allocator);
}

void Device::destroyCommandPool(
    VkCommandPool commandPool,
    const VkAllocationCallbacks* allocator) const
{
    vkDestroyCommandPool(_device, commandPool, allocator);
}

std::vector<VkCommandBuffer> Device::allocateCommandBuffers(
    const VkCommandBufferAllocateInfo* allocateInfo) const
{
    auto commandBuffers =
        std::vector<VkCommandBuffer>(allocateInfo->commandBufferCount);
    check << vkAllocateCommandBuffers(
        _device, allocateInfo, commandBuffers.data());
    return commandBuffers;
}

void Device::resetCommandBuffer(
    VkCommandBuffer commandBuffer,
    VkCommandBufferResetFlags flags) const
{
    check << vkResetCommandBuffer(commandBuffer, flags);
}

void Device::beginCommandBuffer(
    VkCommandBuffer commandBuffer,
    const VkCommandBufferBeginInfo* beginInfo) const
{
    check << vkBeginCommandBuffer(commandBuffer, beginInfo);
}

void Device::endCommandBuffer(VkCommandBuffer commandBuffer) const
{
    check << vkEndCommandBuffer(commandBuffer);
}

void Device::cmdBeginRenderPass(
    VkCommandBuffer commandBuffer,
    const VkRenderPassBeginInfo* renderPassBegin,
    VkSubpassContents contents) const
{
    vkCmdBeginRenderPass(commandBuffer, renderPassBegin, contents);
}

void Device::cmdBindPipeline(
    VkCommandBuffer commandBuffer,
    VkPipelineBindPoint pipelineBindPoint,
    VkPipeline pipeline) const
{
    vkCmdBindPipeline(commandBuffer, pipelineBindPoint, pipeline);
}

void Device::cmdSetViewport(
    VkCommandBuffer commandBuffer,
    uint32_t firstViewport,
    uint32_t viewportCount,
    const VkViewport* viewports) const
{
    vkCmdSetViewport(commandBuffer, firstViewport, viewportCount, viewports);
}

void Device::cmdSetScissor(
    VkCommandBuffer commandBuffer,
    uint32_t firstScissor,
    uint32_t scissorCount,
    const VkRect2D* scissors) const
{
    vkCmdSetScissor(commandBuffer, firstScissor, scissorCount, scissors);
}

void Device::cmdDraw(
    VkCommandBuffer commandBuffer,
    uint32_t vertexCount,
    uint32_t instanceCount,
    uint32_t firstVertex,
    uint32_t firstInstance) const
{
    vkCmdDraw(
        commandBuffer, vertexCount, instanceCount, firstVertex, firstInstance);
}

void Device::cmdEndRenderPass(VkCommandBuffer commandBuffer) const
{
    vkCmdEndRenderPass(commandBuffer);
}

VkSemaphore Device::createSemaphore(
    const VkSemaphoreCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkSemaphore>(
        vkCreateSemaphore, _device, createInfo, allocator);
}

void Device::destroySemaphore(
    VkSemaphore semaphore,
    const VkAllocationCallbacks* allocator) const
{
    vkDestroySemaphore(_device, semaphore, allocator);
}

VkFence Device::createFence(
    const VkFenceCreateInfo* createInfo,
    const VkAllocationCallbacks* allocator) const
{
    return getVkObject<VkFence>(
        vkCreateFence, _device, createInfo, allocator);
}

void Device::destroyFence(
    VkFence fence,
    const VkAllocationCallbacks* allocator) const
{
    vkDestroyFence(_device, fence, allocator);
}

void Device::waitForFences(
    uint32_t fenceCount,
    const VkFence* fences,
    VkBool32 waitAll,
    uint64_t timeout) const
{
    check << vkWaitForFences(_device, fenceCount, fences, waitAll, timeout);
}

void Device::resetFences(
    uint32_t fenceCount,
    const VkFence* fences) const
{
    check << vkResetFences(_device, fenceCount, fences);
}

uint32_t Device::acquireNextImageKHR(
    VkSwapchainKHR swapchain,
    uint64_t timeout,
    VkSemaphore semaphore,
    VkFence fence) const
{
    return getVkObject<uint32_t>(
        vkAcquireNextImageKHR, _device, swapchain, timeout, semaphore, fence);
}

void Device::queueSubmit(
    VkQueue queue,
    uint32_t submitCount,
    const VkSubmitInfo* submits,
    VkFence fence) const
{
    check << vkQueueSubmit(queue, submitCount, submits, fence);
}

void Device::queuePresentKHR(
    VkQueue queue, const VkPresentInfoKHR* presentInfo) const
{
    check << vkQueuePresentKHR(queue, presentInfo);
}

void Device::waitIdle() const
{
    check << vkDeviceWaitIdle(_device);
}

} // namespace rr::vk
