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
};

union PlatformWindowEventData {
  struct {
  } QuitRequest;
  struct {
    int width;
    int height;
  } Resized;
};

struct PlatformWindowEvent {
  PlatformWindowEventType type;
  PlatformWindowEventData data;
};

// The |event_callback| may be called from an arbitrary thread.
PlatformWindow PlatformWindowMakeDefaultWindow(const char* title);
void PlatformWindowDestroyWindow(PlatformWindow platform_window);

NativeWindow PlatformWindowGetNativeWindow(PlatformWindow platform_window);

void PlatformWindowShow(PlatformWindow platform_window);
void PlatformWindowHide(PlatformWindow platform_window);

PlatformWindowEvent PlatformWindowWaitForNextEvent(
    PlatformWindow platform_window);

int32_t PlatformWindowGetWidth(PlatformWindow window);
int32_t PlatformWindowGetHeight(PlatformWindow window);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_H_
