#include "player.h"
#include <filesystem>
#include <iostream>
#include <cstring>

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
	//system(strcat("mpg123 ",filename.c_str()));
	string cmd = "ffmpeg -loglevel fatal -i " + filename + " -f wav - | " + \
		" pacat --device=bluez_sink.F4_78_F3_96_0C_CD.a2dp_sink";
	cout << "EMC PLAYER => Command : " << cmd.c_str() << endl;
	system(cmd.c_str());
	running = false;
}

bool Player::isTrackRunning( void )
{
	return running;
}

}
