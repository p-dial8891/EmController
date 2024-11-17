#include <libevdev/libevdev.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include "mouse.h"

static struct libevdev *dev           = NULL;
static int    fd                      = -1;
static int    rc;

int mouse_initialise ( char * devName )
{
    int retry = 0;
    // Open the input device (replace with your mouse device file, e.g., /dev/input/eventX)
    while ( retry < 10 && fd < 0 )
    {
        fd = open(devName, O_RDONLY | O_NONBLOCK); // Replace "eventX" with the actual event number
        if (fd < 0) {
            perror("Failed to open device");
        }
	retry++;
    }

    if ( fd < 0 ) {
        perror("Failed to open device");
	return 1;
    }

    // Initialize the libevdev device
    rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
        return 1;
    }

    printf("Listening for mouse events...\n");

    return rc;
}

int mouse_process_events ( void )
{
    struct input_event ev;

    rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);

    if (rc == 0) {
	//printf("EMC MOUSE => Event received.\n");
        if (ev.type == EV_KEY && ev.code == BTN_LEFT && ev.value == 1)
	{	
      	    return 1;
	}
	else if (ev.type == EV_KEY && ev.code == BTN_RIGHT && ev.value == 1) 
	{
	    return 2;
	}
    }

    return 0;
}

int mouse_cleanup ( void )
{
    libevdev_free(dev);
    close(fd);
    return 0;
} 
