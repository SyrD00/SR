/*
 * echoserveri.c - An iterative echo server
 */



#include "csapp.h"
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_NAME_LEN 256

void echo(int connfd);

/* 
 * Note that this code only works with IPv4 addresses
 * (IPv6 is not supported)
 */
int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;

    // ajoutons une nouvelle variable pour stocker l’adresse et le port du serveur
    struct sockaddr_in serveraddr;


    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);
    //Le code initialise la taille des informations sur le client en utilisant la fonction “sizeof”.
    clientlen = (socklen_t)sizeof(clientaddr);


    /*ouvre un socket d’écoute en utilisant la fonction “Open_listenfd” fournie par la bibliothèque “csapp.h”. 
    Cette fonction crée un socket d’écoute sur le numéro de port spécifié et retourne le descripteur de fichier du socket d’écoute.*/
    listenfd = Open_listenfd(port);

    /*fonction “getsockname” pour obtenir l’adresse et le port du socket du serveur*/
    getsockname(listenfd, (SA *) &serveraddr, &clientlen);
    printf("Serveur connecté au port %d\n", ntohs(serveraddr.sin_port));
    while (1) {
        /*Accepte une connexion sur le socket d’écoute et retourne le descripteur de fichier du 
        socket de connexion. Cette fonction bloque jusqu’à ce qu’une connexion soit acceptée. 
        L’adresse du client est stockée dans la structure “clientaddr” et la taille de cette 
        structure est stockée dans la variable “clientlen”.*/

        connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        /* determine the name of the client */
        /*Récupère le nom du client à partir de son adresse IP. Cette fonction bloque jusqu’à ce
         que le nom du client soit récupéré.
         La fonction stocke le nom d’hôte dans la variable 
         “client_hostname”.*/
        getnameinfo((SA *) &clientaddr, clientlen,
                    client_hostname, MAX_NAME_LEN, 0, 0, 0);
        
        /* determine the textual representation of the client's IP address */
        /* la fonction “Inet_ntop” pour convertir l’adresse IP du client en une chaîne de caractères
         lisible par l’homme. La fonction stocke la chaîne de caractères dans la variable
          “client_ip_string”.*/
        Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                  INET_ADDRSTRLEN);
        
        /* affiche le nom d’hôte et l’adresse IP du client sur la sortie standard.*/
        //printf("server connected to %s (%s)\n", client_hostname,client_ip_string);

        printf("server connected to %s (%s) on port %d\n", client_hostname,
               client_ip_string, ntohs(clientaddr.sin_port));
        
        getsockname(connfd, (SA *) &serveraddr, &clientlen);//fonction “getsockname” pour obtenir l’adresse et le port du socket du serveur
        getpeername(connfd, (SA *) &clientaddr, &clientlen);//fonction “getpeername” pour obtenir l’adresse et le port du socket du client
        printf("Server socket: IP %s port %d | Client socket: IP %s port %d\n",
               inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port),
               inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

    // inet_ntoa(serveraddr.sin_addr) : convertir l’adresse IP du serveur en une chaîne de caractères lisible par l’homme.
    // ntohs(serveraddr.sin_port) : convertir le numéro de port du serveur en un entier.
    //Idem pour le client.





        /* la fonction “echo” pour traiter la connexion entrante en utilisant le descripteur de
         fichier du socket de connexion.*/
        echo(connfd);
        /*Une fois que la fonction “echo” a terminé de traiter la connexion, le code ferme le socket
         de connexion en utilisant la fonction “Close”.*/
        Close(connfd);
    }
    /*Le code se termine en appelant la fonction “exit”.*/
    exit(0);
}
/*En résumé, le code “echoserveri.c” utilise des sockets en mode connecté pour créer un serveur Echo
 itératif qui écoute les connexions entrantes sur un socket d’écoute et traite chaque connexion en 
 appelant la fonction “echo”. La fonction “echo” lit les données envoyées par le client à travers 
 le socket de connexion, les renvoie au client et répète le processus jusqu’à ce que la connexion 
 soit fermée.
 CTRL+D pour fermer la connexion.
 */

