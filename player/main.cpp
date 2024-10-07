#include "player.h"
#include <thread>
#include <filesystem>
#include <sys/stat.h>
#include "mouse.h"
#include <chrono>
#include <random>
#include <stdio.h>
#include "config.h"
#include <signal.h>

namespace fs = std::filesystem;

char * ticker[] = { "-", "\\", "|", "/"};

typedef std::mt19937 MyRNG_t;

MyRNG_t rng;

static void rand_initialise(void)
{
    FILE * fp = fopen("/dev/urandom", "rb");
    unsigned int randNo = 0;
    fread(&randNo, 1, 4, fp);
    rng.seed(randNo);
    cout << "EMC PLAYER => Random number is " << randNo << endl;
    fclose(fp);
}

static int findNextSlot( int start )
{
    int fnCount = -1;
    struct stat buffer;
    char temp[3] = {};

    for ( fnCount = ( start + 1 ) % 16 ; 
          fnCount != start ; 
	  fnCount =  ( ( fnCount + 1 ) % 16 ) )
    {
	sprintf(temp, "%02d", fnCount);
	if ( stat(temp, &buffer) != 0 )
        {
	   //cout << "EMC PLAYER => Next slot found : " << fnCount << endl;
           return fnCount;
	}
    }

    sprintf(temp, "%02d", fnCount);
    if ( stat(temp, &buffer) != 0 )
    {
        //cout << "EMC PLAYER => Next slot found : " << fnCount << endl;
        return fnCount;
    }

    return -1;
}


static int findNextFile( int start )
{
    int fnCount = -1;
    struct stat buffer;
    char temp[3] = {};

    for ( fnCount = ( start + 1 ) % 16 ; 
          fnCount != start ; 
	  fnCount =  ( ( fnCount + 1 ) % 16 ) )
    {
	sprintf(temp, "%02d", fnCount);
	if ( stat(temp, &buffer) == 0 )
        {
	   //cout << "EMC PLAYER => Next file found : " << fnCount << endl;
           return fnCount;
	}
    }

    sprintf(temp, "%02d", fnCount);
    if ( stat(temp, &buffer) == 0 )
    {
        //cout << "EMC PLAYER => Next file found : " << fnCount << endl;
        return fnCount;
    }

    return -1;
}

static void selectRandomTrack( int currFile )
{
    int trackCount = 0;
    int index = 0;
    int randNum = 0;
    string selection = "";

    for (const fs::directory_entry& dir_entry :
            fs::recursive_directory_iterator("../music"))
    {
        trackCount += 1;
    }

    if ( trackCount != 0 )
    {
        std::uniform_int_distribution<uint32_t> uint_dist(0,trackCount);
	randNum = uint_dist(rng);
    }
    else
	randNum = 0;

    for (const fs::directory_entry& dir_entry :
            fs::recursive_directory_iterator("../music"))
    {
        if ( !dir_entry.is_directory() && index >= randNum )
	{
            selection = dir_entry.path().string();
	    break;
	}
	index++;
    }

    int nextSlot = findNextSlot ( currFile );

    if ( ( currFile >= 0 ) && ( selection != "" ) && ( nextSlot != -1 ) )
    {
        char temp[3] = {};
	sprintf(temp, "%02d", nextSlot);
        cout << "EMC PLAYER => Next track selected : " << selection << endl;
        string cmd = ( "ln -s \"" + selection + "\" " + std::string(temp) );
	system(cmd.c_str());
    }
    cout << "EMC PLAYER => Trac count : " << std::to_string(trackCount) <<
	    " Random number : " << std::to_string(randNum) <<
	    " Next slot : " << std::to_string(nextSlot) <<
	    " Selection : " << selection;
}

void Handler ( int sigNo )
{
    cout << "EMC PLAYER => Exiting player ..." << endl;
    mouse_cleanup();
    exit(0);
}

int main( int argc, char * argv[] )
{
    int nextFile, currFile = 0;
    nextFile = findNextFile( 0 );
    std::thread * thPtr = NULL;
    Player * currTrackPtr = NULL;

    static int tick = 0;

    signal(SIGINT, Handler);
    mouse_initialise ( argv[1] );
    rand_initialise();

    while(1)
    {	
        
	int clicks = mouse_process_events();

        if ( nextFile >= 0 )
	{
            char temp[3] = {};
            sprintf(temp, "%02d", nextFile );	    
	    //cout << "EMC PLAYER => New File found : " << nextFile << endl;
	    //Player track = Player(std::string(temp));
	    //track.Play();
	    if ( ( thPtr == NULL ) && ( currTrackPtr == NULL ) )
	    {
		cout << "EMC PLAYER => Starting new file : " + nextFile <<
		endl;
		currFile = nextFile;
		currTrackPtr = new Player(std::string(temp));
                thPtr = new std::thread(&Player::Play, currTrackPtr);
	    }
	    else if ( ( thPtr != NULL ) && ( currTrackPtr != NULL ) &&
                !currTrackPtr->isTrackRunning() )
	    {
		char temp[3];

	        cout << "EMC PLAYER => Deleting current file." << endl;
		//thPtr->~thread();
                thPtr->detach();
		delete(thPtr);
		delete(currTrackPtr);

		sprintf(temp, "%02d", currFile);
		string cmd = "rm " + std::string(temp);
		system(cmd.c_str());
		
		thPtr = NULL;
		currTrackPtr = NULL;
	    }
	    else if ( ( thPtr != NULL ) && ( currTrackPtr != NULL ) &&
	        currTrackPtr->isTrackRunning() )
	    {
	        //cout << "EMC PLAYER => Finding next file." << endl;
		nextFile = findNextFile( currFile );
	    }
	}
	else
            nextFile = findNextFile( 0 );

	if ( ( clicks == 1 ) && ( thPtr != NULL ) && ( currTrackPtr != NULL) &&
	     currTrackPtr->isTrackRunning() )
	{
	    char temp[3];
     
	    cout << "EMC PLAYER => Skipping track." << endl;
#ifdef USE_FFMPEG
            string s="pkill -f ffplay;";
#elif defined(USE_MPG123)
            string s="pkill -f mpg123;";
#elif defined(USE_PACAT)
            string s="pkill -f ffmpeg; pkill -f pacat;";
#endif
	    system(s.c_str());
	    thPtr->detach();
	    delete(thPtr);
	    delete(currTrackPtr);

	    sprintf(temp, "%02d", currFile);
	    string cmd = "rm " + std::string(temp);
	    system(cmd.c_str());

	    thPtr = NULL;
	    currTrackPtr = NULL;
	}
	else if ( clicks == 2 )
	{
	    cout << "EMC PLAYER => Selecting random track.." << endl;
	    selectRandomTrack(currFile);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	cout<<"\b"<<ticker[tick/10];
        tick = ( tick + 1 ) % 40;	
    }

    mouse_cleanup();

}

