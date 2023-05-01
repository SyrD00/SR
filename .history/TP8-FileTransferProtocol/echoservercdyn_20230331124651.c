

int main(int argc, char **argv) {
    int listenfd, *connfdp, port;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    pthread_t tid;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        exit(0);
    }
    
    port = atoi(argv[1]);

    /* Initialisation de listenfd et port */
    listenfd = Open_listenfd(port);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfdp = malloc(sizeof(int));
        *connfdp = Accept(listenfd, (SA *)&clientaddr, &clientlen);

        /* Créer un nouveau thread pour gérer la connexion entrante */
        pthread_create(&tid, NULL, echo_thread, connfdp);
    }
    exit(0);
}
