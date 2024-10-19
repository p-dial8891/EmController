#include <string>
#include <unistd.h>

int main ( int argc , char * argv[] ) 
{
    //std::string cmd1 = "ffplay -v 0 -nodisp -autoexit "; 
    //std::string cmd2 = "\"-v 0 -nodisp -autoexit\""; 
    //std::string cmd3 = argv[1];
    //if ( setegid(1000) )
    //{
    //   perror("EMC PLAY => unable to setgid\n");
    //   exit(1);
    //}
    //if ( seteuid(1000) )
    //{
    //   perror("EMC PLAY => unable to setuid\n");
    //   exit(1);
    //}
    //execl("ffplay", "ffplay", "-v", "0", "-nodisp", "-autoexit", argv[1]);
    if ( execl("/usr/bin/ffplay", "ffplay", "-v", "0",  "-nodisp", "-autoexit", argv[1], NULL) )
        perror("EMC PLAY => ffplay failed.");

    return 0;
}
