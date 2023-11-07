#pragma once

#ifndef WIN32
#include <arpa/inet.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#else
#include <winsock2.h>

#endif

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

// These are to be used by the user (The API)
typedef struct sockaddr_in address_t;
#ifndef WIN32
typedef int socket_t;
#else
typedef int socklen_t;
typedef SOCKET socket_t;
#endif

int init_networking();
address_t make_address(const char *ipv4, uint16_t port);
socket_t make_tcp_server_socket(address_t *address);
socket_t make_tcp_client_socket(address_t *address);
socket_t wait_for_tcp_client_socket(socket_t server_socket, address_t *address);

// These may just be used internally
socket_t _make_tcp_socket();
int _init_tcp_server(socket_t socket, address_t *address);
int _init_tcp_client(socket_t socket, address_t *address);