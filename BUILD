cc_library(
  name = "platform_window",
  deps = [":platform_window_headers"] + select({
        "@bazel_tools//src/conditions:windows": [":platform_window_win32"],
        "//conditions:default": [":platform_window_x11"],
  }),
  visibility = ["//visibility:public"],
)

cc_library(
  name = "platform_window_headers",
  hdrs = [
    "include/platform_window/platform_window.h",
  ],
  includes = [
    "include",
  ],
)

cc_library(
  name = "cpp",
  hdrs = [
    "include/platform_window/platform_window_cpp.h",
  ],
  srcs = [
    "platform_window_cpp.cc",
  ],
  includes = [
    "include",
  ],
  deps = [":platform_window"],
  visibility = ["//visibility:public"],
)

cc_library(
  name = "platform_window_win32",
  srcs = [
    "platform_window_win32.cc",
  ],
  deps = [
    ":platform_window_headers",
  ],
)

cc_library(
  name = "platform_window_x11",
  hdrs = [
      "include/platform_window/platform_window_x11.h",
  ],
  srcs = [
    "platform_window_x11.cc",
  ],
  linkopts = [
    "-lX11",
  ],
  includes = [
    "include",
  ],
  deps = [
    ":platform_window_headers",
  ],
)

cc_library(
  name = "vulkan",
  deps = [
    ":vulkan_headers",
    "@vulkan_sdk//:vulkan",
  ] + select({
        "@bazel_tools//src/conditions:windows": [":vulkan_win32"],
        "//conditions:default": [":vulkan_x11"],
  }),
  visibility = ["//visibility:public"],
)

cc_library(
  name = "vulkan_headers",
  hdrs = [
    "include/platform_window/vulkan.h",
  ],
  includes = [
    "include",
  ],
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
    ":vulkan_headers",
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
  linkopts = [
    "-lX11",
  ],
  deps=[
    ":vulkan_headers",
    "@vulkan_sdk//:vulkan",
    ":platform_window",
    ":platform_window_x11",
  ],
)