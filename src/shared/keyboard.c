#include <share-a-keyb/keyboard.h>
#include <stdio.h>

//// CLIENT SIDE
#ifndef WIN32
#include "./keyboard-writer-linux.h"

#else
#include "./keyboard-writer-winapi.h"

#endif

//// SERVER SIDE
#ifndef WIN32
#include "./keyboard-reader-linux.h"

#else
#include "./keyboard-reader-winapi.h"

#endif
