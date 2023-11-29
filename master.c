#include <stdio.h> 
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h> 
#include <string.h> 
#include <fcntl.h> 
#include <sys/stat.h> 
#include <sys/types.h> 
#include <sys/select.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>
#include <stdarg.h>


/* Function for write into logfile */
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
    if (fclose(logfile) < 0){
        perror("fclose logfile");
    }
}

/*This function do an exec in child process*/
int spawn(const char * program, char ** arg_list) {
 
  pid_t child_pid = fork();
  if (child_pid != 0)
    //main process
    return child_pid;

   else {
    //child process
    if (execvp (program, arg_list) == -1){
        perror("exec failed");
        return 1;
    }
 }
}

int main() {
    /* The master spawn all the process with watchdog at the end, so it can pass in argv all the process pid. 
    After throught pipe it will pass at all process the watchdog pid.
     */

    //Inizialize the log file, inizialize with mode w, all the data inside will be delete
    FILE *logfile = fopen("logfile.txt", "w");
    if(logfile < 0){ //if problem opening file, send error
        perror("fopen: logfile");
        return 2;
    }else{
        //wtite in logfile
        time_t current_time;
        //obtain local time
        time(&current_time);
        fprintf(logfile, "%s => create MASTER with pid %d\n", ctime(&current_time), getpid());
        if(fclose(logfile) < 0){
            perror("fclose logfile");
        }
    }

    int pipe_fd[2]; // creazione dell'array per le pipe
    if((pipe(pipe_fd)) < 0){
        perror("errore creazione pipe");
    }

    char str_pipe_fd[2][20]; // conversione fd pipe in stringhe
    for (int i = 0; i < 2; i++)
    {
        sprintf(str_pipe_fd[i], "%d", pipe_fd[i]);
    }
    
    

    //now are implemented 3 processes, server, drone, input plus watchdog

    //inizialize the variabiles needed
    int num_ps = 3;       
    pid_t child_pids [num_ps +1];
    int i;

    //this array will need for convert the pisds number in string
    char child_pids_str [num_ps][80];


    //server process
    char * arg_list_server[] = {"konsole", "-e","./server",NULL};
    child_pids[0] = spawn("konsole", arg_list_server);
    
    //drone process -----------------------------------------------------------------------
    char * arg_list_drone[] = {"konsole", "-e","./drone",  str_pipe_fd[0], str_pipe_fd[1],NULL};
    child_pids[1] = spawn ("konsole", arg_list_drone);

    //keyboard_namager process ------------------------------------------------------------
    char * arg_list_i[] = {"konsole", "-e","./input", str_pipe_fd[0], str_pipe_fd[1], NULL};
    child_pids[2] = spawn ("konsole", arg_list_i);

    FILE *initPid = fopen("pid.txt", "w");
    if(initPid < 0){
        perror("fopen initPid");
    }
    if (fclose(initPid) < 0){
        perror("fclose initPid");
    }
    printf("pippo");
    fflush(stdout);
    sleep(1);
    //now need to convert all the integer pid in a string, than pass this string as a argv to watchdog process
    //convert all the pid process fron int to string using sprintf
    for(int i = 0; i < num_ps; i++){   
        sprintf(child_pids_str[i], "%d", child_pids[i]);
    }

    // spawn watchdog, and pass as argument all the pid of processes
    char * arg_list_wd[] = {child_pids_str[0], child_pids_str[1], child_pids_str[2], NULL};
    child_pids[3] = spawn ("./wd", arg_list_wd);

printf("pippo1");
    fflush(stdout);
    
    pid_t waitResult;
    int status;
    for (i = 0; i <= num_ps; i++)
    {
        waitResult = waitpid(child_pids[i], &status, 0);
        if(waitResult == -1){
            perror("waitpid:");
            return 3;
        }
        if (WIFEXITED(status)) {
            printf("Process %d is termined with status %d\n", i, WEXITSTATUS(status));
        } else {
            printf("Iprocess %d is termined with anomaly\n", i);
        }
    }
    
    /*
    while (1)
    {   
        if(wait(NULL) == -1){
            break;
        }
    }
    */

    return 0;
}
