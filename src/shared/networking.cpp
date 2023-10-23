#include <iostream>
#include <share-a-keyb/networking.hpp>

// we need that for winapi support
// returns 0 if succeeded
int init_networking() { return 0; }

address_t make_address(const char *ipv4, uint16_t port) {
  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port);
  addr.sin_addr.s_addr = inet_addr(ipv4);
  return addr;
}

socket_t make_tcp_server_socket(address_t *address) {
  socket_t socket = _make_tcp_socket();
  if (socket == -1) {
    return -1;
  }
  if (_init_tcp_server(socket, address) != 0) {
    return -2;
  }
  return socket;
}
socket_t make_tcp_client_socket(address_t *address) {
  socket_t socket = _make_tcp_socket();
  if (socket == -1) {
    return -1;
  }
  if (_init_tcp_client(socket, address) != 0) {
    return -2;
  }
  return socket;
}

socket_t wait_for_tcp_client_socket(socket_t server_socket,
                                    address_t *address) {
  socklen_t socklen = sizeof(address_t);
  int client_sock = accept(server_socket, (sockaddr *)address,
                           &socklen); /* 2nd and 3rd argument may be NULL. */
  if (client_sock == -1) {
    // perror("Accept error");
    close(server_socket);
    return -1;
  }
  return client_sock;
}

socket_t _make_tcp_socket() {
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock == -1) {
    return -1;
  }
  return sock;
}
int _init_tcp_server(socket_t socket, address_t *address) {
  if (bind(socket, (struct sockaddr *)address, sizeof(address_t)) == -1) {
    // perror("Bind error");
    close(socket);
    return -1;
  }
  if (listen(socket, 1) == -1) {
    // perror("Listen error");
    close(socket);
    return -2;
  }
  return 0;
}
int _init_tcp_client(socket_t socket, address_t *address) {
  if (connect(socket, (struct sockaddr *)address, sizeof(address_t)) == -1) {
    // perror("Connection error");
    close(socket);
    return -1;
  }
  return 0;
}