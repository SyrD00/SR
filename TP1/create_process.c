#include "csapp.h"
int main(){
    pid_t pid;
    //Création de 3 fils
    for(int i=1;i<=3;i++){
        pid = Fork();
        //Pour chaque fils on affiche son PID et son PPID
        if(pid == 0){
            printf("Le PID du processus fils %d est %d\n", i,getpid());
            printf("Le PPID du processus fils %d est %d\n",i, getppid());
            //Pour le fils 1 afficher le PID et le PPID du processus 
        if(i == 1){
            printf("Le PID du petit-fils 1 est %d\n",getpid());
            printf("Le PPID du petit-fils 1 est %d\n", getppid());
        }
        
        }
        
    
       
    }
}


