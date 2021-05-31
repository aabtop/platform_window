#ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_X11_H_
#define _PLATFORM_WINDOW_PLATFORM_WINDOW_X11_H_

#ifdef __cplusplus
extern "C" {
#endif

struct PlatformWindowX11 {
  Display* display;
  Window window;
  Atom delete_message;
  XWindowAttributes attributes;
};


#ifdef __cplusplus
}
#endif

#endif  // #ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_X11_H_
