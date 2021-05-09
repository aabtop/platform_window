cc_library(
  name = "platform_window",
  hdrs = [
    "include/platform_window/platform_window.h",
  ],
  includes = [
    "include",
  ],
  deps = select({
        "@bazel_tools//src/conditions:windows": [":platform_window_win32"],
        "//conditions:default": [":platform_window_x11"],
  }),
  visibility = ["//visibility:public"],
)

cc_library(
  name = "platform_window_win32",
  hdrs = [
    "include/platform_window/platform_window.h",
  ],
  includes = [
    "include",
  ],
  srcs = [
    "platform_window_win32.cc",
  ],
)

cc_library(
  name = "platform_window_x11",
  hdrs = [
    "include/platform_window/platform_window.h",
  ],
  includes = [
    "include",
  ],
  srcs = [
    "platform_window_x11.cc",
  ],
  copts = [
    "-lX11",
  ],
)

cc_library(
  name = "vulkan",
  hdrs = [
    "include/platform_window/vulkan.h",
  ],
  includes = [
    "include",
  ],
  deps = [
    "@vulkan_sdk//:vulkan",
  ] + select({
        "@bazel_tools//src/conditions:windows": [":vulkan_win32"],
        "//conditions:default": [":vulkan_x11"],
  }),
  visibility = ["//visibility:public"],
)

cc_library(
  name = "vulkan_win32",
  includes = [
    "include",
  ],
  hdrs = [
    "include/platform_window/vulkan.h",
  ],
  srcs = [
    "vulkan_win32.cc",
  ],
  deps = [
    "@vulkan_sdk//:vulkan",
    ":platform_window",
  ],
)

cc_library(
  name = "vulkan_x11",
  hdrs = [
    "include/platform_window/vulkan.h",
  ],
  includes = [
    "include",
  ],
  srcs = [
    "vulkan_x11.cc",
  ],
  copts = [
    "-lX11",
  ],
  deps=[
    "@vulkan_sdk//:vulkan",
    ":platform_window",
  ],
)