#include <share-a-keyb/networking.h>
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
    int temp_arr[1] = {0};
    int received_size = recv(sock, temp_arr, sizeof(temp_arr), 0);
    if (received_size == 0) {
      printf("connection ended by server\n");
      return 3;
    } else if (received_size < 0) {
      printf("connection error\n");
      return 4;
    }

    printf("%d Received Key Press: [%d] = '%c'\n", received_size, temp_arr[0],
           temp_arr[0]);

#ifdef WIN32
    INPUT inputs[1] = {};

    inputs[0].type = INPUT_KEYBOARD;
    inputs[0].ki.wVk = temp_arr[0];

    UINT uSent = SendInput(ARRAYSIZE(inputs), inputs, sizeof(INPUT));
    if (uSent != ARRAYSIZE(inputs)) {
      printf("For some reason didn't send the input\n");
    }
#endif
  }
  return 0;
}