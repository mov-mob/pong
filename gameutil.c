#include <stdio.h>
#include <stdlib.h> //random()
#include <locale.h> //idioma

#include <unistd.h>	//  POSIX flags
#include <time.h>	//  clock_gettime(), time()
#include <sys/time.h> //	gethrtime(), gettimeofday()

#include <ncurses.h> //entrada/salida de consola


#include <sys/select.h> //para definir kbhit

int kbhit(void);
double GetRealTime();

int kbhit(void)
{
    struct timeval tv;
    fd_set read_fd;

    /* Do not wait at all, not even a microsecond */
    tv.tv_sec=0;
    tv.tv_usec=0;

    /* Must be done first to initialize read_fd */
    FD_ZERO(&read_fd);

    /* Makes select() ask if input is ready:
    * 0 is the file descriptor for stdin */
    FD_SET(0,&read_fd);

    /* The first parameter is the number of the
    * largest file descriptor to check + 1. */
    if(select(1, &read_fd,NULL, /*No writes*/NULL, /*No exceptions*/&tv) == -1)
        return 0; /* An error occured */

    /* read_fd now holds a bit map of files that are
    * readable. We test the entry for the standard
    * input (file 0). */

    if(FD_ISSET(0,&read_fd))
    /* Character pending on stdin */
        return 1;

    /* no characters were pending */
    return 0;
}

void Delay(double seconds){
    double startTime,endTime;
    startTime=GetRealTime();
    endTime=GetRealTime();
    do{
        endTime=GetRealTime();
    }while((endTime-startTime)<seconds);
}

double GetRealTime()
{
        struct timespec ts;
		const clockid_t id = CLOCK_MONOTONIC_RAW;

		if ( id != (clockid_t)-1 && clock_gettime( id, &ts ) != -1 )
			return (double)ts.tv_sec + (double)ts.tv_nsec / 1000000000.0;
        else
            return (double) -1.0;

}
