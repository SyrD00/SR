#include "csapp.h"
/* Écrire un programme qui affiche le numéro (PID) du processus qui l’exécute. Que
remarquez-vous en le lançant plusieurs fois ?
*/

int main(){
    //if (fork() != 0) {
       printf("Le PID du processus  est %d\n", getpid());
        printf("Le PPID du processus pére est %d\n", getppid());
   // } else {
     //   printf("Le PID du processus fils  est %d\n", getpid());
    //}
}