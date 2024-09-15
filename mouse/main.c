#include <stdio.h>
#include "mouse.h"

int main()
{
    int clicks = 0;

    mouse_initialise();

    while (1)
    {
        clicks = mouse_process_events();

	if ( clicks == 2 )
	    printf("EMC MOUSE => Double click detected!\n");
	else if ( clicks == 1 )
	    printf("EMC MOUSE => Single click detected!\n");
    }

    mouse_cleanup();
}
