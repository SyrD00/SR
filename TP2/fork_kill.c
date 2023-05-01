#include "csapp.h"
int main() 
{
    int i;
    pid_t pid;
    
    for (i = 0; i < 2; i++) 
        if ((pid = Fork()) == 0) {  /* child */
            while(1);
        }
    
    Kill(-getpid(), SIGTERM);
    
    exit(0);
}
