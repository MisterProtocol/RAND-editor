#include <stdio.h>
#include <unistd.h>
#include <signal.h>

void catch_hup();

main()
{
    /*sighandler_t pipesig;*/
    /*void (*pipesig) ();*/
    void (*sig) ();

    /*pipesig = signal (SIGTSTP, SIG_DFL);*/
    /*alarmsig = signal (SIGALRM, catch_alarm);*/
    sig = signal (SIGHUP, catch_hup);
    printf("ok main sig=(%d)\n", sig);
    sleep(20);
}

void
catch_hup( sig )
  int sig;
{
  printf("got hup\n");
}
