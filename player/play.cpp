#include <string>
#include <stdio.h>
#include <unistd.h>

using namespace std;

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
    pid_t pid = getpid();
    string pidStr = std::to_string(pid);
    string filename = "../var/" + pidStr;
    FILE * fp = fopen(filename.c_str(),"w");
    fclose(fp);
    if ( execl("/usr/bin/ffplay", "ffplay", "-v", "0",  "-nodisp", "-autoexit", argv[1], NULL) )
        perror("EMC PLAY => ffplay failed.");
   std::remove(filename.c_str()); 
   return 0;
}
