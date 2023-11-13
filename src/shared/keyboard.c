#include <share-a-keyb/keyboard.h>
#include <stdio.h>

//// CLIENT

#ifndef WIN32 // Linux Definitions
const static uint8_t scancodes_to_enable[] = {KEY_ESC,
                                              KEY_F1,
                                              KEY_PAUSE,
                                              KEY_1,
                                              KEY_F2,
                                              KEY_KPCOMMA,
                                              KEY_2,
                                              KEY_F3,
                                              KEY_HANGEUL,
                                              KEY_3,
                                              KEY_F4,
                                              KEY_HANJA,
                                              KEY_4,
                                              KEY_F5,
                                              KEY_YEN,
                                              KEY_5,
                                              KEY_F6,
                                              KEY_LEFTMETA,
                                              KEY_6,
                                              KEY_F7,
                                              KEY_RIGHTMETA,
                                              KEY_7,
                                              KEY_F8,
                                              KEY_COMPOSE,
                                              KEY_8,
                                              KEY_F9,
                                              KEY_STOP,
                                              KEY_9,
                                              KEY_F10,
                                              KEY_AGAIN,
                                              KEY_0,
                                              KEY_NUMLOCK,
                                              KEY_PROPS,
                                              KEY_MINUS,
                                              KEY_SCROLLLOCK,
                                              KEY_UNDO,
                                              KEY_EQUAL,
                                              KEY_KP7,
                                              KEY_FRONT,
                                              KEY_BACKSPACE,
                                              KEY_KP8,
                                              KEY_COPY,
                                              KEY_TAB,
                                              KEY_KP9,
                                              KEY_OPEN,
                                              KEY_Q,
                                              KEY_KPMINUS,
                                              KEY_PASTE,
                                              KEY_W,
                                              KEY_KP4,
                                              KEY_FIND,
                                              KEY_E,
                                              KEY_KP5,
                                              KEY_CUT,
                                              KEY_R,
                                              KEY_KP6,
                                              KEY_HELP,
                                              KEY_T,
                                              KEY_KPPLUS,
                                              KEY_CALC,
                                              KEY_Y,
                                              KEY_KP1,
                                              KEY_SLEEP,
                                              KEY_U,
                                              KEY_KP2,
                                              KEY_WAKEUP,
                                              KEY_I,
                                              KEY_KP3,
                                              KEY_MAIL,
                                              KEY_O,
                                              KEY_KP0,
                                              KEY_BOOKMARKS,
                                              KEY_P,
                                              KEY_KPDOT,
                                              KEY_COMPUTER,
                                              KEY_LEFTBRACE,
                                              KEY_ZENKAKUHANKAKU,
                                              KEY_WWW,
                                              KEY_RIGHTBRACE,
                                              KEY_102ND,
                                              KEY_COFFEE,
                                              KEY_ENTER,
                                              KEY_F11,
                                              KEY_BACK,
                                              KEY_LEFTCTRL,
                                              KEY_F12,
                                              KEY_FORWARD,
                                              KEY_A,
                                              KEY_RO,
                                              KEY_EJECTCD,
                                              KEY_S,
                                              KEY_KATAKANA,
                                              KEY_NEXTSONG,
                                              KEY_D,
                                              KEY_HIRAGANA,
                                              KEY_PLAYPAUSE,
                                              KEY_F,
                                              KEY_HENKAN,
                                              KEY_PREVIOUSSONG,
                                              KEY_G,
                                              KEY_KATAKANAHIRAGANA,
                                              KEY_STOPCD,
                                              KEY_H,
                                              KEY_MUHENKAN,
                                              KEY_HOMEPAGE,
                                              KEY_J,
                                              KEY_KPJPCOMMA,
                                              KEY_REFRESH,
                                              KEY_K,
                                              KEY_KPENTER,
                                              KEY_EDIT,
                                              KEY_L,
                                              KEY_RIGHTCTRL,
                                              KEY_SCROLLUP,
                                              KEY_SEMICOLON,
                                              KEY_KPSLASH,
                                              KEY_SCROLLDOWN,
                                              KEY_APOSTROPHE,
                                              KEY_SYSRQ,
                                              KEY_KPLEFTPAREN,
                                              KEY_GRAVE,
                                              KEY_RIGHTALT,
                                              KEY_KPRIGHTPAREN,
                                              KEY_LEFTSHIFT,
                                              KEY_HOME,
                                              KEY_F13,
                                              KEY_BACKSLASH,
                                              KEY_UP,
                                              KEY_F14,
                                              KEY_Z,
                                              KEY_PAGEUP,
                                              KEY_F15,
                                              KEY_X,
                                              KEY_LEFT,
                                              KEY_F16,
                                              KEY_C,
                                              KEY_RIGHT,
                                              KEY_F17,
                                              KEY_V,
                                              KEY_END,
                                              KEY_F18,
                                              KEY_B,
                                              KEY_DOWN,
                                              KEY_F19,
                                              KEY_N,
                                              KEY_PAGEDOWN,
                                              KEY_F20,
                                              KEY_M,
                                              KEY_INSERT,
                                              KEY_F21,
                                              KEY_COMMA,
                                              KEY_DELETE,
                                              KEY_F22,
                                              KEY_DOT,
                                              KEY_MACRO,
                                              KEY_F23,
                                              KEY_SLASH,
                                              KEY_MUTE,
                                              KEY_F24,
                                              KEY_RIGHTSHIFT,
                                              KEY_VOLUMEDOWN,
                                              KEY_SEARCH,
                                              KEY_KPASTERISK,
                                              KEY_VOLUMEUP,
                                              KEY_MEDIA,
                                              KEY_LEFTALT,
                                              KEY_POWER,
                                              KEY_UNKNOWN,
                                              KEY_SPACE,
                                              KEY_KPEQUAL,
                                              KEY_CAPSLOCK,
                                              KEY_KPPLUSMINUS};
const static uint32_t scancodes_cnt = 172;

struct keyboard_event_writer {
  int fd;
};

void emit(int fd, int type, int code, int val) {
  struct input_event ie;

  ie.type = type;
  ie.code = code;
  ie.value = val;
  /* timestamp values below are ignored */
  ie.time.tv_sec = 0;
  ie.time.tv_usec = 0;

  write(fd, &ie, sizeof(ie));
}

#else
struct keyboard_event_writer {
  uint8_t unused;
};

#endif

keyboard_event_writer_t *init_keyboard_event_writer() {
#ifndef WIN32
  keyboard_event_writer_t *keybdev = malloc(sizeof(keyboard_event_writer_t));
  memset(keybdev, 0, sizeof(keyboard_event_writer_t));

  keybdev->fd = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
  if (keybdev->fd < 0) {
    printf("Couldn't create uinput device. You may need to run as root.\n");
    return NULL;
  }
  ioctl(keybdev->fd, UI_SET_EVBIT, EV_KEY);
  for (int i = 0; i < scancodes_cnt; i++) {
    ioctl(keybdev->fd, UI_SET_KEYBIT, scancodes_to_enable[i]);
  }

  struct uinput_setup usetup;
  memset(&usetup, 0, sizeof(usetup));
  usetup.id.bustype = BUS_USB;
  usetup.id.vendor = 0x1234;  /* sample vendor */
  usetup.id.product = 0x5678; /* sample product */
  strcpy(usetup.name, "sharedkeyb");

  ioctl(keybdev->fd, UI_DEV_SETUP, &usetup);
  ioctl(keybdev->fd, UI_DEV_CREATE);

  return keybdev;
#else
  keyboard_event_writer_t *keybdev = malloc(sizeof(keyboard_event_writer_t));
  memset(keybdev, 0, sizeof(keyboard_event_writer_t));
  return keybdev;
#endif
}

int write_keyboard_input(keyboard_event_writer_t *keybdev,
                         key_input_info_t input_info) {
#ifndef WIN32
  emit(keybdev->fd, EV_KEY, input_info.scancode, input_info.eventtype);
  emit(keybdev->fd, EV_SYN, SYN_REPORT, 0);
#else
  INPUT siminput = {0};
  const uint8_t strokes = 1;
  siminput.type = INPUT_KEYBOARD;
  siminput.ki.dwFlags = KEYEVENTF_SCANCODE;
  siminput.ki.wScan = input_info.scancode;
  if (input_info.eventtype == 0) {
    siminput.ki.dwFlags |= KEYEVENTF_KEYUP;
  }

  uint32_t uSent = SendInput(strokes, &siminput, sizeof(INPUT));
  if (uSent != strokes) {
    printf("For some reason didn't send the input\n");
    return 1;
  }
#endif
  return 0;
}

//// SERVER

#ifndef WIN32 // Linux Definitions
struct key_input_device_event {
  struct timeval time;
  unsigned short evpart;
  unsigned short unused;
  unsigned int value;
};
typedef struct key_input_device_event key_input_device_event_t;

struct keyboard_event_reader {
  int event_file_desc;
  key_input_device_event_t keyinputs[3];
};

const static size_t keyinputs_size = sizeof(key_input_device_event_t) * 3;

#else // Windows Definitions
struct keyboard_event_reader {
  HINSTANCE hInstance;
  WNDCLASS wdc;
  HWND hwnd;
  bool keyspressed[256];
};

const static char CLASS_NAME[] = "keybliker";

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam,
                            LPARAM lParam) {
  switch (uMsg) {
  case WM_DESTROY:
    PostQuitMessage(0);
    return 0;
  case WM_PAINT: {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(hwnd, &ps);

    // All painting occurs here, between BeginPaint and EndPaint.

    FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 2));

    EndPaint(hwnd, &ps);
  }
    return 0;
  }
  return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
#endif

keyboard_event_reader_t *init_keyboard_event_reader() {
#ifndef WIN32
  // check which input device is a keyboard
  FILE *devicesfile = fopen("/proc/bus/input/devices", "r");
  if (devicesfile == NULL) {
    printf("Device list cannot be accessed\n");
    return NULL;
  }

  // ready the event filename
  const char *evfnamebase = "/dev/input/";
  const size_t evfnamebaselen = strlen(evfnamebase);
  char eventfilename[64] = {0};
  strcpy(eventfilename, evfnamebase);
  char *eventname = eventfilename + evfnamebaselen;

  // go through each line checking for a reference to the keyboard
  char devslinebuff[512] = {0};
  size_t devslinebuffsize = 512;
  char *linebuff = devslinebuff;
  bool found = false;
  while (getline(&linebuff, &devslinebuffsize, devicesfile)) {
    // if we find a matching line, we take out it's last part
    // because it's the event's filename
    const char *comparison = "H: Handlers=sysrq kbd ";
    const size_t comparison_len = strlen(comparison);
    if (strncmp(linebuff, comparison, comparison_len) == 0) {
      char junk[64];
      sscanf(linebuff, "H: Handlers=sysrq kbd %s %s", junk, eventname);
      found = true;
      break;
    }
  }
  if (!found) {
    printf("Couldn't find a keyboard device.\n");
    return NULL;
  }

  fclose(devicesfile);

  // printf("KeyEvent Filename: %s\n", eventfilename);
  // "/dev/input/event3" for example

  // Load the found event file, we'll fetch our key events from there
  int event_file_desc = open(eventfilename, O_RDONLY);
  if (event_file_desc < 0) {
    printf("Keyboard device cannot be accessed; You may need to run as root\n");
    return NULL;
  }

  // Return a new keyboard event with the file descriptor for our keyboard event
  keyboard_event_reader_t *keybdev = malloc(sizeof(keyboard_event_reader_t));
  memset(keybdev, 0, sizeof(keyboard_event_reader_t));
  keybdev->event_file_desc = event_file_desc;

  return keybdev;

#else
  keyboard_event_reader_t *keyb = malloc(sizeof(keyboard_event_reader_t));
  memset(keyb, 0, sizeof(keyboard_event_reader_t));

  // On windows we need a window to process keyboard events, apparently - so
  // we're going to create one ^^ Now, because we aren't starting the programme
  // in a WinAPI main function, we need to do a couple non-con things here but
  // overall this is going to be a fairly standard window initiation
  keyb->hInstance = GetModuleHandle(NULL);

  keyb->wdc.lpfnWndProc = WindowProc;
  keyb->wdc.hInstance = keyb->hInstance;
  keyb->wdc.lpszClassName = CLASS_NAME;

  RegisterClass(&keyb->wdc);

  keyb->hwnd =
      CreateWindowEx(0,                     // Optional window styles.
                     CLASS_NAME,            // Window class
                     "share-a-keyb server", // Window text
                     WS_OVERLAPPEDWINDOW,   // Window style

                     // Size and position
                     CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

                     NULL,            // Parent window
                     NULL,            // Menu
                     keyb->hInstance, // Instance handle
                     NULL             // Additional application data
      );

  if (keyb->hwnd == NULL) {
    return NULL;
  }

  ShowWindow(keyb->hwnd, 1);

  // Return the newly created keyboard event
  return keyb;
#endif
}

int read_keyboard_input(keyboard_event_reader_t *keybdev,
                        key_input_info_t *input_info) {
#ifndef WIN32
  int recvd;
  if ((recvd = read(keybdev->event_file_desc, keybdev->keyinputs,
                    keyinputs_size)) > 0) {
    // Whenever a key event happens, 3 writes for this structure occur.
    // First struct [evpart: 4] contains the Scan Code [value]
    // Second struct [evpart: 1] contains the Event Type [value: 0,1,2]
    // Third struct [evpart: 0] contains nothing, simply signals the end

    // printf("\nScanCode: [%d] Evpart: [%d]\n", keybdev->keyinputs[0].value,
    //        keybdev->keyinputs[0].evpart);
    // printf("EvType: [%d] Evpart: [%d]\n", keybdev->keyinputs[1].value,
    //        keybdev->keyinputs[1].evpart);
    // printf("Zero: [%d] Evpart: [%d]\n", keybdev->keyinputs[2].value,
    //        keybdev->keyinputs[2].evpart);

    // A sanity check in this case would simply mean checking if all those
    // circumstances are met:
    bool sanity_check =
        (((keybdev->keyinputs[0].evpart == 4) && // First read is the Scan Code
          (keybdev->keyinputs[1].evpart ==
           1) && // Second read is the Event Type
          (keybdev->keyinputs[2].evpart == 0)) && // Third read is the Finish
                                                  // Event Type is either
         ((keybdev->keyinputs[1].value == 0) ||   // KeyUp
          (keybdev->keyinputs[1].value == 1) ||   // KeyDown
          (keybdev->keyinputs[1].value == 2)));   // KeyHold

    // I really don't want to mess with what could possibly happen for that to
    // fail, so we just exit the programme.
    if (!sanity_check) {
      printf("Sanity check somehow went wrong.\n");
      return -1;
    }

    // Put parsed values to use
    input_info->eventtype = keybdev->keyinputs[1].value;
    input_info->scancode = keybdev->keyinputs[0].value;

    return 0;
  } else {
    printf("Error reading keyboard input!\n");
    return -2;
  }

#else
  MSG msg = {0};
  uint32_t vk;
  if (GetMessage(&msg, NULL, 0, 0) > 0) {
    TranslateMessage(&msg);
    DispatchMessage(&msg);

    vk = msg.wParam;

    switch (msg.message) {
    case WM_KEYDOWN: {
      if (keybdev->keyspressed[vk]) {
        input_info->eventtype = 2;
      } else {
        input_info->eventtype = 1;
        keybdev->keyspressed[vk] = true;
      }
    } break;
    case WM_KEYUP: {
      input_info->eventtype = 0;
      keybdev->keyspressed[vk] = false;
    } break;
    default:
      return -2;
    }
  } else {
    exit(1);
    return -1;
  }

  uint32_t sc = MapVirtualKeyA(vk, MAPVK_VK_TO_VSC);
  input_info->scancode = sc;

  return 0;
#endif
}