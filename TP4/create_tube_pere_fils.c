/*Ecrire un programme qui crée un tube, puis un fils (rappelons que le fils
hérite des descripteurs du père). Le père doit fermer la sortie du tube, puis effectuer une
boucle de lecture sur le clavier. Chaque ligne lue est envoyée en entrée du tube. Cette
boucle s’arrête lorsque l’utilisateur presse ctrl-d.
Le fils doit fermer l’entrée du tube, puis effectuer une boucle de lecture sur la sortie du
tube. Chaque ligne lue doit être affichée à l’écran (les lignes sont donc affichées progressivement, une par une). Cette boucle s’arrête lorsque l’on détecte que l’entrée du tube a
été fermée par le père.
Utiliser les primitives de lecture et d’écriture définies dans la bibiothèque
csapp.c i.e. Pipe, Read, Write, etc.
*/
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "csapp.h"
int main() {
    //Création d'un tube:un tube a toujours une entrée et une sortie
    int tube[2];//tube[0] est la sortie du tube et tube[1] est l'entrée' du tube
    if (pipe(tube) == -1) {
        fprintf(stderr, "Erreur de création du tube\n");
        exit(1);
    }

    //Création d'un fils
    pid_t pid;
    int m;
    pid = fork();

    //Le pere va fermer la sortie du tube car lui ne va pas lire(entrée) dans le tube(ce sera le fils) par contre il va écrire dans le tube
    if (pid != 0) {
        close(tube[0]);
    }
    //On effectue une boucle de lecture sur le clavier
    char buffer[100];
   //Tant que on a pas appuyé sur ctrl+d
    while (m=read(STDIN_FILENO, buffer, 100) != 0) {
        
        //On écrit dans le tube ce qu'on a lu sur l'entrée standart(on l'a stocké dans buffer)
        if (pid != 0) {
            write(tube[1], buffer, 100);
        }
        //LE fils doit ensuite fermer l'entrée du tube car lui il va lire(sortie) dans le tube
        else {
            close(tube[1]);
        }
        //Tant que le pére n'a pas fermé l'entrée du tube
        while (m=read(tube[1], buffer, 100) != 0){
        
            //On affiche ce qu'on a lu dans le tube
            printf("%s", buffer);
        
        }
    }

/*Question:

On reprend le principe de la question précédente en considérant trois processus. Le père crée un tube, puis deux fils. Le fils 1 envoie dans le tube ce qu’il lit au
clavier, et le fils 2 affiche à l’écran ce qu’il lit dans le tube.
Le père doit se terminer (et rendre la main au shell) après la fin de ses deux fils.
Quelles sont les conditions d’arrêt pour chacun des fils ? Quelle précaution particulière
faut-il prendre par rapport à celle du fils 2 ?

Condition d'arret du fils 1: quand l'utilisateur a appuyé sur ctrl+d,le fils 1 ne peut plus lire sur l'entrée standart et donc il doit s'arrêter.
Condition d'arret du fils 2: quand le fils 1 a fermé l'entrée du tube,le fils 2 ne peut plus lire dans le tube et donc il doit s'arrêter.

    */