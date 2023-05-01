
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
