#include <share-a-keyb/networking.h>
#include <share-a-keyb/keyboard.h>
#include <stdio.h>

#define DESIRED_ADDRESS "127.0.0.1"
#define BUFSIZE 512

int main(int argc, char **argv) {
  // assume the other arguments are the address and the port we want
  const char *chosen_address = ((argc > 1) ? argv[1] : DESIRED_ADDRESS);
  const uint16_t chosen_port = ((argc > 2) ? atoi(argv[2]) : DESIRED_PORT);

  if (init_networking() != 0) {
    perror("Init failed");
    return 1;
  }

  address_t addr = make_address(chosen_address, chosen_port);

  socket_t sock = make_tcp_client_socket(&addr);
  if (sock < 0) {
    perror("Client failed to connect");
    return 2;
  }

  while (true) {
    struct key_input_info keyinputinfo = {0};
    int received_size = recv(sock, &keyinputinfo, sizeof(keyinputinfo), 0);
    if (received_size == 0) {
      printf("connection ended by server\n");
      return 3;
    } else if (received_size < 0) {
      printf("connection error\n");
      return 4;
    }

    int caughtScanCode = keyinputinfo.scancode;

    unsigned int vkey = MapVirtualKeyA(caughtScanCode, MAPVK_VSC_TO_VK);

    printf("(%d) Received Key Press: [%d] = v[%d] = '%c'\n", keyinputinfo.eventtype, received_size, caughtScanCode, vkey,
           vkey);

// #ifndef WIN32
//     // TODO: Handle linux keyboard simulation
// #else 
    // prepare the caught input for windows
    
    // INPUT inputs[3] = {0};
    // uint8_t strokes = 1;
    // for(int i = 0; i < (sizeof(inputs)/sizeof(INPUT)); i++) {
    //   inputs[i].type = INPUT_KEYBOARD;
    // }

    // if(caughtKey == 127) { // backspace
    //   inputs[0].ki.wVk = VK_BACK;
    // }
    // else if(caughtKey == 23) { // ctrl+backspace
    //   strokes = 3;
    //   inputs[0].ki.wVk = VK_CONTROL;
    //   inputs[1].ki.wVk = VK_BACK;
    //   inputs[2].ki.wVk = VK_CONTROL;
    //   inputs[2].ki.dwFlags = KEYEVENTF_KEYUP;
    // }
    // else if(caughtKey == 10) { // enter
    //   inputs[0].ki.wVk = VK_RETURN;
    // }
    // else {
    //   inputs[0].ki.wScan = caughtKey;
    //   inputs[0].ki.dwFlags = KEYEVENTF_UNICODE;
    // }
      INPUT inputs[3] = {0};
      uint8_t strokes = 1;
      for(int i = 0; i < (sizeof(inputs)/sizeof(INPUT)); i++) {
        inputs[i].type = INPUT_KEYBOARD;
      }
      inputs[0].ki.dwFlags = KEYEVENTF_SCANCODE;
      inputs[0].ki.wScan = caughtScanCode;
      if(keyinputinfo.eventtype == 0) {
        inputs[0].ki.dwFlags |= KEYEVENTF_KEYUP;
      }

      UINT uSent = SendInput(strokes, inputs, sizeof(INPUT));
      if (uSent != strokes) {
        printf("For some reason didn't send the input\n");
      }
// #endif
  }
  return 0;
}