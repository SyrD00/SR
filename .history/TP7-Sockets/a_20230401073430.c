#include "csapp.h"
int listenfd;
#define MAX_NAME_LEN 256
#define NPROC 4 // nombre de processus exécutants

void echo(int connfd);

/*ferme le socket d’écoute et envoie un signal SIGTERM à tous les processus enfants en utilisant la fonction Kill*/
void sigint_handler(int sig) {
    printf("Server received SIGINT, closing connections and terminating.\n");
    Close(listenfd);
    Kill(0, SIGTERM); // Envoie SIGTERM à tous les processus enfants
    exit(0);
}

/*Cette fonction est appelée lorsque le processus reçoit le signal SIGTERM. Elle affiche un message à 
l’utilisateur et ferme le socket d’écoute (listenfd) avant de quitter le processus.
SIGTERM est installé pour les processus enfants afin qu’ils puissent terminer proprement leur exécution avant
 de se terminer eux-mêmes.
*/
void sigterm_handler(int sig) {
    printf("Received SIGTERM signal, closing server...\n");
    Close(listenfd);
    exit(0);
}

void sigchld_handler(int sig) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Child process %d terminated\n", pid);
    }
}


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
  
    
      /* Installation des signal handlers pour SIGCHILD, SIGINT et SIGTERM */
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Signal(SIGTERM, sigterm_handler);










    for (int i = 0; i < NPROC; i++) {
        if (Fork() == 0) {
            // Code pour le client
            char *server_host = "localhost";
            int server_port = 8080;
            int sockfd;
            struct sockaddr_in serveraddr;

            // Création du socket client
            sockfd = Socket(AF_INET, SOCK_STREAM, 0);

            // Remplissage de la structure de l'adresse du serveur
            bzero((char *)&serveraddr, sizeof(serveraddr));
            serveraddr.sin_family = AF_INET;
            serveraddr.sin_port = htons(server_port);
            Inet_pton(AF_INET, server_host, &serveraddr.sin_addr);

            // Connexion au serveur
            Connect(sockfd, (SA *)&serveraddr, sizeof(serveraddr));

            // Envoi de données au serveur
            char sendline[MAXLINE];
            sprintf(sendline, "Hello from client %d", i);
            Write(sockfd, sendline, strlen(sendline));

            // Lecture de la réponse du serveur
            char recvline[MAXLINE];
            int n = Read(sockfd, recvline, MAXLINE-1);
            recvline[n] = '\0';
            printf("Received from server: %s\n", recvline);

            // Fermeture de la connexion au serveur
            Close(sockfd);

            // Fermeture du fils
            exit(0);
        }
    }
