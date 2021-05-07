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
