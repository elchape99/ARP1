#include <ncurses.h> 
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

/* function for write in logfile*/
void writeLog(const char *format, ...) {
    
    FILE *logfile = fopen("logfile.txt", "a");
    if (logfile < 0) {
        perror("Error opening logfile");
        exit(EXIT_FAILURE);
    }
    va_list args;
    va_start(args, format);

    time_t current_time;
    time(&current_time);

    fprintf(logfile, "%s => ", ctime(&current_time));
    vfprintf(logfile, format, args);

    va_end(args);
    fflush(logfile);
    if (fclose(logfile)){
        perror("fclose logfile");
    }
}

void sigusr1Handler(int signum, siginfo_t *info, void *context) {
    if (signum == SIGUSR1){
        /*send a signal SIGUSR2 to watchdog */
        //printf("SERVER sig handler");
        kill(info->si_pid, SIGUSR2);
        printf("server signal handler");
        fflush(stdout);
        
    }
}

WINDOW *create_new_window(int row, int col, int ystart, int xstart); // creazione delle finestre


int main(int argc, char *argv[]) 
{
    pid_t server_pid = getpid();
    //write into logfile
    writeLog("spawn SERVER with pid %d", server_pid);

    /* write the pid inside a file and after the wd will read this pid*/
    FILE *initPid = fopen ("pid.txt", "a");
    if (initPid < 0){
        perror("fopen initPid:");
    } 
    if (fprintf(initPid, "%i ", server_pid) < 0){
        perror("fprintf initPid");
    }
    if (fclose(initPid)){
        perror("fclose initPid");
    }
   

    //configure the handler for sigusr1
    struct sigaction sa_usr1;
    sa_usr1.sa_sigaction = sigusr1Handler;
    sa_usr1.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1){ 
        perror("sigaction");
        return -1;
    }

    // parte legata ad ncurses per il server
    int Srow, Scol;
    char drone_icon = '#';

    double Ypos, Xpos; // variabili per collegare la mem condivisa

    WINDOW *map_window; // inizializzo puntatore per la finestra mappa

    // initialization row
    initscr();raw();cbreak();noecho();keypad(stdscr, TRUE);

    getmaxyx(stdscr, Srow, Scol);
    map_window = create_new_window(Srow, Scol, 0, 0);

    

    
    return 0;
}

WINDOW *create_new_window(int row, int col, int ystart, int xstart){
    WINDOW *local_window = newwin(row, col, ystart, xstart);
    box(local_window, 0, 0);

    wrefresh(local_window);
    return local_window;
}
