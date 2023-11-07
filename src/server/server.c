#include <assert.h>
#include <share-a-keyb/keyboard.h>
#include <share-a-keyb/networking.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int find_keyboard_event_name(char *evname) {
  // check which input device is a keyboard
  FILE *devicesfile = fopen("/proc/bus/input/devices", "r");
  if (devicesfile == NULL) {
    printf("Device list cannot be accessed\n");
    return -1;
  }

  // go through each line checking for a reference to the keyboard
  char devslinebuff[512] = {0};
  size_t devslinebuffsize = 512;
  char *linebuff = devslinebuff;
  const char *comparison = "H: Handlers=sysrq kbd ";
  const size_t comparison_len = strlen(comparison);
  while (getline(&linebuff, &devslinebuffsize, devicesfile)) {
    // printf("Found %s\n", linebuff);
    if (strncmp(linebuff, comparison, comparison_len) == 0) {
      char junk[64];
      sscanf(linebuff, "H: Handlers=sysrq kbd %s %s", junk, evname);
      return 0;
    }
  }
  return -2;
}

#define DESIRED_ADDRESS "0.0.0.0"
#define BUFSIZE 512

void handler(int id) { return; }
int main(int argc, char **argv) {
  signal(SIGINT, handler);
  signal(SIGTSTP, handler);
  // assume the other argument is the port we want
  const uint16_t chosen_port = ((argc > 1) ? atoi(argv[1]) : DESIRED_PORT);

  if (init_networking() != 0) {
    printf("Init failed\n");
    return 1;
  }

  // find out which input event is the keyboard
  const char *evfnamebase = "/dev/input/";
  const size_t evfnamebaselen = strlen(evfnamebase);
  char evfname[64] = {0};
  strcpy(evfname, evfnamebase);
  if (find_keyboard_event_name(evfname + evfnamebaselen) < 0) {
    printf("Couldn't find a keyboard device.\n");
    return 2;
  }

  // for now we want the file to just load a specific event
  // file: "/dev/input/event3" for example
  int evfile = open(evfname, O_RDONLY);
  if (evfile < 0) {
    printf("Keyboard device cannot be accessed; You may need to run as root\n");
    return 1;
  }
  address_t addr = make_address(DESIRED_ADDRESS, chosen_port);

  socket_t sock = make_tcp_server_socket(&addr);
  if (sock < 0) {
    printf("Server failed to initialise");
    return 2;
  }

  address_t client_addr = {0};
  socket_t client_sock = wait_for_tcp_client_socket(sock, &client_addr);
  if (client_sock < 0) {
    printf("Couldn't accept a client");
    return 3;
  }

  printf("Client with IP %s connected\n", inet_ntoa(client_addr.sin_addr));

  struct key_input_event {
    struct timeval time;
    unsigned short evpart;
    unsigned short unused;
    unsigned int value;
  };

  int g;

  struct key_input_event keyinputs[3] = {0};
  const size_t keyinputs_size = sizeof(struct key_input_event) * 3;

  key_input_info_t keyinputinfo = {0};

  while (true) {
    if ((g = read(evfile, keyinputs, keyinputs_size)) > 0) {
      // Whenever a key event happens, 3 writes for this structure occur.
      // First struct [evpart: 4] contains the Scan Code [value]
      // Second struct [evpart: 1] contains the Event Type [value: 0,1,2]
      // Third struct [evpart: 0] contains nothing, simply signals the end
      printf("\nScanCode: [%d] Evpart: [%d]\n", keyinputs[0].value,
             keyinputs[0].evpart);
      printf("EvType: [%d] Evpart: [%d]\n", keyinputs[1].value,
             keyinputs[1].evpart);
      printf("Zero: [%d] Evpart: [%d]\n", keyinputs[2].value,
             keyinputs[2].evpart);

      // A sanity check in this case would simply mean checking if all those
      // circumstances are met:
      bool sanity_check =
          (((keyinputs[0].evpart == 4) &&  // First read is the Scan Code
            (keyinputs[1].evpart == 1) &&  // Second read is the Event Type
            (keyinputs[2].evpart == 0)) && // Third read is the Finish
                                           // Event Type is either
           ((keyinputs[1].value == 0) ||   // KeyUp
            (keyinputs[1].value == 1) ||   // KeyDown
            (keyinputs[1].value == 2)));   // KeyHold

      // I really don't want to mess with what could possibly happen for that to
      // fail, so we just exit the programme
      assert(sanity_check);

      // Put parsed values to use
      keyinputinfo.eventtype = keyinputs[1].value;
      keyinputinfo.scancode = keyinputs[0].value;

      if (send(client_sock, &keyinputinfo, sizeof(keyinputinfo), 0) < 0) {
        printf("Send error");
        close(client_sock);
        close(sock);
        return 4;
      }
    } else if (g == 0) {
      printf("Zero Received\n");
    } else {
      printf("Less than zero received\n");
    }
  }

  close(client_sock);
  close(sock);
  return 0;
}