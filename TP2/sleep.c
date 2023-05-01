/*n rappelle que la primitive unsigned int sleep(unsigned int t) suspend le processus
appelant pendant t secondes ou jusqu’à l’arrivée d’un signal. Dans ce dernier cas, elle
renvoie le nombre de secondes qui restaient à attendre.
Écrire un programme qui suspend le processus pendant un nombre de secondes passé en pa-
ramètre, et imprime le nombre de secondes effectivement passées à attendre, le programme
pouvant être interrompu par control-C
*/
#include "csapp.h"
int main(int argc,char *argv[]){
    if(argc < 2){
        printf("Usage: sleep <seconds>\n");
        exit(0);
    }
  
    int seconde=atoi(argv[1]);
    
    printf("Il reste %d secondes à attendre\n",sleep(seconde));
    exit(0);
}