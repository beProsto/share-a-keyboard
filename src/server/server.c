
#include <share-a-keyb/getch.h>
#include <share-a-keyb/networking.h>
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

  while (true) {
    int temp_arr[1] = {getche()};
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