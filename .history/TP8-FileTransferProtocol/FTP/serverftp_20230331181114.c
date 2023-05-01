

#include "csapp.h"



#define MAX_NAME_LEN 256
int main(int argc, char **argv)
{
    int listenfd, connfd, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];


    char filename[MAXLINE];
    size_t n;
    rio_t rio;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    port = atoi(argv[1]);

    clientlen = (socklen_t)sizeof(clientaddr);

    listenfd = Open_listenfd(port);

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


       

    
        /*Initialisatio de la structure rio en utilisant le descripteur de fichier du socket de connexion connfd.
        Cette structure sera utilisée pour lire les données envoyées par le client.
        */
        Rio_readinitb(&rio, connfd);

        
         /*Le serveur aura besoin de lire le nom du fichier envoyé par le client donc nous allons
        utiliser la fonction “Rio_readlineb” fournie par la bibliothèque “csapp.h”. 
        Cette fonction lit une ligne de texte de taille MAXLINE depuis le descripteur de fichier
         “connfd” et la stocke dans la variable “filename”. Cette fonction bloque jusqu’à ce que le
          client envoie le nom du fichier.*/

        /* read filename from client */

        //n est le nombre de caractères lus
        if ((n = Rio_readlineb(&rio, filename, MAXLINE)) == 0) {
            printf("no filename received\n");
            Close(connfd);//fermer le socket de connexion
            continue;
        }

       
        /*Des que le client tape un nom et va à la ligne ca voudra dire que le nom du fichier est terminé.
        Nous allons donc supprimer le caractère de fin de ligne “\n” de la variable “filename” et 
        remplacer ce caractère par le caractère de fin de chaîne “\0”.
        
        
       Si on lit par exemple tom(donc n=3 caractere lus) alors dans filename on aura 
       indice 0 = t, indice 1 = o, indice 2 = m, indice 3 = \n, indice 4 = \0
       */
        if (filename[n-1] == '\n') {
            filename[n-1] = '\0';
        }

        /* open file in mode read and send contents to client */
        int filefd = Open(filename, O_RDONLY, 0);
        char buf[MAXLINE];
        ssize_t bytes_read;

        //Tout ce qui est lu dans le fichier est stocké dans la variable “buf” et envoyé au client
        while ((bytes_read = read(filefd, buf, MAXLINE)) > 0) {
            /*il faut utiliser la fonction Rio_writen pour envoyer les données au client
            On ecit dans le socket de con
            */
            Rio_writen(connfd, buf, bytes_read);
        }

        Close(filefd);
        Close(connfd);
    }

    return 0;
}
