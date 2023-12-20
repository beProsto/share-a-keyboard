#include <share-a-keyb/keyboard.h>
#include <stdio.h>

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


keyboard_event_reader_t *init_keyboard_event_reader() {
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
}

int read_keyboard_input(keyboard_event_reader_t *keybdev,
                        key_input_info_t *input_info) {
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
}