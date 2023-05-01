#include "csapp.h"

/*Écrire un programme manbis qui exécute la commande man avec deux arguments qu’on
lui passe en paramètres : le numéro de la section concerné et le sujet recherché. Exemple
d’invocation : manbis 3 printf
On souhaite que la commande man soit exécutée avec certains réglages particuliers :
— La documentation doit être affichée en anglais. Pour cela, la variable d’environnement
LANG doit être positionnée à la valeur en_US (la valeur pour une configuration française
est fr_FR).
— L’affichage du manuel dans le terminal doit substituer après que l’on ait quitté man.
Pour cela, la variable d’environnement MANPAGER doit être définie et être positionnée à
la valeur less -X.
Pour positionner le variables d’environnement, utiliser la fonction putenv*/
int main(int argc, char *argv[]){
    if(argc != 3){
        printf("Usage: manbis <numéro_section> <subject>\n");
        exit(0);
    }
 
  
    putenv("LANG=en_US");
    putenv("MANPAGER=less -X");
    //Exécuter la commande man avec les arguments numero section et subject
    execlp("man",argv[1],argv[2],NULL);
    



}