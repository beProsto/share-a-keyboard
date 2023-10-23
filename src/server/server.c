
#include <share-a-keyb/networking.h>
#include <stdio.h>

#define DESIRED_ADDRESS "0.0.0.0"
#define BUFSIZE 512

int main() {
  if (init_networking() != 0) {
    perror("Init failed");
    return 1;
  }

  address_t addr = make_address(DESIRED_ADDRESS, DESIRED_PORT);

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

  if (send(client_sock, "hello", 1, 0) == -1) {
    perror("Send error");
    close(client_sock);
    close(sock);
    return EXIT_FAILURE;
  }

  // char buf[BUFSIZE];
  // ssize_t readden = recv(sock, buf, BUFSIZE, 0);
  // if (readden < 0) {
  //   perror("Receive error");
  //   close(client_sock);
  //   close(sock);
  //   return EXIT_FAILURE;
  // } else if (readden == 0) {
  //   fprintf(stderr, "Client orderly shut down the connection.\n");
  // } else if (readden > 0) {
  //   if (readden < BUFSIZE) {
  //     fprintf(stderr, "Received less bytes (%zd) then requested (%d).\n",
  //             readden, BUFSIZE);
  //   }

  //   write(STDOUT_FILENO, buf, readden);
  // }

  close(client_sock);
  close(sock);
  return 0;
}