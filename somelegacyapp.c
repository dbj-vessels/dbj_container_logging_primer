/*
Imagine somelegacyapp is 20+ years old binary, but now your salary provider wants its output to go
to the container log. 

You put the said binary in the image, using e.g alpine:latest in your Dockerfile, then start the container , for example like this

docker run -itd --name mycontainername  imagename

Then go to containers shell and this is how you do it:

./somelegacyapp &>/dev/console -- both STDOUT and STDERR will go to the log
./somelegacyapp 1>/dev/console -- only 1 == STDOUT
./somelegacyapp 2>/dev/console -- only 2 == STDERR

 And then emboldened you might try the following: 
 write 1 and 2 to log.txt and then redirect the log.txt to the container log, and then run the whole as daemon
 
./somelegacyapp &>log.txt>/dev/console &

Note: it is important not to insert spaces above!

*/  
#include <stdio.h>
#include <time.h>
#include <unistd.h> // sleep

int main(void)
{
   int count = 5 ;

   while( count-- ) {

    time_t stamp_ = time(0);

   fprintf(stdout, "%ld STDOUT\r\n", stamp_);
   fflush(stdout);

   fprintf(stdout, "%ld STDERR\r\n", stamp_);
   fflush(stderr);

   sleep(3);

  }

return 0;
}
/*
This is source of somelegacyapp, it is not needed to be rebuilt
binary is alreasy here, but if you have this itch here is how:

Add gcc if not present, here is how: apk add build-base

gcc somelegacyapp.c -o somelegacyapp

*/
