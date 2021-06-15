#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <cassert>
#include <chrono>
#include <iostream>
#include <thread>

#include "platform_window/platform_window.h"

namespace {
PlatformWindowKey XKeyEventToPlatformWindowKey(XKeyEvent* event);
}  // namespace

namespace {
class PlatformWindowX11 {
 public:
  PlatformWindowX11(PlatformWindowEventCallback event_callback,
                    void* callback_context, Display* display, Window window,
                    Atom delete_atom, Atom shutdown_atom);
  ~PlatformWindowX11();

  Window window() const { return window_; }

  void Show();
  void Hide();

  void SetTitle(const char* title);

  PlatformWindowSize GetSize() const;

 private:
  void Run();

  PlatformWindowEventCallback event_callback_;
  void* callback_context_;

  Display* display_;
  Window window_;
  Atom delete_atom_;
  Atom shutdown_atom_;

  std::thread thread_;
};

PlatformWindowX11::PlatformWindowX11(PlatformWindowEventCallback event_callback,
                                     void* callback_context, Display* display,
                                     Window window, Atom delete_atom,
                                     Atom shutdown_atom)
    : event_callback_(event_callback),
      callback_context_(callback_context),
      display_(display),
      window_(window),
      delete_atom_(delete_atom),
      shutdown_atom_(shutdown_atom),
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

void PlatformWindowX11::SetTitle(const char* title) {
  Display* display = XOpenDisplay(NULL);
  XStoreName(display, window_, title);
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
        XKeyEvent* x_key_event = reinterpret_cast<XKeyEvent*>(&event);
        PlatformWindowEventData data;
        data.key.pressed = (event.type == KeyPress);
        data.key.key = XKeyEventToPlatformWindowKey(x_key_event);
        event_callback_(callback_context_, {kPlatformWindowEventTypeKey, data});
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
  Display* display = XOpenDisplay(NULL);
  XWindowAttributes attributes;
  XGetWindowAttributes(display, window_, &attributes);
  XCloseDisplay(display);

  return {attributes.width, attributes.height};
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
      display, root_window, 0, 0, root_window_attributes.width / 2,
      root_window_attributes.height / 2, 0, CopyFromParent, InputOutput,
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

  return new PlatformWindowX11(event_callback, context, display, window,
                               delete_atom, shutdown_atom);
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

void PlatformWindowSetTitle(PlatformWindow window, const char* title) {
  static_cast<PlatformWindowX11*>(window)->SetTitle(title);
}

PlatformWindowSize PlatformWindowGetSize(PlatformWindow window) {
  return static_cast<PlatformWindowX11*>(window)->GetSize();
}

namespace {
// Key translation code adopted from
// https://github.com/youtube/cobalt/blob/master/src/starboard/shared/x11/application_x11.cc.

uint32_t HardwareKeycodeToDefaultXKeysym(uint32_t hardware_code) {
  static const uint32_t kHardwareKeycodeMap[] = {
      0,                // 0x00:
      0,                // 0x01:
      0,                // 0x02:
      0,                // 0x03:
      0,                // 0x04:
      0,                // 0x05:
      0,                // 0x06:
      0,                // 0x07:
      0,                // 0x08:
      XK_Escape,        // 0x09: XK_Escape
      XK_1,             // 0x0A: XK_1
      XK_2,             // 0x0B: XK_2
      XK_3,             // 0x0C: XK_3
      XK_4,             // 0x0D: XK_4
      XK_5,             // 0x0E: XK_5
      XK_6,             // 0x0F: XK_6
      XK_7,             // 0x10: XK_7
      XK_8,             // 0x11: XK_8
      XK_9,             // 0x12: XK_9
      XK_0,             // 0x13: XK_0
      XK_minus,         // 0x14: XK_minus
      XK_equal,         // 0x15: XK_equal
      XK_BackSpace,     // 0x16: XK_BackSpace
      XK_Tab,           // 0x17: XK_Tab
      XK_q,             // 0x18: XK_q
      XK_w,             // 0x19: XK_w
      XK_e,             // 0x1A: XK_e
      XK_r,             // 0x1B: XK_r
      XK_t,             // 0x1C: XK_t
      XK_y,             // 0x1D: XK_y
      XK_u,             // 0x1E: XK_u
      XK_i,             // 0x1F: XK_i
      XK_o,             // 0x20: XK_o
      XK_p,             // 0x21: XK_p
      XK_bracketleft,   // 0x22: XK_bracketleft
      XK_bracketright,  // 0x23: XK_bracketright
      XK_Return,        // 0x24: XK_Return
      XK_Control_L,     // 0x25: XK_Control_L
      XK_a,             // 0x26: XK_a
      XK_s,             // 0x27: XK_s
      XK_d,             // 0x28: XK_d
      XK_f,             // 0x29: XK_f
      XK_g,             // 0x2A: XK_g
      XK_h,             // 0x2B: XK_h
      XK_j,             // 0x2C: XK_j
      XK_k,             // 0x2D: XK_k
      XK_l,             // 0x2E: XK_l
      XK_semicolon,     // 0x2F: XK_semicolon
      XK_apostrophe,    // 0x30: XK_apostrophe
      XK_grave,         // 0x31: XK_grave
      XK_Shift_L,       // 0x32: XK_Shift_L
      XK_backslash,     // 0x33: XK_backslash
      XK_z,             // 0x34: XK_z
      XK_x,             // 0x35: XK_x
      XK_c,             // 0x36: XK_c
      XK_v,             // 0x37: XK_v
      XK_b,             // 0x38: XK_b
      XK_n,             // 0x39: XK_n
      XK_m,             // 0x3A: XK_m
      XK_comma,         // 0x3B: XK_comma
      XK_period,        // 0x3C: XK_period
      XK_slash,         // 0x3D: XK_slash
      XK_Shift_R,       // 0x3E: XK_Shift_R
      0,                // 0x3F: XK_KP_Multiply
      XK_Alt_L,         // 0x40: XK_Alt_L
      XK_space,         // 0x41: XK_space
      XK_Caps_Lock,     // 0x42: XK_Caps_Lock
      XK_F1,            // 0x43: XK_F1
      XK_F2,            // 0x44: XK_F2
      XK_F3,            // 0x45: XK_F3
      XK_F4,            // 0x46: XK_F4
      XK_F5,            // 0x47: XK_F5
      XK_F6,            // 0x48: XK_F6
      XK_F7,            // 0x49: XK_F7
      XK_F8,            // 0x4A: XK_F8
      XK_F9,            // 0x4B: XK_F9
      XK_F10,           // 0x4C: XK_F10
      XK_Num_Lock,      // 0x4D: XK_Num_Lock
      XK_Scroll_Lock,   // 0x4E: XK_Scroll_Lock
  };

  return hardware_code <
                 sizeof(kHardwareKeycodeMap) / sizeof(kHardwareKeycodeMap[0])
             ? kHardwareKeycodeMap[hardware_code]
             : 0;
}

PlatformWindowKey KeysymToPlatformWindowKey(KeySym keysym) {
  switch (keysym) {
    case XK_BackSpace:
      return kPlatformWindowKeyBackspace;
    case XK_Delete:
    case XK_KP_Delete:
      return kPlatformWindowKeyDelete;
    case XK_Tab:
    case XK_KP_Tab:
    case XK_ISO_Left_Tab:
      return kPlatformWindowKeyTab;
    case XK_Linefeed:
    case XK_Return:
    case XK_KP_Enter:
    case XK_ISO_Enter:
      return kPlatformWindowKeyReturn;
    case XK_Clear:
    case XK_KP_Begin:
      return kPlatformWindowKeyClear;
    case XK_KP_Space:
    case XK_space:
      return kPlatformWindowKeySpace;
    case XK_Home:
    case XK_KP_Home:
      return kPlatformWindowKeyHome;
    case XK_End:
    case XK_KP_End:
      return kPlatformWindowKeyEnd;
    case XK_Page_Up:
    case XK_KP_Page_Up:  // aka XK_KP_Prior
      return kPlatformWindowKeyPrior;
    case XK_Page_Down:
    case XK_KP_Page_Down:  // aka XK_KP_Next
      return kPlatformWindowKeyNext;
    case XK_Left:
    case XK_KP_Left:
      return kPlatformWindowKeyLeft;
    case XK_Right:
    case XK_KP_Right:
      return kPlatformWindowKeyRight;
    case XK_Down:
    case XK_KP_Down:
      return kPlatformWindowKeyDown;
    case XK_Up:
    case XK_KP_Up:
      return kPlatformWindowKeyUp;
    case XK_Escape:
      return kPlatformWindowKeyEscape;
    case XK_Kana_Lock:
    case XK_Kana_Shift:
      return kPlatformWindowKeyKana;
    case XK_Hangul:
      return kPlatformWindowKeyHangul;
    case XK_Hangul_Hanja:
      return kPlatformWindowKeyHanja;
    case XK_Kanji:
      return kPlatformWindowKeyKanji;
    case XK_Henkan:
      return kPlatformWindowKeyConvert;
    case XK_Muhenkan:
      return kPlatformWindowKeyNonconvert;
    case XK_Zenkaku_Hankaku:
      return kPlatformWindowKeyDbeDbcschar;
    case XK_A:
    case XK_a:
      return kPlatformWindowKeyA;
    case XK_B:
    case XK_b:
      return kPlatformWindowKeyB;
    case XK_C:
    case XK_c:
      return kPlatformWindowKeyC;
    case XK_D:
    case XK_d:
      return kPlatformWindowKeyD;
    case XK_E:
    case XK_e:
      return kPlatformWindowKeyE;
    case XK_F:
    case XK_f:
      return kPlatformWindowKeyF;
    case XK_G:
    case XK_g:
      return kPlatformWindowKeyG;
    case XK_H:
    case XK_h:
      return kPlatformWindowKeyH;
    case XK_I:
    case XK_i:
      return kPlatformWindowKeyI;
    case XK_J:
    case XK_j:
      return kPlatformWindowKeyJ;
    case XK_K:
    case XK_k:
      return kPlatformWindowKeyK;
    case XK_L:
    case XK_l:
      return kPlatformWindowKeyL;
    case XK_M:
    case XK_m:
      return kPlatformWindowKeyM;
    case XK_N:
    case XK_n:
      return kPlatformWindowKeyN;
    case XK_O:
    case XK_o:
      return kPlatformWindowKeyO;
    case XK_P:
    case XK_p:
      return kPlatformWindowKeyP;
    case XK_Q:
    case XK_q:
      return kPlatformWindowKeyQ;
    case XK_R:
    case XK_r:
      return kPlatformWindowKeyR;
    case XK_S:
    case XK_s:
      return kPlatformWindowKeyS;
    case XK_T:
    case XK_t:
      return kPlatformWindowKeyT;
    case XK_U:
    case XK_u:
      return kPlatformWindowKeyU;
    case XK_V:
    case XK_v:
      return kPlatformWindowKeyV;
    case XK_W:
    case XK_w:
      return kPlatformWindowKeyW;
    case XK_X:
    case XK_x:
      return kPlatformWindowKeyX;
    case XK_Y:
    case XK_y:
      return kPlatformWindowKeyY;
    case XK_Z:
    case XK_z:
      return kPlatformWindowKeyZ;

    case XK_0:
    case XK_1:
    case XK_2:
    case XK_3:
    case XK_4:
    case XK_5:
    case XK_6:
    case XK_7:
    case XK_8:
    case XK_9:
      return static_cast<PlatformWindowKey>(kPlatformWindowKey0 +
                                            (keysym - XK_0));

    case XK_parenright:
      return kPlatformWindowKey0;
    case XK_exclam:
      return kPlatformWindowKey1;
    case XK_at:
      return kPlatformWindowKey2;
    case XK_numbersign:
      return kPlatformWindowKey3;
    case XK_dollar:
      return kPlatformWindowKey4;
    case XK_percent:
      return kPlatformWindowKey5;
    case XK_asciicircum:
      return kPlatformWindowKey6;
    case XK_ampersand:
      return kPlatformWindowKey7;
    case XK_asterisk:
      return kPlatformWindowKey8;
    case XK_parenleft:
      return kPlatformWindowKey9;

    case XK_KP_0:
    case XK_KP_1:
    case XK_KP_2:
    case XK_KP_3:
    case XK_KP_4:
    case XK_KP_5:
    case XK_KP_6:
    case XK_KP_7:
    case XK_KP_8:
    case XK_KP_9:
      return static_cast<PlatformWindowKey>(kPlatformWindowKeyNumpad0 +
                                            (keysym - XK_KP_0));

    case XK_multiply:
    case XK_KP_Multiply:
      return kPlatformWindowKeyMultiply;
    case XK_KP_Add:
      return kPlatformWindowKeyAdd;
    case XK_KP_Separator:
      return kPlatformWindowKeySeparator;
    case XK_KP_Subtract:
      return kPlatformWindowKeySubtract;
    case XK_KP_Decimal:
      return kPlatformWindowKeyDecimal;
    case XK_KP_Divide:
      return kPlatformWindowKeyDivide;
    case XK_KP_Equal:
    case XK_equal:
    case XK_plus:
      return kPlatformWindowKeyOemPlus;
    case XK_comma:
    case XK_less:
      return kPlatformWindowKeyOemComma;
    case XK_minus:
    case XK_underscore:
      return kPlatformWindowKeyOemMinus;
    case XK_greater:
    case XK_period:
      return kPlatformWindowKeyOemPeriod;
    case XK_colon:
    case XK_semicolon:
      return kPlatformWindowKeyOem1;
    case XK_question:
    case XK_slash:
      return kPlatformWindowKeyOem2;
    case XK_asciitilde:
    case XK_quoteleft:
      return kPlatformWindowKeyOem3;
    case XK_bracketleft:
    case XK_braceleft:
      return kPlatformWindowKeyOem4;
    case XK_backslash:
    case XK_bar:
      return kPlatformWindowKeyOem5;
    case XK_bracketright:
    case XK_braceright:
      return kPlatformWindowKeyOem6;
    case XK_quoteright:
    case XK_quotedbl:
      return kPlatformWindowKeyOem7;
    case XK_Shift_L:
    case XK_Shift_R:
      return kPlatformWindowKeyShift;
    case XK_Control_L:
    case XK_Control_R:
      return kPlatformWindowKeyControl;
    case XK_Meta_L:
    case XK_Meta_R:
    case XK_Alt_L:
    case XK_Alt_R:
      return kPlatformWindowKeyMenu;
    case XK_Pause:
      return kPlatformWindowKeyPause;
    case XK_Caps_Lock:
      return kPlatformWindowKeyCapital;
    case XK_Num_Lock:
      return kPlatformWindowKeyNumlock;
    case XK_Scroll_Lock:
      return kPlatformWindowKeyScroll;
    case XK_Select:
      return kPlatformWindowKeySelect;
    case XK_Print:
      return kPlatformWindowKeyPrint;
    case XK_Execute:
      return kPlatformWindowKeyExecute;
    case XK_Insert:
    case XK_KP_Insert:
      return kPlatformWindowKeyInsert;
    case XK_Help:
      return kPlatformWindowKeyHelp;
    case XK_Super_L:
      return kPlatformWindowKeyLwin;
    case XK_Super_R:
      return kPlatformWindowKeyRwin;
    case XK_Menu:
      return kPlatformWindowKeyApps;
    case XK_F1:
    case XK_F2:
    case XK_F3:
    case XK_F4:
    case XK_F5:
    case XK_F6:
    case XK_F7:
    case XK_F8:
    case XK_F9:
    case XK_F10:
    case XK_F11:
    case XK_F12:
    case XK_F13:
    case XK_F14:
    case XK_F15:
    case XK_F16:
    case XK_F17:
    case XK_F18:
    case XK_F19:
    case XK_F20:
    case XK_F21:
    case XK_F22:
    case XK_F23:
    case XK_F24:
      return static_cast<PlatformWindowKey>(kPlatformWindowKeyF1 +
                                            (keysym - XK_F1));
    case XK_KP_F1:
    case XK_KP_F2:
    case XK_KP_F3:
    case XK_KP_F4:
      return static_cast<PlatformWindowKey>(kPlatformWindowKeyF1 +
                                            (keysym - XK_KP_F1));
  }
  return kPlatformWindowKeyUnknown;
}

// Get a PlatformWindowKey from an XKeyEvent.
PlatformWindowKey XKeyEventToPlatformWindowKey(XKeyEvent* event) {
  // XLookupKeysym does not take into consideration the state of the
  // lock/shift etc. keys. So it is necessary to use XLookupString instead.
  KeySym keysym = XK_VoidSymbol;
  XLookupString(event, NULL, 0, &keysym, NULL);
  PlatformWindowKey key = KeysymToPlatformWindowKey(keysym);
  if (key == kPlatformWindowKeyUnknown) {
    keysym = HardwareKeycodeToDefaultXKeysym(event->keycode);
    key = KeysymToPlatformWindowKey(keysym);
  }

  return key;
}
}  // namespace
