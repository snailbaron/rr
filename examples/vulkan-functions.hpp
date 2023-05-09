#ifndef VULKAN_FUNCTION
#define VULKAN_FUNCTION(NAME)
#endif

#ifndef VULKAN_INSTANCE_COMMAND
#define VULKAN_INSTANCE_COMMAND(NAME)
#endif

#ifndef VULKAN_DEVICE_COMMAND
#define VULKAN_DEVICE_COMMAND(NAME)
#endif

#ifndef VULKAN_XCB_FUNCTION
#define VULKAN_XCB_FUNCTION(NAME)
#endif

// global commands
VULKAN_FUNCTION(vkCreateInstance)
VULKAN_FUNCTION(vkEnumerateInstanceExtensionProperties)
VULKAN_FUNCTION(vkEnumerateInstanceLayerProperties)
VULKAN_FUNCTION(vkEnumerateInstanceVersion)

// instance commands
VULKAN_INSTANCE_COMMAND(vkCreateDevice)
VULKAN_INSTANCE_COMMAND(vkEnumeratePhysicalDevices)
VULKAN_INSTANCE_COMMAND(vkGetDeviceProcAddr)
VULKAN_INSTANCE_COMMAND(vkGetPhysicalDeviceProperties)
VULKAN_INSTANCE_COMMAND(vkGetPhysicalDeviceQueueFamilyProperties)

// device commands
VULKAN_DEVICE_COMMAND(vkCreateCommandPool)

// XCB
VULKAN_XCB_FUNCTION(vkCreateXcbSurfaceKHR)
VULKAN_XCB_FUNCTION(vkGetPhysicalDeviceXcbPresentationSupportKHR)

#undef VULKAN_FUNCTION
#undef VULKAN_INSTANCE_COMMAND
#undef VULKAN_DEVICE_COMMAND
#undef VULKAN_XCB_FUNCTION
