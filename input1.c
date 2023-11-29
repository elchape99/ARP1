#include <curses.h>
#include <stdarg.h>
#include <time.h>

/* function for write in logfile*/
void writeLog(const char *format, ...) {
    
    FILE *logfile = fopen("logfile.txt", "a");
    if (logfile < 0) {
        perror("Error opening logfile");
        //exit(EXIT_FAILURE);
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

int main() {
    initscr();  // Inizializza ncurses
    cbreak();   // Disabilita il buffering del tastierino (line buffering)
    noecho();   // Non visualizzare i caratteri immessi

    int ch;
    while (1) {
        ch = getch();  // Cattura un tasto premuto

        // Puoi fare qualcosa con il tasto premuto qui
        // Ad esempio, puoi stamparlo a schermo
        printw("Tasto premuto: %c\n", ch);
        refresh();  // Aggiorna la schermata
    printf("%c", ch);
        if (ch == 'q') {
            break;  // Esci se viene premuto il tasto 'q'
        }
    }

    endwin();  // Termina ncurses
    return 0;
}