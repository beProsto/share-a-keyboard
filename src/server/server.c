#include <share-a-keyb/keyboard.h>
#include <share-a-keyb/networking.h>
#include <stdio.h>

#define DESIRED_ADDRESS "0.0.0.0"
#define BUFSIZE 512

#ifndef WIN32
void handler(int id) { return; }
#endif

int main(int argc, char **argv) {
#ifndef WIN32
  // Handle halt/termination signals on linux (ctrl+z and ctrl+c)
  signal(SIGINT, handler);
  signal(SIGTSTP, handler);
#endif

  // assume the other argument is the port we want
  const uint16_t chosen_port = ((argc > 1) ? atoi(argv[1]) : DESIRED_PORT);

  keyboard_event_t *kevent = init_keyboard_event();
  if (kevent == NULL) {
    printf("Keyboard init failed\n");
    return 1;
  }

  if (init_networking() != 0) {
    printf("Init failed\n");
    return 1;
  }

  address_t addr = make_address(DESIRED_ADDRESS, chosen_port);

  socket_t sock = make_tcp_server_socket(&addr);
  if (sock < 0) {
    printf("Server failed to initialise");
    return 2;
  }

  printf("Server waiting for connections on port %d...\n", chosen_port);

  address_t client_addr = {0};
  socket_t client_sock = wait_for_tcp_client_socket(sock, &client_addr);
  if (client_sock < 0) {
    printf("Couldn't accept a client");
    return 3;
  }

  printf("Client with IP %s connected\n", inet_ntoa(client_addr.sin_addr));

  key_input_info_t keyinputinfo = {0};

  while (true) {
    if (read_keyboard_input(kevent, &keyinputinfo) < 0) {
      // printf("Invalid keyboard input!\n");
      continue;
      // return -1;
    }

    // printf("[%d](%d)\n", keyinputinfo.eventtype, keyinputinfo.scancode);

    if (send(client_sock, &keyinputinfo, sizeof(keyinputinfo), 0) < 0) {
      printf("Connection broken\n");
      close(client_sock);
      close(sock);
      return -1;
    }
  }

  close(client_sock);
  close(sock);
  return 0;
}