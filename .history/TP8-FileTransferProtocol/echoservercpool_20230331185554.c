/**
**Dans la méthode de pool de processus, un certain nombre fixe de processus exécutants est créé à 
*l’avance. Lorsqu’une demande de connexion arrive, elle est traitée par l’un de ces processus, qui se
*remet ensuite en attente. Le veilleur, quant à lui, continue d’attendre de nouvelles demandes de
*connexion en utilisant la boucle while(1).
processus principal (ou veilleur) attend de nouvelles demandes de connexion en utilisant la primitive
 accept, et lorsqu’une demande arrive, il la transfère à l’un des processus exécutants disponibles 
 pour le traitement.

*TODO:Pour faire attendre et activer les exécutants, il y a deux solutions possibles :
*La primitive accept est exécutée par:
*? Le processus principal (le veilleur) qui attend de nouvelles demandes de connexion.
*?dans ce cas, le veilleur doit gérer une file de connexions en attente de traitement. 
*? Lorsqu’une connexion est acceptée, le veilleur doit la placer dans la file et réveiller un
*?processus exécutant disponible  qui sera chargé de la traiter.
*?Cette stratégie permet de traiter plusieurs connexions en parallèle sans avoir besoin de créer un
*? nouveau processus pour chaque connexion.
*? Le processus veilleur ne fait rien de plus que d’attendre des signaux
*? tandis que les processus exécutants restent en attente de nouvelles connexions en utilisant 
*?la primitive accept. 
*?Les connexions sont traitées par les processus exécutants puis les sockets sont fermées.
*!les processus exécutants : dans ce cas, chaque processus exécutant doit attendre une nouvelle 
*!connexion en utilisant la primitive accept. Lorsqu’une nouvelle connexion arrive, le processus la
*! traite et se remet en attente de la prochaine connexion.

**NB:La solution la plus simple à réaliser dépend du contexte d’utilisation. 
*?Si le nombre de connexions est très élevé, il peut être plus efficace d’utiliser la 1er solution 
*?pour éviter d’avoir trop de processus en attente. 
*!Si le nombre de connexions est plus faible, la 2nd solution peut être plus simple à implémenter.



*/




/*
 * echoserverp.c - A concurrent echo server (using process pool)
 */




/**
 * !Dans ce code j'utilise la stratégie:
 *!les processus exécutants attendent une nouvelle connexion en utilisant la primitive accept. 
 *!Lorsqu’une nouvelle connexion arrive, le processus qui est disponible la traite(ici echo) et se remet 
 *!en attente de la prochaine connexio(le w)
 *!Le processus veilleur ne fait rien, il se contente d’attendre des signaux et ne gère pas une
 *! file de connexions en attente de traitement.
*/
#include "csapp.h"

#define MAX_NAME_LEN 256
#define NPROC 4 // nombre de processus exécutants

void echo(int connfd);


int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);


    // créer le pool de processus exécutants
    for (int i = 0; i < NPROC; i++) {
        if (Fork() == 0) {
            while (1) {
                connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);

                /* determine the name of the client */
                Getnameinfo((SA *) &clientaddr, clientlen,
                            client_hostname, MAX_NAME_LEN, 0, 0, 0);

                /* determine the textual representation of the client's IP address */
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                          INET_ADDRSTRLEN);

                printf("server connected to %s (%s)\n", client_hostname,
                       client_ip_string);
                  
                echo(connfd);

                Close(connfd);
            }
            exit(0);
        }
    }

    while (1) {
        pause(); // le processus veilleur ne fait rien, il se contente d'attendre des signaux
    }

    return 0;
}

/*Execute ce programme et ouvre un autre terminal pour tester le serveur.
Tape ps aux | grep echoservercpool pour voir les processus exécutants.:tu verra 4 processus exécutants et 1 processus veilleur(qui est le processus principal)
*/