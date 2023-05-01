
/*Écrire un programme qui affiche son PID ainsi que l’ensemble des variables définies dans
son environnement.*/
#include "csapp.h"
int main(int argc, char *argv[]){
    //Afficher le pid du programme
    printf("PID: %d\n", getpid());

    //Afficher les variables défini dans l'environnement du programme

    int i = 0;
    while(environ[i] != NULL){
        printf("%s\n", environ[i]);
    }

   
}