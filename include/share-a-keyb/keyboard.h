#pragma once

#ifndef WIN32
#include <stdio.h>
#include <unistd.h>

#else
#include <conio.h>
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
