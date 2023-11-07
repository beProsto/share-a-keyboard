#pragma once

#include "pch.h"

#ifndef WIN32
#include <signal.h>
#include <unistd.h>

#else
#include <windows.h>

#endif

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