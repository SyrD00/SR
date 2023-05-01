/*
 * echoclient.c - An echo client
 */

#include "csapp.h"
#include <sys/socket.h>
#include <netinet/in.h>


int main(int argc, char **argv)
{
    /*déclarer des variables pour le descripteur de fichier du socket de client, le numéro de port, 
    l’adresse du serveur et un tampon pour stocker les données reçues du serveur.
    */
    int clientfd, port;
    char *host, buf[MAXLINE];
    rio_t rio;


     // ajoutons une nouvelle variable pour stocker l’adresse et le port du serveur
    struct sockaddr_in clientaddr, serveraddr;
    socklen_t len;
    

    if (argc != 2) {
        fprintf(stderr, "usage: %s <host>\n", argv[0]);
        exit(0);
    }
    host = argv[1];
 

    /*
     * Note that the 'host' can be a name or an IP address.
     * If necessary, Open_clientfd will perform the name resolution
     * to obtain the IP address.
     */

    /* créer un socket de client et se connecter au serveur spécifié en utilisant l’adresse IP ou le
    nom d’hôte et le numéro de port. La fonction retourne le descripteur de fichier du socket de 
    client.
    */
    clientfd = Open_clientfd(host, 2121);
    
    /*
     * At this stage, the connection is established between the client
     * and the server OS ... but it is possible that the server application
     * has not yet called "Accept" for this connection
     */

    //affiche un message pour indiquer que la connexion a été établie.
    printf("client connected to server OS\n"); 
    

    /*Nous avons ajouté les lignes suivantes pour récupérer les adresses et les ports des sockets du client et du serveur :*/
    len = sizeof(clientaddr);
    getsockname(clientfd, (SA *) &clientaddr, &len);//fonction “getsockname” pour obtenir l’adresse et le port du socket du serveur
    printf("Client socket: IP %s port %d\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

    len = sizeof(serveraddr);
    getpeername(clientfd, (SA *) &serveraddr, &len);
    printf("Server socket: IP %s port %d\n", inet_ntoa(serveraddr.sin_addr), ntohs(serveraddr.sin_port));







    /* la fonction “Rio_readinitb” pour initialiser une structure “rio_t” à utiliser pour lire des
     données à partir du socket de client. Cette structure est utilisée pour effectuer la lecture 
     et l’écriture en tamponnant les données c-à-d tampon=
    */
    Rio_readinitb(&rio, clientfd);
    /* entre dans une boucle qui lit des lignes de données à partir de l’entrée standard et les 
    envoie au serveur en utilisant la fonction “Rio_writen”.
    Si la lecture réussit, le code lit la réponse du serveur à l’aide de la fonction “Rio_readlineb”
    */

    // on lit des lignes de données à partir de l’entrée standard.
    while (Fgets(buf, MAXLINE, stdin) != NULL) {
        //et  les envoie au serveur en utilisant la fonction “Rio_writen”
        Rio_writen(clientfd, buf, strlen(buf));
        //Si le serveur répond,le client lit la réponse du serveur à l’aide de la fonction “Rio_readlineb”
        if (Rio_readlineb(&rio, buf, MAXLINE) > 0) {
            Fputs(buf, stdout);// le code affiche la réponse à l’écran en utilisant la fonction “Fputs”. 
        } else { /* the server has prematurely closed the connection */
            break;//. Si le serveur ferme prématurément la connexion, la boucle se termine.


        }
    }
    Close(clientfd);//on ferme le socket de client.
    exit(0);//on termine le processus.
}
/*n résumé, le code “echoclient.c” utilise des sockets en mode connecté pour créer un client Echo qui se connecte à un serveur et envoie des
 données à travers un socket de client. Le client lit les réponses du serveur à l’aide d’une structure “rio_t” et affiche les réponses à l’écran
  en utilisant la fonction “Fputs”.
*/

/*Lorsque vous lancez le client avant le serveur, le client ne peut pas se connecter au serveur, car le serveur n’est pas encore en écoute sur
 le port spécifié. Vous obtiendrez une erreur de connexion.

 Si on essaye d’accéder au même serveur(par exemple localhost) à partir d’un processus client différent alors qu’un client est déjà connecté au 
 serveur, le 2ieme client ne sera pas en mesure de se connecter, car le serveur n’est configuré que pour accepter une seule connexion à la fois.
  Le 2ieme client recevra une erreur de connexion.
*/