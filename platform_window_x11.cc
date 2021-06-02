#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "platform_window/platform_window.h"

namespace {
class PlatformWindowX11 {
 public:
  PlatformWindowX11(PlatformWindowEventCallback event_callback,
                    void* callback_context, Display* display, Window window,
                    Atom delete_atom, Atom shutdown_atom,
                    XWindowAttributes attributes);
  ~PlatformWindowX11();

  Window window() const { return window_; }

  void Show();
  void Hide();

  PlatformWindowSize GetSize() const;

 private:
  void Run();

  PlatformWindowEventCallback event_callback_;
  void* callback_context_;

  Display* display_;
  Window window_;
  Atom delete_atom_;
  Atom shutdown_atom_;
  XWindowAttributes attributes_;

  std::thread thread_;
};

PlatformWindowX11::PlatformWindowX11(PlatformWindowEventCallback event_callback,
                                     void* callback_context, Display* display,
                                     Window window, Atom delete_atom,
                                     Atom shutdown_atom,
                                     XWindowAttributes attributes)
    : event_callback_(event_callback),
      callback_context_(callback_context),
      display_(display),
      window_(window),
      delete_atom_(delete_atom),
      shutdown_atom_(shutdown_atom),
      attributes_(attributes),
      thread_([this] { Run(); }) {}

PlatformWindowX11::~PlatformWindowX11() {
  // XLib is not thread-safe. Since we're on another thread, we have to open
  // another connection to the display to inject the wake-up event.
  Display* display = XOpenDisplay(NULL);
  XClientMessageEvent event = {0};
  event.type = ClientMessage;
  event.message_type = shutdown_atom_;
  event.window = window_;
  event.format = 32;
  XSendEvent(display, event.window, 0, 0, reinterpret_cast<XEvent*>(&event));
  XFlush(display);
  XCloseDisplay(display);

  thread_.join();
}

void PlatformWindowX11::Show() {
  Display* display = XOpenDisplay(NULL);
  XMapWindow(display, window_);
  XFlush(display);
  XCloseDisplay(display);
}
void PlatformWindowX11::Hide() {
  Display* display = XOpenDisplay(NULL);
  XUnmapWindow(display, window_);
  XFlush(display);
  XCloseDisplay(display);
}

void PlatformWindowX11::Run() {
  XEvent event;
  while (true) {
    XNextEvent(display_, &event);
    switch (event.type) {
      case KeyPress:
      case KeyRelease: {
        std::cerr << "Key press" << std::endl;
      } break;
      case ButtonPress:
      case ButtonRelease: {
        XButtonEvent* x_button_event = reinterpret_cast<XButtonEvent*>(&event);

        // Handle mouse wheel events.
        if (x_button_event->button == 4 || x_button_event->button == 5) {
          PlatformWindowEventData data;
          data.mouse_wheel.angle_in_degrees =
              15 * (x_button_event->button == 4 ? 1 : -1);
          event_callback_(callback_context_,
                          {kPlatformWindowEventTypeMouseWheel, data});
          continue;
        }

        // Okay, normal button click then.
        PlatformWindowEventData data;
        data.mouse_button.pressed = (ButtonPress == event.type);
        data.mouse_button.button = [x_button_event] {
          switch (x_button_event->button) {
            case Button1:
              return kPlatformWindowMouseLeft;
            case Button3:
              return kPlatformWindowMouseRight;
            default:
              return kPlatformWindowMouseUnknown;
          }
        }();
        data.mouse_button.x = x_button_event->x;
        data.mouse_button.y = x_button_event->y;
        event_callback_(callback_context_,
                        {kPlatformWindowEventTypeMouseButton, data});

      } break;
      case MotionNotify: {
        XMotionEvent* x_motion_event = reinterpret_cast<XMotionEvent*>(&event);
        PlatformWindowEventData data;
        data.mouse_move.x = x_motion_event->x;
        data.mouse_move.y = x_motion_event->y;
        event_callback_(callback_context_,
                        {kPlatformWindowEventTypeMouseMove, data});
      } break;
      case ClientMessage: {
        const XClientMessageEvent* client_message =
            reinterpret_cast<const XClientMessageEvent*>(&event);
        if (client_message->message_type == shutdown_atom_) {
          return;
        } else if (event.xclient.data.l[0] == delete_atom_) {
          event_callback_(callback_context_,
                          {kPlatformWindowEventTypeQuitRequest, {}});
        }

      } break;
    }
  }
}

PlatformWindowSize PlatformWindowX11::GetSize() const {
  return {attributes_.width, attributes_.height};
}

}  // namespace

PlatformWindow PlatformWindowMakeDefaultWindow(
    const char* title, PlatformWindowEventCallback event_callback,
    void* context) {
  Display* display = XOpenDisplay(NULL);
  assert(display);

  Window root_window = DefaultRootWindow(display);

  const bool kFullscreen = false;

  XWindowAttributes root_window_attributes;
  XGetWindowAttributes(display, root_window, &root_window_attributes);

  XSetWindowAttributes window_attributes;
  window_attributes.border_pixel = 0;
  window_attributes.event_mask =
      ExposureMask | PointerMotionMask | KeyPressMask | KeyReleaseMask;
  window_attributes.override_redirect = (kFullscreen ? True : False);

  Window window = XCreateWindow(
      display, root_window, 0, 0, root_window_attributes.width,
      root_window_attributes.height, 0, CopyFromParent, InputOutput,
      CopyFromParent,
      CWBorderPixel | CWEventMask | (kFullscreen ? CWOverrideRedirect : 0),
      &window_attributes);

  XSelectInput(display, window,
               VisibilityChangeMask | ExposureMask | FocusChangeMask |
                   StructureNotifyMask | KeyPressMask | KeyReleaseMask |
                   ButtonPressMask | ButtonReleaseMask | PointerMotionMask);

  // Hide the mouse cursor in fullscreen mode.
  if (kFullscreen) {
    XUndefineCursor(display, window);
    // TODO: Actually focus the window or something so that the mouse
    // cursor change takes effect, otherwise you have to actually click.
  }

  Atom delete_atom = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &delete_atom, 1);

  Atom shutdown_atom = XInternAtom(display, "WakeUpAtom", 0);

  XWMHints hints;
  hints.input = True;
  hints.flags = InputHint;
  XSetWMHints(display, window, &hints);

  // make the window visible on the screen
  XStoreName(display, window, title);

  XWindowAttributes attributes;
  XGetWindowAttributes(display, root_window, &attributes);

  return new PlatformWindowX11(event_callback, context, display, window,
                               delete_atom, shutdown_atom, attributes);
}

void PlatformWindowDestroyWindow(PlatformWindow platform_window) {
  delete static_cast<PlatformWindowX11*>(platform_window);
}

NativeWindow PlatformWindowGetNativeWindow(PlatformWindow platform_window) {
  return reinterpret_cast<NativeWindow>(
      static_cast<PlatformWindowX11*>(platform_window)->window());
}

void PlatformWindowShow(PlatformWindow window) {
  static_cast<PlatformWindowX11*>(window)->Show();
}

void PlatformWindowHide(PlatformWindow window) {
  static_cast<PlatformWindowX11*>(window)->Hide();
}

PlatformWindowSize PlatformWindowGetSize(PlatformWindow window) {
  return static_cast<PlatformWindowX11*>(window)->GetSize();
}
