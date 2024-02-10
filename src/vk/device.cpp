#include <vk/device.hpp>

#include <vk/check.hpp>

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
    auto queue = VkQueue{};
    vkGetDeviceQueue(_device, queueFamilyIndex, queueIndex, &queue);
    return queue;
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

} // namespace rr::vk
