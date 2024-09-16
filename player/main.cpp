#include "player.h"
#include <thread>
#include <filesystem>
#include <sys/stat.h>
#include "mouse.h"
#include <chrono>


namespace fs = std::filesystem;

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
        randNum = rand()%(trackCount - 1);
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
        cout << "EMC PLAYER => Next track selected : " << selection << endl;
        string cmd = ( "ln -s \"" + selection + "\" " + std::to_string(nextSlot) );
	system(cmd.c_str());
    }
    cout << "EMC PLAYER => Trac count : " << std::to_string(trackCount) <<
	    " Random number : " << std::to_string(randNum) <<
	    " Next slot : " << std::to_string(nextSlot) <<
	    " Selection : " << selection;
}

int main()
{
    int nextFile, currFile = 0;
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
	else if ( clicks == 2 )
	{
	    cout << "EMC PLAYER => Selecting random track.." << endl;
	    selectRandomTrack(currFile);
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    }

    mouse_cleanup();

}

