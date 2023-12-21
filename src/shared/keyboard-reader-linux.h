#include <share-a-keyb/keyboard.h>
#include <stdio.h>

struct key_input_device_event {
  struct timeval time;
  unsigned short evpart;
  unsigned short unused;
  unsigned int value;
}; // This struct is defined in <linux/input-event-codes.h> as input_event
   // but it's naming scheme is weird just so it can be universal.
typedef struct key_input_device_event key_input_device_event_t;

struct keyboard_event_reader {
  int event_file_desc;
  key_input_device_event_t keyinputs[3];
};

const static size_t keyinputs_size = sizeof(key_input_device_event_t) * 3;

keyboard_event_reader_t *init_keyboard_event_reader() {
  // check which input device is a keyboard
  FILE *devicesfile = fopen("/proc/bus/input/devices", "r");
  if (devicesfile == NULL) {
    printf("Device list cannot be accessed\n");
    return NULL;
  }

  // ready the event filename
  const char *evfnamebase = "/dev/input/";
  const size_t evfnamebaselen = strlen(evfnamebase);
  char eventfilename[64] = {0};
  strcpy(eventfilename, evfnamebase);
  char *eventname = eventfilename + evfnamebaselen;

  // go through each line checking for a reference to the keyboard
  char devslinebuff[512] = {0};
  size_t devslinebuffsize = 512;
  char *linebuff = devslinebuff;
  bool found = false;
  while (getline(&linebuff, &devslinebuffsize, devicesfile)) {
    // if we find a matching line, we take out it's last part
    // because it's the event's filename
    const char *comparison = "H: Handlers=sysrq kbd ";
    const size_t comparison_len = strlen(comparison);
    if (strncmp(linebuff, comparison, comparison_len) == 0) {
      char junk[64];
      sscanf(linebuff, "H: Handlers=sysrq kbd %s %s", junk, eventname);
      found = true;
      break;
    }
  }
  if (!found) {
    printf("Couldn't find a keyboard device.\n");
    return NULL;
  }

  fclose(devicesfile);

  // printf("KeyEvent Filename: %s\n", eventfilename);
  // "/dev/input/event3" for example

  // Load the found event file, we'll fetch our key events from there
  int event_file_desc = open(eventfilename, O_RDONLY);
  if (event_file_desc < 0) {
    printf("Keyboard device cannot be accessed; You may need to run as root\n");
    return NULL;
  }

  // Return a new keyboard event with the file descriptor for our keyboard event
  keyboard_event_reader_t *keybdev = malloc(sizeof(keyboard_event_reader_t));
  memset(keybdev, 0, sizeof(keyboard_event_reader_t));
  keybdev->event_file_desc = event_file_desc;

  return keybdev;
}

int read_keyboard_input(keyboard_event_reader_t *keybdev,
                        key_input_info_t *input_info) {
  int recvd;
  if ((recvd = read(keybdev->event_file_desc, keybdev->keyinputs,
                    keyinputs_size)) > 0) {
    // Whenever a key event happens, 3 writes for this structure occur.
    // First struct [evpart: 4] contains the Scan Code [value]
    // Second struct [evpart: 1] contains the Event Type [value: 0,1,2]
    // Third struct [evpart: 0] contains nothing, simply signals the end

    // printf("\nScanCode: [%d] Evpart: [%d]\n", keybdev->keyinputs[0].value,
    //        keybdev->keyinputs[0].evpart);
    // printf("EvType: [%d] Evpart: [%d]\n", keybdev->keyinputs[1].value,
    //        keybdev->keyinputs[1].evpart);
    // printf("Zero: [%d] Evpart: [%d]\n", keybdev->keyinputs[2].value,
    //        keybdev->keyinputs[2].evpart);

    // A sanity check in this case would simply mean checking if all those
    // circumstances are met:
    bool sanity_check =
        (((keybdev->keyinputs[0].evpart == 4) && // First read is the Scan Code
          (keybdev->keyinputs[1].evpart ==
           1) && // Second read is the Event Type
          (keybdev->keyinputs[2].evpart == 0)) && // Third read is the Finish
                                                  // Event Type is either
         ((keybdev->keyinputs[1].value == 0) ||   // KeyUp
          (keybdev->keyinputs[1].value == 1) ||   // KeyDown
          (keybdev->keyinputs[1].value == 2)));   // KeyHold

    // I really don't want to mess with what could possibly happen for that to
    // fail, so we just exit the programme.
    if (!sanity_check) {
      printf("Sanity check somehow went wrong.\n");
      return -1;
    }

    // Put parsed values to use
    input_info->eventtype = keybdev->keyinputs[1].value;
    input_info->scancode = keybdev->keyinputs[0].value;

    return 0;
  } else {
    printf("Error reading keyboard input!\n");
    return -2;
  }
}