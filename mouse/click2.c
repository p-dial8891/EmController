#include <libevdev/libevdev.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>

#define DOUBLE_CLICK_TIME 0.5 // Max time in seconds between clicks to be considered a double click

int main() {
    struct libevdev *dev = NULL;
    int fd;
    int rc;

    // Open the input device (replace with your mouse device file, e.g., /dev/input/eventX)
    fd = open("/dev/input/by-id/usb-ZEPHYR_Zephyr_HID_mouse_sample_2735883958566D65-event-mouse", O_RDONLY | O_NONBLOCK); // Replace "eventX" with the actual event number
    if (fd < 0) {
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

    struct timeval last_click_time = {0, 0};
    int click_count = 0;
    struct timeval current_time;
    double deadline;
    double dCurTime;

    while (1) {
        struct input_event ev;

        gettimeofday(&current_time, NULL);
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
        dCurTime = current_time.tv_sec + ( current_time.tv_usec / 1000000.0 );

        if (rc == 0) {
            if (ev.type == EV_KEY && ev.code == BTN_LEFT && ev.value == 1) { // Button press

                click_count++;
                if (click_count == 3)
                    click_count = 0;

                deadline = current_time.tv_sec + ( current_time.tv_usec / 1000000.0 ) +
                           DOUBLE_CLICK_TIME;

            }
        }
        
        if ( dCurTime > deadline )
        {
            if (click_count == 1) { 
                printf("Single click detected!\n");
            }
            else
            if (click_count == 2) {
                printf("Double click detected!\n");
            }
            click_count = 0;
        }
    }

    libevdev_free(dev);
    close(fd);
    return 0;
}
