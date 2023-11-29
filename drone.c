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

<<<<<<< HEAD
=======
#define INP_NUM 8

>>>>>>> d78f10c8511c433f11a7681679c387378faf3b55
/* function for write in logfile*/
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
    vfprintf(logfile, format, args);

    va_end(args);
    fflush(logfile);
}

/* signal handler functions, when receive a ignal from watchdog sena bach a signal*/
void sigusr1Handler(int signum, siginfo_t *info, void *context)
{
    if (signum == SIGUSR1)
    {
        /*send a signal SIGUSR2 to watchdog */

        kill(info->si_pid, SIGUSR2);
    }
}

// dichiarazioni funzioni per la dinamica
double *generate_input_vect(double **vect_pointer, char ch); // trasformare input in forza
double *put_vector_to_zero(double **vect_pointer);           // inizializzazione vettore input

double *generate_x_force(double *vect_pointer, double *force); // somma input nella forza X
double *generate_y_force(double *vect_pointer, double *force); // somma input nella forza Y

double *velocity(double Force, double initial_velocity, double *new_vel);     // data una forza calcola velocità sull'asse
double *position(double Velocity, double initial_position, double *new_pose); // data una velocità calcola posizione sull'asse

int main(int argc, char *argv[])
{

    pid_t drone_pid = getpid();
    // write into logfile
    writeLog("spawn DRONE with pid %d", drone_pid);

    // write the pid inside the pid file
    FILE *initPid = fopen("pid.txt", "a");
    if (initPid < 0)
    {
        perror("fopen initPid:");
    }
    if (fprintf(initPid, "%i ", drone_pid) < 0)
    {
        perror("fprintf initPid");
    }
    if (fclose(initPid))
    {
        perror("fclose initPid");
    }

    // configure the handler for sigusr1
    struct sigaction sa_usr1;
    sa_usr1.sa_sigaction = sigusr1Handler;
    sa_usr1.sa_flags = SA_SIGINFO;

    if (sigaction(SIGUSR1, &sa_usr1, NULL) == -1)
    {
        perror("sigaction");
        return -1;
    }

    int pipe_fd[2]; // recupero dall'argv i file descriptor delle pipe
    for (int i = 1; i < argc; i++)
    {
        pipe_fd[i - 1] = atoi(argv[i]);
    }
    printf("valore fd controllo(s,l): %d, %d\n", pipe_fd[1], pipe_fd[0]);
    fflush(stdout);

    close(pipe_fd[1]);

    // inizializzazione delle variabili per la dinamica
<<<<<<< HEAD
    double *input_vect = malloc(sizeof(double) * 8); // riservo la memoria per il vettore di input
    for (int i = 0; i < 8; i++)
=======
    double *input_vect = malloc(sizeof(double) * INP_NUM); // riservo la memoria per il vettore di input
    for (int i = 0; i < INP_NUM; i++)
>>>>>>> d78f10c8511c433f11a7681679c387378faf3b55
    {
        input_vect[i] = 0.0;
        printf("%.2f  ", input_vect[i]);
        fflush(stdout);
    }

    double XForce = 0.0, YForce = 0.0;
    double *XForce_p = &XForce, *YForce_p = &YForce;

    double Xvel = 0.0, Yvel = 0.0, Xpos = 0.0, Ypos = 0.0;
    double *Xvel_p = &Xvel, *Xpos_p = &Xpos, *Yvel_p = &Yvel, *Ypos_p = &Ypos;

    int retVal_read;
    char ch;

    // definizione variabili per la select
    int retVal_sel;
    fd_set read_fd;
    struct timeval time_sel;

    // ciclo infinito per ricever input dalla tastiera
    while (1)
    {
        // ridefinisco ad ogni ciclo --> azione select retVal_sel == 0
        FD_ZERO(&read_fd);
        FD_SET(pipe_fd[0], &read_fd); // definisco il set dei fd da controllare

        time_sel.tv_sec = 0; // timeout settatto a 0.5 secondi
        time_sel.tv_usec = 500000;

        if ((retVal_sel = select(pipe_fd[0] + 1, &read_fd, NULL, NULL, &time_sel)) < 0)
        {
            perror("errore select: "); // controllo errori
        }
        else if (retVal_sel == 0)
        {
            printf("no new data\n"); // pipe vuota
            fflush(stdout);
        }
        else
        { // nuovi dati disponibili
            if ((retVal_read = read(pipe_fd[0], &ch, 1)) < 0)
            {
                perror("errore read"); // controllo errore read
            }
            else
            {
                printf("controllo lettura: %d, ( %c )\n", retVal_read, ch); // controllo valori letti
                fflush(stdout);
            }
<<<<<<< HEAD
            input_vect = generate_input_vect(&input_vect, ch); // gli passo il valore di input ricevuto e scrivo nel vettore
=======
            input_vect = generate_input_vect( &input_vect, ch ); // gli passo il valore di input ricevuto e scrivo nel vettore
>>>>>>> d78f10c8511c433f11a7681679c387378faf3b55
            for (int i = 0; i < 8; i++)
            {
                printf("%.2f ", input_vect[i]);
            }
            printf("\n");
            fflush(stdout);
        }

        // genero valori di forza sui due assi
        XForce_p = generate_x_force(input_vect, XForce_p);
        YForce_p = generate_y_force(input_vect, YForce_p);

        // genera velocità
        Xvel_p = velocity(*XForce_p, *Xvel_p, Xvel_p);
        Yvel_p = velocity(*YForce_p, *Yvel_p, Yvel_p);
        // printf("controllo valori: yf:%.2f, yVel:%.2f\n", *YForce_p, *Yvel_p);
        fflush(stdout);

        // genera posizione
        Xpos_p = position(*Xvel_p, *Xpos_p, Xpos_p);
        Ypos_p = position(*Yvel_p, *Ypos_p, Ypos_p);

        // invia posizione a server (mappa)
        printf("forVal(x,y):%.2f, %.2f---velVal(x,y):%.2f, %.2f---posVal(x,y):%.2f, %.2f\n", *XForce_p, *YForce_p, *Xvel_p, *Yvel_p, *Xpos_p, *Ypos_p);
        fflush(stdout);
    }

    return 0;
}

double *generate_input_vect(double **vect_pointer, char ch)
{
    switch (ch)
    {
    case 'w':
        (*vect_pointer)[7] = (*vect_pointer)[7] + 1;
        break; // per ogni possibile input aumento il valore di un vettore
    case 'e':
        (*vect_pointer)[0] = (*vect_pointer)[0] + 1;
        break; // caso 'q' --> programma termina o si blocca
    case 'r':
        (*vect_pointer)[1] = (*vect_pointer)[1] + 1;
        break; // caso default da definire
    case 'f':
        (*vect_pointer)[2] = (*vect_pointer)[2] + 1;
        break;
    case 'v':
        (*vect_pointer)[3] = (*vect_pointer)[3] + 1;
        break;
    case 'c':
        (*vect_pointer)[4] = (*vect_pointer)[4] + 1;
        break;
    case 'x':
        (*vect_pointer)[5] = (*vect_pointer)[5] + 1;
        break;
    case 's':
        (*vect_pointer)[6] = (*vect_pointer)[6] + 1;
        break;
    case 'd':
        *vect_pointer = put_vector_to_zero(&(*vect_pointer));
        break;
    case 'q':
        exit(1);
    default:
        break;
    }

    return *vect_pointer;
}

double *put_vector_to_zero(double **vect_pointer)
{
<<<<<<< HEAD
    for (int i = 0; i < 8; i++)
=======
    for (int i = 0; i < INP_NUM; i++)
>>>>>>> d78f10c8511c433f11a7681679c387378faf3b55
    {
        (*vect_pointer)[i] = 0.0; // inizializzazione
    }
    return *vect_pointer;
}

double *generate_x_force(double *vect_pointer, double *force)
{
    // ogni elemento rapprensenta una direzione per il drone
    *force = vect_pointer[2] - vect_pointer[6] + vect_pointer[1] / 2.0 + vect_pointer[3] / 2.0 - vect_pointer[7] / 2.0 - vect_pointer[5] / 2.0;
    return force;
}

double *generate_y_force(double *vect_pointer, double *force)
{
    *force = vect_pointer[0] - vect_pointer[4] + vect_pointer[1] / 2.0 - vect_pointer[3] / 2.0 + vect_pointer[7] / 2.0 - vect_pointer[5] / 2.0;
    return force;
}

// le due seguenti funzioni eseguono un integrazione numerica approssimata forza(accelerazione)->velocità->posizione
double *velocity(double Force, double initial_velocity, double *new_vel)
{
    double dtime_m = 1, frict_k = -0.1;

    // printf("in -- %.2f + %f * (%.2f + %f * %.2f) --- %.2f\n", initial_velocity, dtime_m, Force, frict_k, initial_velocity, *new_vel);
    *new_vel = initial_velocity + dtime_m * (Force + frict_k * initial_velocity);
    // printf("out-- %.2f --\n", *new_vel);
    return new_vel;
}

double *position(double Velocity, double initial_position, double *new_pose)
{
    double dtime = 0.1;

    *new_pose = initial_position + dtime * Velocity;
    return new_pose;
}
