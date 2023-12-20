#include <share-a-keyb/keyboard.h>
#include <stdio.h>

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


keyboard_event_writer_t *init_keyboard_event_writer() {
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
}

int write_keyboard_input(keyboard_event_writer_t *keybdev,
                         key_input_info_t input_info) {
  emit(keybdev->fd, EV_KEY, input_info.scancode, input_info.eventtype);
  emit(keybdev->fd, EV_SYN, SYN_REPORT, 0);
  return 0;
}