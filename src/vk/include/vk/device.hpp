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

    std::vector<VkPipeline> createGraphicsPipelines(
        VkPipelineCache pipelineCache,
        uint32_t createInfoCount,
        const VkGraphicsPipelineCreateInfo* createInfos,
        const VkAllocationCallbacks* allocator) const;

private:
    [[maybe_unused]] VkDevice _device {};

    PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr;

#define DEVICE(NAME) PFN_##NAME NAME;
#include <vk/vulkan-functions.hpp>
};

} // namespace rr::vk
