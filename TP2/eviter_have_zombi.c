#include "csapp.h"

int main() {
    if (fork() != 0) {
        int statut; pid_t fils;
        printf("je suis le père %d, j’attends mon fils\n", getpid());
        fils = wait(&statut);

        if (WIFEXITED(statut)) {
            printf("%d : mon fils %d s'est terminé avec le code %d\n",
            getpid(), fils, WEXITSTATUS(statut)); 
        };
        exit(0);
    } 
    else {
        printf("je suis le fils, mon PID est %d\n", getpid());
        sleep(2); /* blocage pendant 2 secondes */
        printf("fin du fils\n");
        exit(1);
    }
}