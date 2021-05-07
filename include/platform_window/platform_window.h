#ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_H_
#define _PLATFORM_WINDOW_PLATFORM_WINDOW_H_

#include <cstdint>

#ifdef __cplusplus
extern "C" {
#endif

typedef void* PlatformWindow;
typedef void* NativeWindow;

enum PlatformWindowEvent {
  kPlatformWindowEventQuitRequest,
};

typedef void (*PlatformWindowEventCallback)(
    void*,                // Context data passed in by the caller.
    PlatformWindowEvent,  // The type of newly fired event.
    void*                 // Event data.
);

// The |event_callback| may be called from an arbitrary thread.
PlatformWindow PlatformWindowMakeDefaultWindow(
    const char* title, PlatformWindowEventCallback event_callback,
    void* context);
void PlatformWindowDestroyWindow(PlatformWindow platform_window);

NativeWindow PlatformWindowGetNativeWindow(PlatformWindow platform_window);

int32_t PlatformWindowGetWidth(PlatformWindow window);
int32_t PlatformWindowGetHeight(PlatformWindow window);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_H_
