//Ecrire un programme qui ne se termine pas (boucle infinie)

#include "csapp.h"

int main(int argc, char *argv[]){
    while(1);
}
/*LOrsqu'on éxécute ce programme et on lance ctrl c et puis on tape ps
Le programme boucle infini n'apparait pas,ctrlc l'efface de la mémoire et on ne peut plus l'exécuter
Tandique que ctrl z le met en pause et on peut le relancer avec fg

*/

/*Différence entre fg et bg,fg met le processus en premier plan tandique bg le met en arrière plan
FG=foreground
BG=background

*/