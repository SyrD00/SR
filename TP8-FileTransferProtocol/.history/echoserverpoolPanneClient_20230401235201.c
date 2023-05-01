#include "csapp.h"

#define MAX_NAME_LEN 256
#define NPROC 2
#define MAX_FILE_PATH_LEN 10000

typedef struct {
    char filename[32];
    off_t offset;
} file_request;


int listenfd;

void sigint_handler(int sig) {
    printf("Server received SIGINT, closing connections and terminating.\n");
    Close(listenfd);
    Kill(0, SIGTERM); // Envoie SIGTERM à tous les processus enfants
    exit(0);
}

void sigterm_handler(int sig) {
    printf("Child process %d received SIGTERM, terminating.\n", getpid());
    exit(0);
}

void sigchld_handler(int sig) {
    pid_t pid;
    int status;
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("Child process %d terminated\n", pid);
    }
}

int main(int argc, char **argv) {
    int connfd;
    socklen_t clientlen;
    struct sockaddr_in clientaddr;
    char client_ip_string[INET_ADDRSTRLEN];
    char client_hostname[MAX_NAME_LEN];
    size_t n;
    rio_t rio;

    if (argc != 1) {
        fprintf(stderr, "usage: %s\n", argv[0]);
        exit(0);
    }

    clientlen = (socklen_t)sizeof(clientaddr);
    listenfd = Open_listenfd(2121);
    Signal(SIGCHLD, sigchld_handler);
    Signal(SIGINT, sigint_handler);
    Signal(SIGTERM, sigterm_handler);

    for (int i = 0; i < NPROC; i++) {
        if (Fork() == 0) {
            while (1) {
                connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
                Getnameinfo((SA *) &clientaddr, clientlen,
                            client_hostname, MAX_NAME_LEN, 0, 0, 0);
                Inet_ntop(AF_INET, &clientaddr.sin_addr, client_ip_string,
                          INET_ADDRSTRLEN);
                printf("server connected to %s (%s)\n", client_hostname, client_ip_string);
                Rio_readinitb(&rio, connfd);

                while (1) {
                    file_request req;
                    if ((n = Rio_readnb(&rio, (char *)&req, sizeof(file_request))) == 0) {
                        printf("No file request received\n");
                        break;
                    }

                

                    char server_file_path[MAX_FILE_PATH_LEN];
                    snprintf(server_file_path, sizeof(server_file_path), "FichiersServeur/%s", req.filename);

                    if (access(server_file_path, F_OK) == -1) {
                        printf("File %s does not exist in the server directory\n", server_file_path);
                        char error_msg[] = "FILE_NOT_FOUND";
                        Rio_writen(connfd, error_msg, strlen(error_msg));
                        break;
                    }

                    int filefd = Open(server_file_path, O_RDONLY, 0);
                    if (filefd == -1) {
                        perror("Error opening file");
                        Close(connfd);
                        continue;
                    }

                    off_t file_offset = req.offset;
                    lseek(filefd, file_offset, SEEK_SET);

                    ssize_t bytes_read;
                    char buf[32];
                    while ((bytes_read = read(filefd, buf, 20)) > 0) {
                        Rio_writen(connfd, buf, bytes_read);
                    }

                    Close(filefd);
                    printf("File %s has been sent to the client.\n", req.filename);
                    Close(connfd);
                }
            }
            exit(0);
        }
    }

    while (1) {
        pause();
    }

    return 0;
}
