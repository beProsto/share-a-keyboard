#pragma once

#ifndef WIN32
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

#else
#include <windows.h>

#endif

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// eventtype;
//  0 - keyup
//  1 - keydown
//  2 - keyheld
// scancode;
//  system agnostic device (keyboard) scancode
struct key_input_info {
  unsigned int eventtype;
  unsigned int scancode;
};
typedef struct key_input_info key_input_info_t;

// used by the client to simulate key presses
int simulate_keyboard_input(key_input_info_t input_info);