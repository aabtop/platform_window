#ifndef _PLATFORM_WINDOW_VULKAN_H_
#define _PLATFORM_WINDOW_VULKAN_H_

#include <vulkan/vulkan.h>

#include <cstdint>

#include "platform_window/platform_window.h"

#ifdef __cplusplus
extern "C" {
#endif

int32_t PlatformWindowVulkanGetRequiredInstanceExtensionsCount();
const char** PlatformWindowVulkanGetRequiredInstanceExtensions();
VkResult PlatformWindowVulkanCreateSurface(VkInstance vk_instance,
                                           PlatformWindow window,
                                           VkSurfaceKHR* surface);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef _PLATFORM_WINDOW_VULKAN_H_
