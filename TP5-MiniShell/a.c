#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    int fd[2];
    char buffer[5];

    // Crée un tuyau
    if (pipe(fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Ferme le côté lecture du tuyau
    close(fd[0]);

    // Écrit dans le tuyau
    if (write(fd[1], "hello", 5) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // Ferme le côté écriture du tuyau
    close(fd[1]);

    // Lit depuis le tuyau
    if (read(fd[0], buffer, 5) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("Message lu : %s\n", buffer);

    return 0;
}
