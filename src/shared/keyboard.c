#include <share-a-keyb/keyboard.h>

//// CLIENT

int simulate_keyboard_input(key_input_info_t input_info) {
#ifndef WIN32
  // TODO: Handle linux keyboard simulation
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

struct keyboard_event {
  int event_file_desc;
  key_input_device_event_t keyinputs[3];
};

const static size_t keyinputs_size = sizeof(key_input_device_event_t) * 3;

#else // Windows Definitions
struct keyboard_event {
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

keyboard_event_t *init_keyboard_event() {
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
  keyboard_event_t *keybdev = malloc(sizeof(keyboard_event_t));
  memset(keybdev, 0, sizeof(keyboard_event_t));
  keybdev->event_file_desc = event_file_desc;

  return keybdev;

#else
  keyboard_event_t *keyb = malloc(sizeof(keyboard_event_t));
  memset(keyb, 0, sizeof(keyboard_event_t));

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

int read_keyboard_input(keyboard_event_t *keybdev,
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