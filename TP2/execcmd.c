#include "csapp.h"
/*Écrire un programme execcmd qui exécute une commande Unix qu’on lui passe en para-
mètre
 Exemple d’invocation :
execcmd /bin/ls -Ft /
*/
int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Usage: execcmd <commande> <arguments>\n");
        exit(0);
    }
    //Exécuter la commande
    //execv(argv[1],argv+1);
    execvp(argv[1],argv);//plus besoin de spécifier le chemin du programme,il le cherche automatiquement
    //Quel est le probléme d'écrire argv a la place de argv+1?CAR argv[0](argv) contient le nom du programme tandique argv+1 contient les arguments
    
}
/*Reprendre le programme ci dessus qui exécute une commande Unix qu’on lui passe en
paramètre et faites en sorte à ce qu’il cherche automatiquement l’emplacement du fichier
exécutable correspondant.

*/
