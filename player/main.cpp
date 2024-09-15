#include "player.h"
#include <thread>
#include <filesystem>
#include <sys/stat.h>
#include "mouse.h"
#include <chrono>


namespace fs = std::filesystem;

int findNextFile( int start )
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

int main()
{
    int nextFile, currFile;
    nextFile = findNextFile( 0 );
    std::thread * thPtr = NULL;
    Player * currTrackPtr = NULL;

    mouse_initialise();

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
	        cout << "EMC PLAYER => Deleting current file." << endl;
		//thPtr->~thread();
                thPtr->detach();
		delete(thPtr);
		delete(currTrackPtr);

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
	    cout << "EMC PLAYER => Skipping track." << endl;
            string s="pkill -f ffmpeg; pkill -f pacat;";
	    system(s.c_str());
	    thPtr->detach();
	    delete(thPtr);
	    delete(currTrackPtr);
	    thPtr = NULL;
	    currTrackPtr = NULL;
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(100));
    
    }

    mouse_cleanup();

}

