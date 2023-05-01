/*
 * echoclient.c - An echo client
 */
#include "csapp.h"

int main(int argc, char **argv)
{
    /*déclarer des variables pour le descripteur de fichier du socket de client, le numéro de port, 
    l’adresse du serveur et un tampon pour stocker les données reçues du serveur.


    */
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;

    if (argc != 3) {
        fprintf(stderr, "usage: %s <host> <port>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
    port = atoi(argv[2]);

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */

    /* créer un socket de client et se connecter au serveur spécifié en utilisant l’adresse IP ou le
    nom d’hôte et le numéro de port. La fonction retourne le descripteur de fichier du socket de 
    client.
    */
    clientfd = Open_clientfd(host, port);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */

    //affiche un message pour indiquer que la connexion a été établie.
    printf("client connected to server OS\n"); 
    
    /* la fonction “Rio_readinitb” pour initialiser une structure “rio_t” à utiliser pour lire des
     données à partir du socket de client. Cette structure est utilisée pour effectuer la lecture 
     et l’écriture en tamponnant les données c-à-d. 
    */
    Rio_readinitb(&rio, clientfd);

    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        Rio_writen(clientfd, buf, strlen(buf));
        if (Rio_readlineb(&rio, buf, MAXLINE) > 0) {
            Fputs(buf, stdout);
        } else { /* the server has prematurely closed the connection */
            break;
        }
    }
    Close(clientfd);
    exit(0);
}
