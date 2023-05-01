#include "csapp.h"
/*Un processus père peut attendre la fin d’un ou plusieurs fils en utilisant
wait() ou waitpid(). Tant que son père n’a pas pris connaissance de sa
terminaison par l’une de ces primitives, un processus terminé reste dans un
état dit zombi. Un processus zombi ne peut plus s’exécuter, mais consomme
encore des ressources. Il faut éviter de conserver des processus dans cet état.
*/
int main() {
    if (fork() != 0) {
        printf("je suis le père, mon PID est %d\n", getpid());
        while (1) ; /* le pere rentre dans une boucle sans fin sans attendre le fils du coup il ne pourra pas savoir qaund son fils a terminé son exécution donc création de zop*/
    } else {
        printf("je suis le fils, mon PID est %d\n", getpid());
        sleep(2) ;/* blocage pendant 2 secondes */
        printf("fin du fils\n");
        exit(0);
    }
}