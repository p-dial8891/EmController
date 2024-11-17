#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>
#include <string>
#include <cstdlib>
#include <stdio.h>
#include <iostream>
#include <sys/stat.h>
#include <thread>
#include <chrono>

#define DOUBLE_CLICK_TIME 0.5 // Max time in seconds between clicks to be considered a double click

using namespace std;

static struct libevdev *dev           = NULL;
static int    fd;
static int    rc;
static int    click_count             =  0;
static struct timeval current_time;
static double deadline;
static double dCurTime;
static struct libevdev *devPlayer;
static struct libevdev_uinput *uidev;


static int mouse_initialise ( char * filename )
{
    // Open the input device (replace with your mouse device file, e.g., /dev/input/eventX)
    fd = open(filename, O_RDONLY | O_NONBLOCK); // Replace "eventX" with the actual event number
    if (fd < 0) {
        perror("Failed to open device");
        return 1;
    }

    // Initialize the libevdev device
    rc = libevdev_new_from_fd(fd, &dev);
    if (rc < 0) {
        //fprintf(stderr, "Failed to init libevdev (%s)\n", strerror(-rc));
        cout << "Failed to init libevdev " << endl;
        return 1;
    }

    printf("Listening for mouse events...\n");

    return rc;
}

static int mouse_process_events ( void )
{
    struct input_event ev;
    int ret_clicks = 0;

    gettimeofday(&current_time, NULL);
    rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev);
    dCurTime = current_time.tv_sec + ( current_time.tv_usec / 1000000.0 );

    if (rc == 0) {
        if (ev.type == EV_KEY && ev.code == BTN_EAST && ev.value == 1) { // Button press

            click_count++;
            if (click_count == 3)
                click_count = 0;

            deadline = current_time.tv_sec + ( current_time.tv_usec / 1000000.0 ) + DOUBLE_CLICK_TIME;

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
	ret_clicks = click_count;
        click_count = 0;
    }

    return ret_clicks;
}

static int mouse_cleanup ( void )
{
    libevdev_free(dev);
    close(fd);
    return 0;
}

void Handler ( int sigNo )
{

    cout << "EMC CTL => Controller exiting..." << endl;
    (void)mouse_cleanup();
    libevdev_uinput_destroy(uidev);
    exit(0);
}

int main ( int argc, char * argv[] )
{

    int err;
    struct stat buffer;
    mouse_initialise("/dev/input/by-id/usb-Microsoft_Inc._WX4_controller-event-joystick");

    devPlayer = libevdev_new();
    libevdev_set_name(devPlayer, "toPlayer");
    libevdev_enable_event_type(devPlayer, EV_KEY);
    libevdev_enable_event_code(devPlayer, EV_KEY, BTN_LEFT, NULL);
    libevdev_enable_event_code(devPlayer, EV_KEY, BTN_RIGHT, NULL);
    libevdev_enable_event_type(devPlayer, EV_SYN);
    libevdev_enable_event_code(devPlayer, EV_SYN, SYN_REPORT, NULL);

    err = libevdev_uinput_create_from_device(devPlayer, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    if ( err < 0 )
    {
        cout << "EMC CTL => Error creating uinput." << endl;
        return 1;
    }
    
    signal(SIGINT, Handler);

    const char * player_devnode = libevdev_uinput_get_devnode(uidev);
    while( stat(player_devnode, &buffer) != 0 ) ;
    std::string cmd1 = "../player/player.out ";
    std::string cmd2 = player_devnode;
    std::string cmd3 = " &";
    const std::string cmd = cmd1 + cmd2 + cmd3;
    cout << "EMC CTL => Command string is : " + cmd << endl;
    system(cmd.c_str());

    while ( true )
    {
        int clicks = mouse_process_events();
	if ( clicks == 1 )
	{
	    err = libevdev_uinput_write_event(uidev, EV_KEY, BTN_LEFT, 1);
	    err = libevdev_uinput_write_event(uidev, EV_KEY, BTN_LEFT, 0);
	    err = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
	}
	else if ( clicks == 2 )
	{
	    err = libevdev_uinput_write_event(uidev, EV_KEY, BTN_RIGHT, 1);
	    err = libevdev_uinput_write_event(uidev, EV_KEY, BTN_RIGHT, 0);
	    err = libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
	}
        
	std::this_thread::sleep_for(std::chrono::milliseconds(50));   
    }

    mouse_cleanup();
    libevdev_uinput_destroy(uidev);

}
