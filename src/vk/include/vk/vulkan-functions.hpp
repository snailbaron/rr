#ifndef GLOBAL
    #define GLOBAL(NAME)
#endif
#ifndef INSTANCE
    #define INSTANCE(NAME)
#endif
#ifndef DEVICE
    #define DEVICE(NAME)
#endif

GLOBAL(vkCreateInstance)
GLOBAL(vkEnumerateInstanceExtensionProperties)
GLOBAL(vkEnumerateInstanceLayerProperties)
GLOBAL(vkEnumerateInstanceVersion)

INSTANCE(vkCreateDebugUtilsMessengerEXT)
INSTANCE(vkCreateDevice)
INSTANCE(vkCreateXcbSurfaceKHR)
INSTANCE(vkDestroyDebugUtilsMessengerEXT)
INSTANCE(vkDestroyInstance)
INSTANCE(vkDestroySurfaceKHR)
INSTANCE(vkEnumerateDeviceExtensionProperties)
INSTANCE(vkEnumeratePhysicalDevices)
INSTANCE(vkGetDeviceProcAddr)
INSTANCE(vkGetPhysicalDeviceFeatures)
INSTANCE(vkGetPhysicalDeviceProperties)
INSTANCE(vkGetPhysicalDeviceQueueFamilyProperties)
INSTANCE(vkGetPhysicalDeviceSurfaceCapabilitiesKHR)
INSTANCE(vkGetPhysicalDeviceSurfaceFormatsKHR)
INSTANCE(vkGetPhysicalDeviceSurfacePresentModesKHR)
INSTANCE(vkGetPhysicalDeviceSurfaceSupportKHR)

DEVICE(vkAcquireNextImageKHR)
DEVICE(vkAllocateCommandBuffers)
DEVICE(vkBeginCommandBuffer)
DEVICE(vkCmdBeginRenderPass)
DEVICE(vkCmdBindPipeline)
DEVICE(vkCmdDraw)
DEVICE(vkCmdEndRenderPass)
DEVICE(vkCmdSetScissor)
DEVICE(vkCmdSetViewport)
DEVICE(vkCreateCommandPool)
DEVICE(vkCreateFence)
DEVICE(vkCreateFramebuffer)
DEVICE(vkCreateGraphicsPipelines)
DEVICE(vkCreateImageView)
DEVICE(vkCreatePipelineLayout)
DEVICE(vkCreateRenderPass)
DEVICE(vkCreateSemaphore)
DEVICE(vkCreateShaderModule)
DEVICE(vkCreateSwapchainKHR)
DEVICE(vkDestroyCommandPool)
DEVICE(vkDestroyDevice)
DEVICE(vkDestroyFence)
DEVICE(vkDestroyFramebuffer)
DEVICE(vkDestroyImageView)
DEVICE(vkDestroyPipeline)
DEVICE(vkDestroyPipelineLayout)
DEVICE(vkDestroyRenderPass)
DEVICE(vkDestroySemaphore)
DEVICE(vkDestroyShaderModule)
DEVICE(vkDestroySwapchainKHR)
DEVICE(vkDeviceWaitIdle)
DEVICE(vkEndCommandBuffer)
DEVICE(vkGetDeviceQueue)
DEVICE(vkGetSwapchainImagesKHR)
DEVICE(vkQueuePresentKHR)
DEVICE(vkQueueSubmit)
DEVICE(vkResetCommandBuffer)
DEVICE(vkResetFences)
DEVICE(vkWaitForFences)

#undef GLOBAL
#undef INSTANCE
#undef DEVICE
