#include "player.h"
#include <filesystem>
#include <iostream>
#include <cstring>
#include "../config/config.h"
#include <unistd.h>

extern "C" {

using namespace std;

Player::Player( string filename )
{
        running = true;
        this->filename = filename;
}

void Player::Play( void )
{
        running = true;	
        cout << "EMC PLAYER => Playing file : " << filename << endl;
#ifdef USE_FFMPEG
	string cmd = "../player/play.out " + filename;
#elif defined(USE_MPG123)
	string cmd = "mpg123 -q --no-control --no-visual " + filename;
#elif defined(USE_PACAT)
	string cmd = "ffmpeg -loglevel fatal -i " + filename + " -f wav - | " + \
		" pacat --device=bluez_sink.F4_78_F3_96_0C_CD.a2dp_sink";
#endif
	cout << "EMC PLAYER => Command : " << cmd.c_str() << endl;
	system(cmd.c_str());
	//execlp(cmd, filename.c_str(), NULL);
	running = false;
}

bool Player::isTrackRunning( void )
{
	return running;
}

}
