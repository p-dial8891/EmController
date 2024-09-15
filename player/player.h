#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>

extern "C" {

using namespace  std;

class Player
{
private:
	bool running;
        string filename;

public:
	Player( string filename );
        void Play( void );
	bool isTrackRunning( void );
};

}
