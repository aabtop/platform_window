#ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_H_
#define _PLATFORM_WINDOW_PLATFORM_WINDOW_H_

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* PlatformWindow;
typedef void* NativeWindow;

enum PlatformWindowEventType {
  kPlatformWindowEventTypeNoEvent,
  kPlatformWindowEventTypeQuitRequest,
  kPlatformWindowEventTypeResized,
  kPlatformWindowEventTypeCustom,
};

struct PlatformWindowEventDataCustom {
  // If your payload can fit into an integer, it can make things simpler
  // to just stuff it into this `small_data` field.
  int small_data;
  // Otherwise, the `more_data` field can be used to specify arbitrarily
  // more information (at the cost of more involved data lifetime management).
  void* more_data;
};

union PlatformWindowEventData {
  struct {
  } quit_request;
  struct {
    int width;
    int height;
  } resized;
  PlatformWindowEventDataCustom custom;
};

struct PlatformWindowEvent {
  PlatformWindowEventType type;
  PlatformWindowEventData data;
};

typedef void (*PlatformWindowEventCallback)(void* context,
                                            PlatformWindowEvent event);

// The |event_callback| may be called from an arbitrary thread.
PlatformWindow PlatformWindowMakeDefaultWindow(
    const char* title, PlatformWindowEventCallback event_handler_func,
    void* context);
void PlatformWindowDestroyWindow(PlatformWindow platform_window);

NativeWindow PlatformWindowGetNativeWindow(PlatformWindow platform_window);

void PlatformWindowShow(PlatformWindow platform_window);
void PlatformWindowHide(PlatformWindow platform_window);

int32_t PlatformWindowGetWidth(PlatformWindow window);
int32_t PlatformWindowGetHeight(PlatformWindow window);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_H_
