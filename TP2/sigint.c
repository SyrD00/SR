#include "csapp.h"

void handler(int sig) /* SIGINT handler */
{
    printf("Caught SIGINT\n");
    exit(0);
}

int main() 
{
    /* Install the SIGINT handler */
    Signal(SIGINT, handler);
    pause(); /* wait for the receipt of a signal */
    exit(0);
}