#include "platform_window/platform_window_cpp.h"

namespace platform_window {

std::optional<Window> Window::Create(
    const std::string& title,
    const EventHandlerFunction& event_handler_function) {
  auto event_handler_function_ptr =
      std::make_unique<EventHandlerFunction>(event_handler_function);

  PlatformWindow window = PlatformWindowMakeDefaultWindow(
      title.c_str(),
      [](void* context, PlatformWindowEvent event) {
        (*reinterpret_cast<EventHandlerFunction*>(context))(event);
      },
      event_handler_function_ptr.get());

  if (window == INVALID_PLATFORM_WINDOW) {
    return std::nullopt;
  }

  return Window(std::move(event_handler_function_ptr), std::move(window));
}

Window::~Window() {
  if (window_ != INVALID_PLATFORM_WINDOW) {
    PlatformWindowDestroyWindow(window_);
  }
}

NativeWindow Window::GetNativeWindow() {
  return PlatformWindowGetNativeWindow(window_);
}

void Window::Show() { PlatformWindowShow(window_); }
void Window::Hide() { PlatformWindowHide(window_); }

int32_t Window::GetWidth() { return PlatformWindowGetWidth(window_); }
int32_t Window::GetHeight() { return PlatformWindowGetWidth(window_); }

}  // namespace platform_window
