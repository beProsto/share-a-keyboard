#include <share-a-keyb/networking.h>
#include <share-a-keyb/networking.h>
#include <stdio.h>
#include <sys/socket.h>

#define DESIRED_ADDRESS "127.0.0.1"
#define BUFSIZE 512

int main() {
  if (init_networking() != 0) {
    perror("Init failed");
    return 1;
  }

  address_t addr = make_address(DESIRED_ADDRESS, DESIRED_PORT);

  socket_t sock = make_tcp_client_socket(&addr);
  if (sock < 0) {
    perror("Client failed to connect");
    return 2;
  }

  while (true) {
    char pressed = 0;

    int received_size = recv(sock, &pressed, 1, 0);
    if (received_size == 0) {
      printf("connection ended\n");
      return 3;
    } else if (received_size < 0) {
      printf("connection error\n");
      return 4;
    }

    printf("Received Key Press: [%d] = '%c'\n", pressed, pressed);
  }
  return 0;
}