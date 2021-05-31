#define VK_USE_PLATFORM_XLIB_KHR
#define VK_PROTOTYPES
#include <vulkan/vulkan.h>

#include <vector>

#include "platform_window/vulkan.h"
#include "platform_window/platform_window_x11.h"

namespace {
const std::vector<const char*>* GetRequiredInstanceExtensions() {
  static std::vector<const char*> extensions = {
      VK_KHR_XLIB_SURFACE_EXTENSION_NAME,
      "VK_KHR_surface",
  };
  return &extensions;
}
}  // namespace

size_t PlatformWindowVulkanGetRequiredInstanceExtensionsCount() {
  return GetRequiredInstanceExtensions()->size();
}

const char** PlatformWindowVulkanGetRequiredInstanceExtensions() {
  return const_cast<const char**>(GetRequiredInstanceExtensions()->data());
}

VkResult PlatformWindowVulkanCreateSurface(VkInstance vk_instance,
                                           PlatformWindow window,
                                           VkSurfaceKHR* surface) {
  PlatformWindowX11* platform_window_x11 =
      static_cast<PlatformWindowX11*>(window);

  VkXlibSurfaceCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
  create_info.dpy = platform_window_x11->display;
  create_info.window = platform_window_x11->window;

  return vkCreateXlibSurfaceKHR(vk_instance, &create_info, nullptr, surface);
}
