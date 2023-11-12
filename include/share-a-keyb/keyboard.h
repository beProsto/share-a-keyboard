#pragma once

#include "pch.h"
#include <stdint.h>

#ifndef WIN32
#include <fcntl.h>
#include <linux/input-event-codes.h>
#include <linux/uinput.h>
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
  uint8_t eventtype;
  uint8_t scancode;
};
typedef struct key_input_info key_input_info_t;

// used by the client to simulate key presses
struct keyboard_event_writer;
typedef struct keyboard_event_writer keyboard_event_writer_t;

keyboard_event_writer_t *init_keyboard_event_writer();

int write_keyboard_input(keyboard_event_writer_t *keybdev,
                         key_input_info_t input_info);

// used by the server to process key presses
struct keyboard_event_reader;
typedef struct keyboard_event_reader keyboard_event_reader_t;

keyboard_event_reader_t *init_keyboard_event_reader();

int read_keyboard_input(keyboard_event_reader_t *keybdev,
                        key_input_info_t *input_info);