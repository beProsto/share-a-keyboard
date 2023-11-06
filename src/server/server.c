
#include <share-a-keyb/getch.h>
#include <share-a-keyb/networking.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define DESIRED_ADDRESS "0.0.0.0"
#define BUFSIZE 512

int main(int argc, char **argv) {
  // assume the other argument is the port we want
  const uint16_t chosen_port = ((argc > 1) ? atoi(argv[1]) : DESIRED_PORT);

  if (init_networking() != 0) {
    perror("Init failed");
    return 1;
  }

  // for now we want the file to just load a specific event
  int evfile = open("/dev/input/event3", O_RDONLY);
  // int flags = fcntl(evfile, F_GETFL, 0);
  // fcntl(evfile, F_SETFL, flags | O_NONBLOCK);

  int g;

  address_t addr = make_address(DESIRED_ADDRESS, chosen_port);

  socket_t sock = make_tcp_server_socket(&addr);
  if (sock < 0) {
    perror("Server failed to initialise");
    return 2;
  }

  address_t client_addr = {0};
  socket_t client_sock = wait_for_tcp_client_socket(sock, &client_addr);
  if (client_sock < 0) {
    perror("Couldn't accept a client");
    return 3;
  }

  printf("Client with IP %s connected\n", inet_ntoa(client_addr.sin_addr));

  struct input_event {
    struct timeval time;
    unsigned short type;
    unsigned short code;
    unsigned int value;
  };

  while (true) {
    int temp_arr[1] = {getch()};
    int i = 0;
    struct input_event curr_input = {0};
    const size_t sizeex = sizeof(struct input_event);
    while ((g = read(evfile, &curr_input, sizeex)) >= 0) {
      printf("Key: [%c]|[%d] Code: [%c]|[%d] Type: [%c]|[%d]\n",
             curr_input.value, curr_input.value, curr_input.code,
             curr_input.code, curr_input.type, curr_input.type);
    } // make this while loop nonblocking
    if (send(client_sock, temp_arr, sizeof(temp_arr), 0) < 0) {
      perror("Send error");
      close(client_sock);
      close(sock);
      return 4;
    }
  }

  close(client_sock);
  close(sock);
  return 0;
}