#include <windows.h>

#include <cassert>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>

#include "platform_window/platform_window.h"

namespace {
const int kWindowWidth = 1920;
const int kWindowHeight = 1080;

class Window {
 public:
  Window(const char* title);
  ~Window();

  bool error() { return hwnd() == NULL; }
  HWND hwnd() { return hwnd_; }

  void Show() { ShowWindow(hwnd_, SW_SHOWDEFAULT); }
  void Hide() { ShowWindow(hwnd_, SW_HIDE); }

  PlatformWindowEvent WaitForNextEvent();

 private:
  void OnEvent(const PlatformWindowEvent& event);

  HWND hwnd_ = NULL;

  std::queue<PlatformWindowEvent> event_queue_;

  friend long __stdcall HandleWindowEvent(HWND, unsigned int, WPARAM, LPARAM);
};

thread_local Window* s_window = nullptr;

long __stdcall HandleWindowEvent(HWND window, unsigned int msg, WPARAM wp,
                                 LPARAM lp) {
  switch (msg) {
    case WM_CLOSE: {
      s_window->OnEvent({kPlatformWindowEventTypeQuitRequest, {}});
      return 0L;
    } break;
    default: {
      return DefWindowProc(window, msg, wp, lp);
    }
  }
}

PlatformWindowEvent Window::WaitForNextEvent() {
  MSG msg;
  if (!GetMessage(&msg, 0, 0, 0)) {
    return {kPlatformWindowEventTypeQuitRequest, {}};
  } else {
    DispatchMessage(&msg);
    if (!event_queue_.empty()) {
      PlatformWindowEvent result = event_queue_.front();
      event_queue_.pop();
      return result;
    } else {
      return {kPlatformWindowEventTypeNoEvent, {}};
    }
  }
}

namespace {
const char* const CreateWindowClass() {
  const char* const myclass = "myclass";
  WNDCLASSEX wndclass = {sizeof(WNDCLASSEX),
                         CS_DBLCLKS,
                         (WNDPROC)HandleWindowEvent,
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
}  // namespace

Window::Window(const char* title) {
  static auto window_class = CreateWindowClass();

  s_window = this;

  DWORD style = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;

  hwnd_ = CreateWindowEx(0, window_class, title, style, CW_USEDEFAULT,
                         CW_USEDEFAULT, kWindowWidth, kWindowHeight, 0, 0,
                         GetModuleHandle(0), NULL);
}

Window::~Window() {
  if (!error()) {
    PostMessageA(hwnd(), WM_QUIT, 0, 0);
    DestroyWindow(hwnd_);
  }
}

void Window::OnEvent(const PlatformWindowEvent& event) {
  event_queue_.push(event);
}

}  // namespace

PlatformWindow PlatformWindowMakeDefaultWindow(const char* title) {
  auto window = std::make_unique<Window>(title);
  if (window->error()) {
    return NULL;
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

PlatformWindowEvent PlatformWindowWaitForNextEvent(
    PlatformWindow platform_window) {
  return static_cast<Window*>(platform_window)->WaitForNextEvent();
}

int32_t PlatformWindowGetWidth(PlatformWindow window) { return kWindowWidth; }

int32_t PlatformWindowGetHeight(PlatformWindow window) { return kWindowHeight; }
