#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>
#include <stdlib.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <stdarg.h>

/* number of process that are controlled by watchdog*/
#define PROCESS_NUMBER 3;

void writeLog(const char *format, ...)
{

    FILE *logfile = fopen("logfile.txt", "a");
    if (logfile < 0)
    {
        perror("Error opening logfile");
        exit(EXIT_FAILURE);
    }
    va_list args;
    va_start(args, format);

    time_t current_time;
    time(&current_time);

    fprintf(logfile, "%s => ", ctime(&current_time));
    fprintf(logfile, " ");
    vfprintf(logfile, format, args);

    va_end(args);
    fflush(logfile);
}

/*  when signal arrive counter --
    when wd send kill counter ++ */
int counter = 0;

/*signal hadler function*/
void sigusr2Handler(int signum, siginfo_t *info, void *context)
{
    if (signum == SIGUSR2)
    {
        writeLog("WATCHDOG received signal from %d", info->si_pid);
        counter--;
    }
}

int main(int argc, char *argv[])
{
    // All the process controlled by watchdog
    int num_ps = PROCESS_NUMBER;
    // this array will contain all the process pid readed by regular file
    pid_t pids[num_ps];
    int i; // declared for all the for cycle

    /* configure the handler for sigusr2*/
    struct sigaction sa_usr2;
    sa_usr2.sa_sigaction = sigusr2Handler;
    sa_usr2.sa_flags = SA_SIGINFO; // I need also the info foruse the pid of the process for unde
    if (sigaction(SIGUSR2, &sa_usr2, NULL) == -1)
    {
        perror("sigaction");
        return -1;
    }

    writeLog("the pid of watchdog is %i", getpid());
        /*
        // convert the pid in argv from dtring to int
        for (i = 0; i < argc; i++){
            // convert all the pid form string to int
            pids[i] = atoi(argv[i]);
        }
        */
        /* read the pid from file text*/
        FILE *initPid = fopen("pid_file.txt", "r");
    if (initPid < 0)
    {
        perror("fopen initPid");
        exit(EXIT_FAILURE);
    }
    fscanf(initPid, "%d %d %d", &pids[0], &pids[1], &pids[2]);

    // Chiudi il file
    if (fclose(initPid) < 0)
    {
        perror("fclose initPid");
    }
    // write the pid received on logfile
    writeLog("riceived pid1 %d, pid2 %d, pid3 %d", pids[0], pids[1], pids[2]);

    while (1)
    {

        counter = 0; // Inizialize the counter every time entr in the loop
        /* send a signal to all process */
        for (i = 0; i < argc; i++)
        {
            /* send signal to all process*/
            kill(pids[i], SIGUSR1);
            /* increment the counter when send the signal*/
            counter++;
            sleep(1);
            printf("%d", counter);
            fflush(stdout);
            if (counter == 0)
            {
                /* in this case the proccess is alive*
                /* write into logfile*/
                writeLog("Process %d is alive", pids[i]);
            }
            else
            {
                /*The proces doesn't work*/
                /*kill all process*/
                for (int j = 0; j < argc; j++)
                {
                    kill(pids[j], SIGKILL);
                    /*write into logfile that wd close the process*/
                    writeLog("process %d is closed by WATCHDOG", pids[j]);
                }
                exit(0);
            }
            sleep(5);
        }
    }
    return 0;
}