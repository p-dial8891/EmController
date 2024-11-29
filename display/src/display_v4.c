#define _DEFAULT_SOURCE 1
#include "EPD_Test.h"
#include "EPD_2in13_V4.h"
#include <time.h> 

#define _XOPEN_SOURCE 700
#include <stdio.h>
#include <sys/types.h>
#include <dirent.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>

static int getch (void)
{
        int ch;
        struct termios oldt, newt;

//        tcgetattr(STDIN_FILENO, &oldt);
//        newt = oldt;
//        newt.c_lflag &= ~(ICANON|ECHO);
//        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        ch = getchar();
//        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

        return ch;
}

static int countTracks ( char * output )
{
  DIR *dp;
  struct dirent *ep; 
  int count = 0;  
  dp = opendir ("./");
  if (dp != NULL)
  {
    while ((ep = readdir (dp)) != NULL)
      //puts (ep->d_name);
      count++;
          
    (void) closedir (dp);
    sprintf( output, "%d", count - 2 );
    return count;
  }
  else
  {
    perror ("Couldn't open the directory");
    return -1;
  }
}

int EPD_2in13_V4_test(void)
{

    Debug("EPD_2in13_V4_test Demo\r\n");
    if(DEV_Module_Init()!=0){
        return -1;
    }


    Debug("e-Paper Init and Clear...\r\n");
    EPD_2in13_V4_Init_Fast();
    DEV_Delay_ms(5000);

    struct timespec start={0,0};
    struct timespec finish={0,0}; 
    clock_gettime(CLOCK_REALTIME,&start);

    EPD_2in13_V4_Clear();
#if 0
    clock_gettime(CLOCK_REALTIME,&finish);
    Debug("%ld S\r\n",finish.tv_sec-start.tv_sec);	
#endif

    //Create a new image cache
    UBYTE *BlackImage;
    UWORD Imagesize = ((EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        return -1;
    }

#if 1 
    Debug("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);
    Paint_Clear(WHITE);
#endif

#if 1 
    Debug("Drawing display.\r\n");
    Paint_SelectImage(BlackImage);
    char fileCount[3] = {};
    int fileCountInt, oldFileCountInt = -1;
    while  ( getch() != 'q' )
    {
        fileCountInt = countTracks( fileCount );
	if ( oldFileCountInt != fileCountInt )
	{
            Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
            Paint_DrawString_EN(150, 80, fileCount, &Font20, WHITE, BLACK);
            EPD_2in13_V4_Display_Partial(BlackImage);
	}
	oldFileCountInt = fileCountInt;
	sleep(1);
    }
    //EPD_2in13_V4_Display_Fast(BlackImage);
    DEV_Delay_ms(10000);
#endif

#if 0   //Partial refresh, example shows time
	Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);  
    Debug("Partial refresh\r\n");
    Paint_SelectImage(BlackImage);
	
    PAINT_TIME sPaint_time;
    sPaint_time.Hour = 12;
    sPaint_time.Min = 34;
    sPaint_time.Sec = 56;
    UBYTE num = 10;
    for (;;) {
        sPaint_time.Sec = sPaint_time.Sec + 1;
        if (sPaint_time.Sec == 60) {
            sPaint_time.Min = sPaint_time.Min + 1;
            sPaint_time.Sec = 0;
            if (sPaint_time.Min == 60) {
                sPaint_time.Hour =  sPaint_time.Hour + 1;
                sPaint_time.Min = 0;
                if (sPaint_time.Hour == 24) {
                    sPaint_time.Hour = 0;
                    sPaint_time.Min = 0;
                    sPaint_time.Sec = 0;
                }
            }
        }
        Paint_ClearWindows(150, 80, 150 + Font20.Width * 7, 80 + Font20.Height, WHITE);
        Paint_DrawTime(150, 80, &sPaint_time, &Font20, WHITE, BLACK);

        num = num - 1;
        if(num == 0) {
            break;
        }
		EPD_2in13_V4_Display_Partial(BlackImage);
        DEV_Delay_ms(500);//Analog clock 1s
    }
#endif

#if 1 
	Debug("Clear...\r\n");
	EPD_2in13_V4_Init();
    EPD_2in13_V4_Clear();
#endif

    Debug("Goto Sleep...\r\n");
    EPD_2in13_V4_Sleep();

#if 0
    free(BlackImage);
    BlackImage = NULL;
#endif

    DEV_Delay_ms(2000);//important, at least 2s
    // close 5V
    Debug("close 5V, Module enters 0 power consumption ...\r\n");
    DEV_Module_Exit();
    return 0;
}

