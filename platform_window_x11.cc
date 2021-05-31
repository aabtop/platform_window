#include "platform_window/platform_window.h"

#include <cassert>
#include <chrono>
#include <thread>

#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include "platform_window/platform_window_x11.h"

PlatformWindow PlatformWindowMakeDefaultWindow(
    const char* title, PlatformWindowEventCallback event_callback,
    void* context) {
  std::unique_ptr<PlatformWindowX11> window(new PlatformWindowX11());
  window->display = XOpenDisplay(NULL);
  assert(window->display);

  Window root_window = DefaultRootWindow(window->display);

  const bool kFullscreen = true;

  XWindowAttributes root_window_attributes;
  XGetWindowAttributes(window->display, root_window, &root_window_attributes);

  XSetWindowAttributes window_attributes;
  window_attributes.border_pixel = 0;
  window_attributes.event_mask =
      ExposureMask | PointerMotionMask | KeyPressMask;
  window_attributes.override_redirect = (kFullscreen ? True : False);

  window->window = XCreateWindow(
      window->display, root_window,
      0, 0, root_window_attributes.width, root_window_attributes.height, 0,
      CopyFromParent, InputOutput,
      CopyFromParent, CWBorderPixel | CWEventMask |
                      (kFullscreen ? CWOverrideRedirect : 0),
      &window_attributes);

  // Hide the mouse cursor in fullscreen mode.
  if (kFullscreen) {
    XUndefineCursor(window->display, window->window);
    // TODO: Actually focus the window or something so that the mouse cursor
    // change takes effect, otherwise you have to actually click.
  }

  window->delete_message =
      XInternAtom(window->display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(window->display, window->window, &window->delete_message, 1);

  XWMHints hints;
  hints.input = True;
  hints.flags = InputHint;
  XSetWMHints(window->display, window->window, &hints);

  // make the window visible on the screen
  XStoreName(window->display, window->window, title);

  XGetWindowAttributes(window->display, root_window, &window->attributes);

  return window.release();
}

void PlatformWindowDestroyWindow(PlatformWindow platform_window) {
  delete static_cast<PlatformWindowX11*>(platform_window);
}

NativeWindow PlatformWindowGetNativeWindow(PlatformWindow platform_window) {
  return reinterpret_cast<NativeWindow>(
      static_cast<PlatformWindowX11*>(platform_window)->window);
}

void PlatformWindowShow(PlatformWindow window) {
  auto platform_window_x11 = static_cast<PlatformWindowX11*>(window);

  XMapWindow(platform_window_x11->display, platform_window_x11->window);
  XFlush(platform_window_x11->display);
}

void PlatformWindowHide(PlatformWindow window) {
  auto platform_window_x11 = static_cast<PlatformWindowX11*>(window);
  
  XUnmapWindow(platform_window_x11->display, platform_window_x11->window);
  XFlush(platform_window_x11->display);
}

PlatformWindowSize PlatformWindowGetSize(PlatformWindow window) {
  return {
      static_cast<PlatformWindowX11*>(window)->attributes.width,
      static_cast<PlatformWindowX11*>(window)->attributes.height};
}

