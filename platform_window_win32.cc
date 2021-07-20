#include <windows.h>
#include <windowsx.h>

#include <algorithm>
#include <array>
#include <cassert>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "platform_window/platform_window.h"

namespace {
const int kInitialWindowWidth = 1920;
const int kInitialWindowHeight = 1080;

class Window {
 public:
  Window(const char* title, PlatformWindowEventCallback event_callback,
         void* event_callback_context);
  ~Window();

  bool error() { return hwnd() == NULL; }
  HWND hwnd() {
    WaitForInitialization();

    return hwnd_;
  }

  void Show();
  void Hide();

  void SetTitle(const char* title);

  PlatformWindowSize GetSize();

 private:
  void WaitForInitialization() {
    std::unique_lock lock(mutex_);
    initialized_condition_.wait(lock, [this] { return initialized_; });
  }
  void Run(const char* title);
  void Start(const char* title);
  void Shutdown();

  long OnEvent(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp);

  bool any_buttons_pressed() const {
    return std::any_of(is_pressed_.begin(), is_pressed_.end(),
                       [](bool x) { return x; });
  }
  bool initialized_ = false;
  PlatformWindowEventCallback event_callback_;
  void* context_;
  PlatformWindowSize size_;

  std::mutex mutex_;
  std::condition_variable initialized_condition_;
  std::thread thread_;

  HWND hwnd_ = NULL;

  std::array<bool, kPlatformWindowMouseCount> is_pressed_;

  friend LRESULT CALLBACK HandleWindowEvent(HWND, UINT, WPARAM, LPARAM);
};

thread_local Window* s_window = nullptr;

LRESULT CALLBACK HandleWindowEvent(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  return s_window->OnEvent(hwnd, msg, wp, lp);
}

bool PumpNextWindowEvent() {
  MSG msg;
  if (!GetMessage(&msg, 0, 0, 0)) {
    return false;
  } else {
    DispatchMessage(&msg);
    return true;
  }
}

Window::Window(const char* title, PlatformWindowEventCallback event_callback,
               void* event_callback_context)
    : event_callback_(event_callback),
      context_(event_callback_context),
      size_{kInitialWindowWidth, kInitialWindowHeight},
      thread_(&Window::Run, this, title) {
  is_pressed_.fill(false);
}

Window::~Window() {
  if (!error()) {
    PostMessageA(hwnd(), WM_QUIT, 0, 0);
  }

  thread_.join();
}

void Window::Show() { ShowWindow(hwnd_, SW_SHOWDEFAULT); }
void Window::Hide() { ShowWindow(hwnd_, SW_HIDE); }

void Window::SetTitle(const char* title) { SetWindowTextA(hwnd_, title); }

PlatformWindowSize Window::GetSize() {
  std::lock_guard<std::mutex> lock(mutex_);
  return size_;
}

void Window::Run(const char* title) {
  Start(title);
  if (hwnd_ == NULL) {
    return;
  }

  while (PumpNextWindowEvent()) {
  }

  Shutdown();
}

const LPCSTR CreateWindowClass() {
  const LPCSTR myclass = "myclass";
  WNDCLASSEX wndclass = {sizeof(WNDCLASSEX),
                         CS_DBLCLKS,
                         HandleWindowEvent,
                         0,
                         0,
                         GetModuleHandle(0),
                         LoadIcon(0, IDI_APPLICATION),
                         LoadCursor(0, IDC_ARROW),
                         HBRUSH(COLOR_WINDOW + 1),
                         0,
                         myclass,
                         LoadIcon(0, IDI_APPLICATION)};

  ATOM registered_class = RegisterClassEx(&wndclass);
  assert(registered_class);

  return myclass;
}

void Window::Start(const char* title) {
  static auto window_class = CreateWindowClass();

  s_window = this;

  DWORD style =
      WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_THICKFRAME;

  PlatformWindowSize size = GetSize();
  hwnd_ = CreateWindowEx(0, window_class, title, style, CW_USEDEFAULT,
                         CW_USEDEFAULT, size.width, size.height, 0, 0,
                         GetModuleHandle(0), NULL);

  std::lock_guard<std::mutex> lock(mutex_);
  initialized_ = true;
  initialized_condition_.notify_all();
}

void Window::Shutdown() { DestroyWindow(hwnd_); }

long Window::OnEvent(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
  switch (msg) {
    case WM_CLOSE: {
      event_callback_(context_, {kPlatformWindowEventTypeQuitRequest, {}});
      return 0;
    } break;
    case WM_SIZE: {
      PlatformWindowEventData data;
      data.resized = PlatformWindowEventDataResized{{LOWORD(lp), HIWORD(lp)}};

      {
        std::lock_guard<std::mutex> lock(mutex_);
        size_ = data.resized.size;
      }
      event_callback_(context_, {kPlatformWindowEventTypeResized, data});
      return 0;
    } break;
    case WM_LBUTTONDBLCLK:
    case WM_RBUTTONDBLCLK:
    case WM_LBUTTONDOWN:
    case WM_RBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONUP: {
      PlatformWindowEventData data{};
      data.mouse_button.button = [msg] {
        switch (msg) {
          case WM_LBUTTONDBLCLK:
          case WM_LBUTTONDOWN:
          case WM_LBUTTONUP:
            return kPlatformWindowMouseLeft;
          case WM_RBUTTONDBLCLK:
          case WM_RBUTTONDOWN:
          case WM_RBUTTONUP:
            return kPlatformWindowMouseRight;
          default:
            return kPlatformWindowMouseUnknown;
        };
      }();
      data.mouse_button.pressed = [msg] {
        switch (msg) {
          case WM_LBUTTONDBLCLK:
          case WM_LBUTTONDOWN:
          case WM_RBUTTONDOWN:
            return true;
          case WM_RBUTTONDBLCLK:
          case WM_LBUTTONUP:
          case WM_RBUTTONUP:
            return false;
          default:
            return false;
        };
      }();
      data.mouse_button.x = GET_X_LPARAM(lp);
      data.mouse_button.y = GET_Y_LPARAM(lp);
      event_callback_(context_, {kPlatformWindowEventTypeMouseButton, data});

      bool were_buttons_previously_pressed = any_buttons_pressed();
      is_pressed_[data.mouse_button.button] = data.mouse_button.pressed;
      if (data.mouse_button.pressed) {
        if (!were_buttons_previously_pressed) {
          SetCapture(hwnd);
        }
      } else {
        if (!any_buttons_pressed()) {
          ReleaseCapture();
        }
      }
      return 0;
    } break;
    case WM_MOUSEMOVE: {
      PlatformWindowEventData data{};
      data.mouse_move.x = GET_X_LPARAM(lp);
      data.mouse_move.y = GET_Y_LPARAM(lp);
      event_callback_(context_, {kPlatformWindowEventTypeMouseMove, data});
      return 0;
    } break;
    case WM_MOUSEWHEEL: {
      PlatformWindowEventData data{};
      // This is a bit of an arbitrary value chosen as what kind of felt best.
      constexpr float WHEEL_DELTAS_PER_DEGREE = 10.0f;
      data.mouse_wheel.angle_in_degrees =
          static_cast<float>(GET_WHEEL_DELTA_WPARAM(wp)) /
          WHEEL_DELTAS_PER_DEGREE;
      data.mouse_wheel.x = GET_X_LPARAM(lp);
      data.mouse_wheel.y = GET_Y_LPARAM(lp);
      event_callback_(context_, {kPlatformWindowEventTypeMouseWheel, data});
      return 0;
    } break;
    case WM_CAPTURECHANGED: {
      for (size_t i = 0; i < kPlatformWindowMouseCount; ++i) {
        if (is_pressed_[i]) {
          PlatformWindowEventData data{};
          data.mouse_button = {static_cast<PlatformWindowMouseButton>(i), false,
                               GET_X_LPARAM(lp), GET_Y_LPARAM(lp)};
          event_callback_(context_,
                          {kPlatformWindowEventTypeMouseButton, data});
          is_pressed_[i] = false;
        }
      }
      // Probably makese sense to somehow propagate this...

      is_pressed_.fill(false);
      return 0;
    } break;
    case WM_KEYDOWN:
    case WM_KEYUP: {
      PlatformWindowEventData data{};
      data.key.key = static_cast<PlatformWindowKey>(wp);
      data.key.pressed = (msg == WM_KEYDOWN);
      event_callback_(context_, {kPlatformWindowEventTypeKey, data});
      return 0;
    } break;
    default: {
      return DefWindowProc(hwnd, msg, wp, lp);
    }
  }
}

}  // namespace

PlatformWindow PlatformWindowMakeDefaultWindow(
    const char* title, PlatformWindowEventCallback event_handler_func,
    void* context) {
  auto window = std::make_unique<Window>(title, event_handler_func, context);
  if (window->error()) {
    return INVALID_PLATFORM_WINDOW;
  } else {
    return window.release();
  }
}

void PlatformWindowDestroyWindow(PlatformWindow platform_window) {
  assert(platform_window != NULL);
  delete static_cast<Window*>(platform_window);
}

NativeWindow PlatformWindowGetNativeWindow(PlatformWindow platform_window) {
  return static_cast<NativeWindow>(
      static_cast<Window*>(platform_window)->hwnd());
}

void PlatformWindowShow(PlatformWindow platform_window) {
  static_cast<Window*>(platform_window)->Show();
}

void PlatformWindowHide(PlatformWindow platform_window) {
  static_cast<Window*>(platform_window)->Hide();
}

void PlatformWindowSetTitle(PlatformWindow platform_window, const char* title) {
  static_cast<Window*>(platform_window)->SetTitle(title);
}

PlatformWindowSize PlatformWindowGetSize(PlatformWindow platform_window) {
  return static_cast<Window*>(platform_window)->GetSize();
}