/*Lorsqu’un processus crée des processus fils, il peut attendre leur fin avec la primitive
wait ou waitpid. Néanmoins, dans les variantes que nous avons vues, le processus père
est bloqué et ne peut pas faire de travail utile pendant cette attente. C’est pourquoi on
souhaite que le processus père traite la fin de ses fils uniquement au moment où cette fin
est signalée par le signal SIGCHLD.
Une option utile pour éviter au processus père d’être bloqué en attente, alors qu’il n’y
a plus de fils à attendre est WNOHANG. Voici un extrait du man spécifiant l’appel système
waitpid :
"Si waitpid() renvoie en raison d'un processus d'enfant arrêté ou terminé, l'identifiant de processus de
 l'enfant est renvoyé au processus appelant. S'il n'y a pas d'enfants inattendus précédemment, -1 est 
 renvoyé avec errno défini sur [ECHILD]. Sinon, si WNOHANG est spécifié et qu'il n'y a pas d'enfants 
 arrêtés ou sortis, 0 est renvoyé. Si une erreur est détectée ou qu'un signal intercepté annule l'appel,
  une valeur de -1 est renvoyée et errno est défini pour indiquer l'erreur."

Écrire le programme d’un processus créant plusieurs fils et traitant leur fin comme il vient
d’être indiqué.
Attention : tenir compte du fait que les signaux ne sont pas mémorisés (un seul signal d’un
type donné peut être dans l’état pendant, cf. question précédente)
*/

#include <stdio.h>
#include "csapp.h"

int counter=0;

void handler_child(int sig){
    counter++;
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) { //-1 pour attendre la fin d'un processus fils quelconque
      //WNOHANG pour ne pas bloquer le processus père
      //waitpid renvoie 0 si il n'y a pas de fils à attendre
      //waitpid renvoie -1 si il y a une erreur
      //waitpid renvoie le pid du fils si il y a un fils à attendre

        if (WIFEXITED(status)) //Si le fils s'est terminé normalement
            printf("Fils de pid %d s'est terminé normalement avec le code de retour %d", pid, WEXITSTATUS(status));

    }

}



int main(int argc, char *argv[]){
    int i;
   
    
     
    if (argc != 2) {
        //Afficher un message d'erreur pour dirre qu'il faut fournir un fichier en argument sinon on quitte
        perror("Il faut fournir un fichier en argument au programme");
        exit(0);
    }

    Signal(SIGCHLD, handler_child); //On enregistre le handler pour le signal SIGCHLD

    if(Fork() == 0){
       for(i=0; i<5; i++){
          kill(getppid(), SIGCHLD);//kill permet d'envoyer un signal SIGCHILD à un processus (ici le père)
          printf("On a envoyé un signal SIGCHILD au processus père\n");
       }
        exit(0);
       
    }
    Wait(NULL);//peermet d'attendre la fin du fils
    printf("counter=%d\n",counter);
    exit(0);

}
  