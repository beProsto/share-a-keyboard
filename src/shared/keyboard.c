#include <share-a-keyb/keyboard.h>

int simulate_keyboard_input(key_input_info_t input_info) {
#ifndef WIN32
    // TODO: Handle linux keyboard simulation
#else 
    INPUT siminput = {0};
    const uint8_t strokes = 1;
    siminput.type = INPUT_KEYBOARD;
    siminput.ki.dwFlags = KEYEVENTF_SCANCODE;
    siminput.ki.wScan = input_info.scancode;
    if(input_info.eventtype == 0) {
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