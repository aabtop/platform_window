// clang-format off
#include <windows.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_win32.h>
// clang-format on

#include <vector>

#include "platform_window/vulkan.h"

namespace {
const std::vector<const char*>* GetRequiredInstanceExtensions() {
  static std::vector<const char*> extensions = {
      "VK_KHR_win32_surface",
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
  VkWin32SurfaceCreateInfoKHR create_info{};
  create_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
  create_info.hwnd = static_cast<HWND>(PlatformWindowGetNativeWindow(window));
  create_info.hinstance = GetModuleHandle(nullptr);

  return vkCreateWin32SurfaceKHR(vk_instance, &create_info, nullptr, surface);
}
