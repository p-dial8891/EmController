﻿#include <stdlib.h>     //exit()
#include <signal.h>     //signal()
#include "EPD_Test.h"   //Examples

void  Handler(int signo)
{
    //System Exit
    printf("\r\nHandler:exit\r\n");
    EPD_2in13_V4_Init();
    EPD_2in13_V4_Clear();
    EPD_2in13_V4_Sleep();
    DEV_Module_Exit();

    exit(0);
}

int main(void)
{
    // Exception handling:ctrl + c
    signal(SIGINT, Handler);
    
#if ( epd2in13V4 || emc )
    EPD_2in13_V4_test();
    
#else
    printf("Please specify the EPD model when making. \r\n");
    printf("Example: When you run the EPD_7in5_V2_test() program, input: sudo make clean && make EPD=epd7in5V2 \r\n");
    printf("Don't know which program you need to run? Refer to the user manual (Wiki) and main.c \r\n");
#endif
    
    return 0;
}
