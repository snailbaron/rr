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

DEVICE(vkCreateGraphicsPipelines)
DEVICE(vkCreateImageView)
DEVICE(vkCreatePipelineLayout)
DEVICE(vkCreateRenderPass)
DEVICE(vkCreateShaderModule)
DEVICE(vkCreateSwapchainKHR)
DEVICE(vkDestroyDevice)
DEVICE(vkDestroyImageView)
DEVICE(vkDestroyPipeline)
DEVICE(vkDestroyPipelineLayout)
DEVICE(vkDestroyRenderPass)
DEVICE(vkDestroyShaderModule)
DEVICE(vkDestroySwapchainKHR)
DEVICE(vkGetDeviceQueue)
DEVICE(vkGetSwapchainImagesKHR)

#undef GLOBAL
#undef INSTANCE
#undef DEVICE
