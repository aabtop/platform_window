#ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_H_
#define _PLATFORM_WINDOW_PLATFORM_WINDOW_H_

#include <cstdint>

#include "platform_window/platform_window_key.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* PlatformWindow;
typedef void* NativeWindow;

const PlatformWindow INVALID_PLATFORM_WINDOW = nullptr;

struct PlatformWindowSize {
  int32_t width;
  int32_t height;
};

enum PlatformWindowEventType {
  kPlatformWindowEventTypeNoEvent,
  kPlatformWindowEventTypeQuitRequest,
  kPlatformWindowEventTypeResized,
  kPlatformWindowEventTypeMouseMove,
  kPlatformWindowEventTypeMouseButton,
  kPlatformWindowEventTypeMouseWheel,
  kPlatformWindowEventTypeKey,
};

struct PlatformWindowEventDataQuitRequest {};

struct PlatformWindowEventDataResized {
  PlatformWindowSize size;
};

struct PlatformWindowEventDataMouseMove {
  // Specified in absolute positions relative to the top left point of the
  // window, in units of pixels.
  int32_t x;
  int32_t y;
};

enum PlatformWindowMouseButton {
  kPlatformWindowMouseLeft,
  kPlatformWindowMouseRight,
  kPlatformWindowMouseUnknown,
  kPlatformWindowMouseCount = kPlatformWindowMouseUnknown,
};
struct PlatformWindowEventDataMouseButton {
  PlatformWindowMouseButton button;
  bool pressed;
  int32_t x;
  int32_t y;
};

struct PlatformWindowEventDataMouseWheel {
  float angle_in_degrees;
};

struct PlatformWindowEventDataKeyEvent {
  bool pressed;
  PlatformWindowKey key;
};

union PlatformWindowEventData {
  PlatformWindowEventDataQuitRequest quit_request;
  PlatformWindowEventDataResized resized;
  PlatformWindowEventDataMouseMove mouse_move;
  PlatformWindowEventDataMouseButton mouse_button;
  PlatformWindowEventDataMouseWheel mouse_wheel;
  PlatformWindowEventDataKeyEvent key;
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
void PlatformWindowDestroyWindow(PlatformWindow window);

NativeWindow PlatformWindowGetNativeWindow(PlatformWindow window);

void PlatformWindowSetTitle(PlatformWindow window, const char* title);

void PlatformWindowShow(PlatformWindow window);
void PlatformWindowHide(PlatformWindow window);

PlatformWindowSize PlatformWindowGetSize(PlatformWindow window);

#ifdef __cplusplus
}
#endif

#endif  // #ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_H_
