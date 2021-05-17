#ifndef _PLATFORM_WINDOW_PLATFORM_WINDOW_CPP_H_
#define _PLATFORM_WINDOW_PLATFORM_WINDOW_CPP_H_

#include <functional>
#include <optional>

#include "platform_window/platform_window.h"

namespace platform_window {

class Window {
 public:
  Window(const Window&) = delete;
  Window& operator=(const Window&) = delete;
  Window(Window&& x) { operator=(std::move(x)); }
  Window& operator=(Window&& x) {
    event_handler_function_ = std::move(x.event_handler_function_);
    window_ = std::move(x.window_);
    x.window_ = INVALID_PLATFORM_WINDOW;
    return *this;
  }

  using EventHandlerFunction = std::function<void(PlatformWindowEvent)>;

  static std::optional<Window> Create(
      const std::string& title,
      const EventHandlerFunction& event_handler_function);

  ~Window();

  PlatformWindow GetPlatformWindow() { return window_; }

  NativeWindow GetNativeWindow();

  void Show();
  void Hide();
  int32_t GetWidth();
  int32_t GetHeight();

 private:
  Window(std::unique_ptr<EventHandlerFunction> event_handler_function,
         PlatformWindow window)
      : event_handler_function_(std::move(event_handler_function)),
        window_(window){};

  std::unique_ptr<EventHandlerFunction> event_handler_function_;
  PlatformWindow window_;
};

}  // namespace platform_window

#endif  // _PLATFORM_WINDOW_PLATFORM_WINDOW_CPP_H_
