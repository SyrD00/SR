/*Serveur concurrent ECHO: Gestion des processus dans une application client-serveur

Dans cette architecture, le serveur est un processus “veilleur” qui attend les demandes de connexion
des clients. Lorsqu’une demande de connexion arrive, le serveur crée un nouveau processus “exécutant”
pour gérer la demande.

2.1 Création dynamique de processus
Dans cette méthode, les processus exécutants sont créés dynamiquement en utilisant la fonction fork()
Chaque nouveau processus exécutant est responsable de gérer une demande de connexion spécifique, en 
lisant les données envoyées par le client et en y répondant.

NB:
Il est important de noter que les processus exécutants sont lancés en arrière-plan et passent à
 l’état “zombie” lorsqu’ils se terminent. 

Solution pour l'eviter:

Pour éviter que ces processus zombies ne restent en mémoire et ne consomment des ressources système inutilement, le veilleur doit les éliminer 
régulièrement en utilisant le traitant du signal SIGCHLD. Ce traitant permet au veilleur de détecter la fin des processus exécutants et de libérer les ressources associées.

*/
/*
 * echoserverd.c - A concurrent echo server (using fork)
 */

#include "csapp.h"

#define MAX_NAME_LEN 256
/**
 * *Facultatif
 *?la déclaration anticipée de la fonction echo() permet d’éviter une erreur de compilation 
 *?“undefined function” (fonction non définie) lors de l’appel à la fonction echo() dans la fonction
 *?  main().
 */
void echo(int connfd);


/*
void sigchld_handler(int sig) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Child process %d terminated\n", pid);
    }
}

*/
/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */





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

    /* Installation du signal handler pour SIGCHILD */
    //Signal(SIGCHLD, sigchld_handler);


    /**
     * !Le processus parent, quant à lui, continue d’attendre de nouvelles demandes de connexion en
    *!utilisant la boucle while(1)
    */
    while (1) {

        connfd = accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* determine the name of the client */
        Getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);

        /* determine the textual representation of the client's IP address */
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                  INET_ADDRSTRLEN);

        printf("server connected to %s (%s)\n", client_hostname,
               client_ip_string);


        /**
         **Chaque fois qu’un client se connecte, le serveur crée un nouveau processus en utilisant
         ** la fonction fork(). Ce processus enfant gère la demande de connexion spécifique du client
         ** en lisant les données envoyées par le client et en y répondant.
         * 
         */
         
       

        // fork a new process to handle the client request 
        if (Fork() == 0) {
            /**
             *!le processus enfant doit fermer la socket d’écoute (variable listenfd) avant de 
             *!commencer à communiquer avec le client( comme ca le serveur ne peut plus accepter de nouvelles
             *!connexions ), sinon il risque de créer des conflits lors de la gestion des connexions.
             * 
             */
            Close(listenfd);
            echo(connfd);
            /**
             *!Une fois que la fonction echo() a fini de traiter les données envoyées par le client. 
             *!la socket de connexion est fermée en appelant la fonction Close(connfd). 
             */
            Close(connfd);
            exit(0);
        }
        /**
         *!Pour éviter les processus zombies, le parent doit fermer les sockets des connexions devenus
         *! inutiles après avoir créé un nouveau processus enfant pour les gérer.
         */
        Close(connfd);
    }
    exit(0);
}

//A