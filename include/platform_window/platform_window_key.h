#ifndef _PLATFORM_WINDOW_KEY_CODES_H_
#define _PLATFORM_WINDOW_KEY_CODES_H_

#ifdef __cplusplus
extern "C" {
#endif

// List adopted from
// https://github.com/youtube/cobalt/blob/master/src/starboard/key.h
// Key codes for key events, adopted from Windows virtual key codes:
//   https://msdn.microsoft.com/en-us/library/windows/desktop/dd375731%28v=vs.85%29.aspx
typedef enum PlatformWindowKey {
  kPlatformWindowKeyUnknown = 0,
  kPlatformWindowKeyBackspace = 0x08,
  kPlatformWindowKeyTab = 0x09,
  kPlatformWindowKeyBacktab = 0x0A,
  kPlatformWindowKeyClear = 0x0C,
  kPlatformWindowKeyReturn = 0x0D,  // Return/Enter key
  kPlatformWindowKeyShift = 0x10,
  kPlatformWindowKeyControl = 0x11,
  kPlatformWindowKeyMenu = 0x12,  // Alt key
  kPlatformWindowKeyPause = 0x13,
  kPlatformWindowKeyCapital = 0x14,
  kPlatformWindowKeyKana = 0x15,
  kPlatformWindowKeyHangul = 0x15,
  kPlatformWindowKeyJunja = 0x17,
  kPlatformWindowKeyFinal = 0x18,
  kPlatformWindowKeyHanja = 0x19,
  kPlatformWindowKeyKanji = 0x19,
  kPlatformWindowKeyEscape = 0x1B,
  kPlatformWindowKeyConvert = 0x1C,
  kPlatformWindowKeyNonconvert = 0x1D,
  kPlatformWindowKeyAccept = 0x1E,
  kPlatformWindowKeyModechange = 0x1F,
  kPlatformWindowKeySpace = 0x20,
  kPlatformWindowKeyPrior = 0x21,
  kPlatformWindowKeyNext = 0x22,
  kPlatformWindowKeyEnd = 0x23,
  kPlatformWindowKeyHome = 0x24,
  kPlatformWindowKeyLeft = 0x25,
  kPlatformWindowKeyUp = 0x26,
  kPlatformWindowKeyRight = 0x27,
  kPlatformWindowKeyDown = 0x28,
  kPlatformWindowKeySelect = 0x29,
  kPlatformWindowKeyPrint = 0x2A,
  kPlatformWindowKeyExecute = 0x2B,
  kPlatformWindowKeySnapshot = 0x2C,
  kPlatformWindowKeyInsert = 0x2D,
  kPlatformWindowKeyDelete = 0x2E,
  kPlatformWindowKeyHelp = 0x2F,
  kPlatformWindowKey0 = 0x30,
  kPlatformWindowKey1 = 0x31,
  kPlatformWindowKey2 = 0x32,
  kPlatformWindowKey3 = 0x33,
  kPlatformWindowKey4 = 0x34,
  kPlatformWindowKey5 = 0x35,
  kPlatformWindowKey6 = 0x36,
  kPlatformWindowKey7 = 0x37,
  kPlatformWindowKey8 = 0x38,
  kPlatformWindowKey9 = 0x39,
  kPlatformWindowKeyA = 0x41,
  kPlatformWindowKeyB = 0x42,
  kPlatformWindowKeyC = 0x43,
  kPlatformWindowKeyD = 0x44,
  kPlatformWindowKeyE = 0x45,
  kPlatformWindowKeyF = 0x46,
  kPlatformWindowKeyG = 0x47,
  kPlatformWindowKeyH = 0x48,
  kPlatformWindowKeyI = 0x49,
  kPlatformWindowKeyJ = 0x4A,
  kPlatformWindowKeyK = 0x4B,
  kPlatformWindowKeyL = 0x4C,
  kPlatformWindowKeyM = 0x4D,
  kPlatformWindowKeyN = 0x4E,
  kPlatformWindowKeyO = 0x4F,
  kPlatformWindowKeyP = 0x50,
  kPlatformWindowKeyQ = 0x51,
  kPlatformWindowKeyR = 0x52,
  kPlatformWindowKeyS = 0x53,
  kPlatformWindowKeyT = 0x54,
  kPlatformWindowKeyU = 0x55,
  kPlatformWindowKeyV = 0x56,
  kPlatformWindowKeyW = 0x57,
  kPlatformWindowKeyX = 0x58,
  kPlatformWindowKeyY = 0x59,
  kPlatformWindowKeyZ = 0x5A,
  kPlatformWindowKeyLwin = 0x5B,
  kPlatformWindowKeyCommand =
      kPlatformWindowKeyLwin,  // Provide the Mac name for convenience.
  kPlatformWindowKeyRwin = 0x5C,
  kPlatformWindowKeyApps = 0x5D,
  kPlatformWindowKeySleep = 0x5F,
  kPlatformWindowKeyNumpad0 = 0x60,
  kPlatformWindowKeyNumpad1 = 0x61,
  kPlatformWindowKeyNumpad2 = 0x62,
  kPlatformWindowKeyNumpad3 = 0x63,
  kPlatformWindowKeyNumpad4 = 0x64,
  kPlatformWindowKeyNumpad5 = 0x65,
  kPlatformWindowKeyNumpad6 = 0x66,
  kPlatformWindowKeyNumpad7 = 0x67,
  kPlatformWindowKeyNumpad8 = 0x68,
  kPlatformWindowKeyNumpad9 = 0x69,
  kPlatformWindowKeyMultiply = 0x6A,
  kPlatformWindowKeyAdd = 0x6B,
  kPlatformWindowKeySeparator = 0x6C,
  kPlatformWindowKeySubtract = 0x6D,
  kPlatformWindowKeyDecimal = 0x6E,
  kPlatformWindowKeyDivide = 0x6F,
  kPlatformWindowKeyF1 = 0x70,
  kPlatformWindowKeyF2 = 0x71,
  kPlatformWindowKeyF3 = 0x72,
  kPlatformWindowKeyF4 = 0x73,
  kPlatformWindowKeyF5 = 0x74,
  kPlatformWindowKeyF6 = 0x75,
  kPlatformWindowKeyF7 = 0x76,
  kPlatformWindowKeyF8 = 0x77,
  kPlatformWindowKeyF9 = 0x78,
  kPlatformWindowKeyF10 = 0x79,
  kPlatformWindowKeyF11 = 0x7A,
  kPlatformWindowKeyF12 = 0x7B,
  kPlatformWindowKeyF13 = 0x7C,
  kPlatformWindowKeyF14 = 0x7D,
  kPlatformWindowKeyF15 = 0x7E,
  kPlatformWindowKeyF16 = 0x7F,
  kPlatformWindowKeyF17 = 0x80,
  kPlatformWindowKeyF18 = 0x81,
  kPlatformWindowKeyF19 = 0x82,
  kPlatformWindowKeyF20 = 0x83,
  kPlatformWindowKeyF21 = 0x84,
  kPlatformWindowKeyF22 = 0x85,
  kPlatformWindowKeyF23 = 0x86,
  kPlatformWindowKeyF24 = 0x87,
  kPlatformWindowKeyNumlock = 0x90,
  kPlatformWindowKeyScroll = 0x91,
  kPlatformWindowKeyWlan = 0x97,
  kPlatformWindowKeyPower = 0x98,
  kPlatformWindowKeyBrowserBack = 0xA6,
  kPlatformWindowKeyBrowserForward = 0xA7,
  kPlatformWindowKeyBrowserRefresh = 0xA8,
  kPlatformWindowKeyBrowserStop = 0xA9,
  kPlatformWindowKeyBrowserSearch = 0xAA,
  kPlatformWindowKeyBrowserFavorites = 0xAB,
  kPlatformWindowKeyBrowserHome = 0xAC,
  kPlatformWindowKeyVolumeMute = 0xAD,
  kPlatformWindowKeyVolumeDown = 0xAE,
  kPlatformWindowKeyVolumeUp = 0xAF,
  kPlatformWindowKeyMediaNextTrack = 0xB0,
  kPlatformWindowKeyMediaPrevTrack = 0xB1,
  kPlatformWindowKeyMediaStop = 0xB2,
  kPlatformWindowKeyMediaPlayPause = 0xB3,
  kPlatformWindowKeyMediaLaunchMail = 0xB4,
  kPlatformWindowKeyMediaLaunchMediaSelect = 0xB5,
  kPlatformWindowKeyMediaLaunchApp1 = 0xB6,
  kPlatformWindowKeyMediaLaunchApp2 = 0xB7,
  kPlatformWindowKeyOem1 = 0xBA,
  kPlatformWindowKeyOemPlus = 0xBB,
  kPlatformWindowKeyOemComma = 0xBC,
  kPlatformWindowKeyOemMinus = 0xBD,
  kPlatformWindowKeyOemPeriod = 0xBE,
  kPlatformWindowKeyOem2 = 0xBF,
  kPlatformWindowKeyOem3 = 0xC0,
  kPlatformWindowKeyBrightnessDown = 0xD8,
  kPlatformWindowKeyBrightnessUp = 0xD9,
  kPlatformWindowKeyKbdBrightnessDown = 0xDA,
  kPlatformWindowKeyOem4 = 0xDB,
  kPlatformWindowKeyOem5 = 0xDC,
  kPlatformWindowKeyOem6 = 0xDD,
  kPlatformWindowKeyOem7 = 0xDE,
  kPlatformWindowKeyOem8 = 0xDF,
  kPlatformWindowKeyOem102 = 0xE2,
  kPlatformWindowKeyKbdBrightnessUp = 0xE8,
  kPlatformWindowKeyDbeSbcschar = 0xF3,
  kPlatformWindowKeyDbeDbcschar = 0xF4,
  kPlatformWindowKeyPlay = 0xFA,

  // Beyond this point are non-Windows key codes that are provided as
  // extensions, as they otherwise have no analogs.

  // Other supported CEA 2014 keys.
  kPlatformWindowKeyMediaRewind = 0xE3,
  kPlatformWindowKeyMediaFastForward = 0xE4,

  // Key codes from the DTV Application Software Environment,
  //   http://www.atsc.org/wp-content/uploads/2015/03/a_100_4.pdf
  kPlatformWindowKeyRed = 0x193,
  kPlatformWindowKeyGreen = 0x194,
  kPlatformWindowKeyYellow = 0x195,
  kPlatformWindowKeyBlue = 0x196,

  kPlatformWindowKeyChannelUp = 0x1AB,
  kPlatformWindowKeyChannelDown = 0x1AC,
  kPlatformWindowKeySubtitle = 0x1CC,
  kPlatformWindowKeyClosedCaption = kPlatformWindowKeySubtitle,

  kPlatformWindowKeyInfo = 0x1C9,
  kPlatformWindowKeyGuide = 0x1CA,

  // Key codes from OCAP,
  //   https://apps.cablelabs.com/specification/opencable-application-platform-ocap/
  kPlatformWindowKeyLast = 0x25f,
  kPlatformWindowKeyPreviousChannel = kPlatformWindowKeyLast,

  // Also from OCAP
  kPlatformWindowKeyInstantReplay = 0x273,

  // Custom Starboard-defined keycodes.

  // A button that will directly launch the current application.
  kPlatformWindowKeyLaunchThisApplication = 0x3000,

  // A button that will switch between different available audio tracks.
  kPlatformWindowKeyMediaAudioTrack = 0x3001,

  // A button that will trigger voice input.
  kPlatformWindowKeyMicrophone = 0x3002,

  // Mouse buttons, starting with the left mouse button.
  kPlatformWindowKeyMouse1 = 0x7000,
  kPlatformWindowKeyMouse2 = 0x7001,
  kPlatformWindowKeyMouse3 = 0x7002,
  kPlatformWindowKeyMouse4 = 0x7003,
  kPlatformWindowKeyMouse5 = 0x7004,

  // Gamepad extensions to windows virtual key codes.
  // http://www.w3.org/TR/gamepad/

  // Xbox A, PS O or X (depending on region), Switch A
  kPlatformWindowKeyGamepad1 = 0x8000,

  // Xbox B, PS X or O (depending on region), Switch B
  kPlatformWindowKeyGamepad2 = 0x8001,

  // Xbox X, PS square, Switch X
  kPlatformWindowKeyGamepad3 = 0x8002,

  // Xbox Y, PS triangle, Switch Y
  kPlatformWindowKeyGamepad4 = 0x8003,

  // Pretty much every gamepad has bumpers at the top front of the controller,
  // and triggers at the bottom front of the controller.
  kPlatformWindowKeyGamepadLeftBumper = 0x8004,
  kPlatformWindowKeyGamepadRightBumper = 0x8005,
  kPlatformWindowKeyGamepadLeftTrigger = 0x8006,
  kPlatformWindowKeyGamepadRightTrigger = 0x8007,

  // Xbox 360 Back, XB1 minimize, PS and WiiU Select
  kPlatformWindowKeyGamepad5 = 0x8008,

  // Xbox 360 Play, XB1 Menu, PS and WiiU Start
  kPlatformWindowKeyGamepad6 = 0x8009,

  // This refers to pressing the left stick like a button.
  kPlatformWindowKeyGamepadLeftStick = 0x800A,

  // This refers to pressing the right stick like a button.
  kPlatformWindowKeyGamepadRightStick = 0x800B,

  kPlatformWindowKeyGamepadDPadUp = 0x800C,
  kPlatformWindowKeyGamepadDPadDown = 0x800D,
  kPlatformWindowKeyGamepadDPadLeft = 0x800E,
  kPlatformWindowKeyGamepadDPadRight = 0x800F,

  // The system key in the middle of the gamepad, if it exists.
  kPlatformWindowKeyGamepadSystem = 0x8010,

  // Codes for thumbstick to virtual dpad conversions.
  kPlatformWindowKeyGamepadLeftStickUp = 0x8011,
  kPlatformWindowKeyGamepadLeftStickDown = 0x8012,
  kPlatformWindowKeyGamepadLeftStickLeft = 0x8013,
  kPlatformWindowKeyGamepadLeftStickRight = 0x8014,
  kPlatformWindowKeyGamepadRightStickUp = 0x8015,
  kPlatformWindowKeyGamepadRightStickDown = 0x8016,
  kPlatformWindowKeyGamepadRightStickLeft = 0x8017,
  kPlatformWindowKeyGamepadRightStickRight = 0x8018,
} PlatformWindowKey;

#ifdef __cplusplus
}
#endif

#endif  // #ifndef _PLATFORM_WINDOW_KEY_CODES_H_
