#include <share-a-keyb/keyboard.h>
#include <stdio.h>

struct keyboard_event_writer {
  uint8_t unused;
};


keyboard_event_writer_t *init_keyboard_event_writer() {
  keyboard_event_writer_t *keybdev = malloc(sizeof(keyboard_event_writer_t));
  memset(keybdev, 0, sizeof(keyboard_event_writer_t));
  return keybdev;
}

int write_keyboard_input(keyboard_event_writer_t *keybdev,
                         key_input_info_t input_info) {
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
  return 0;
}