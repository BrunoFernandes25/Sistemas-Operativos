#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


/* 
Escreva um programa que emule o funcionamento do interpretador de comandos na execução encadeada de ls /etc | wc -l.

Retorna o total de linhas de ls /etc mais o  num_linhas de wc -l
*/


int main() {

    int fd[2];
    int status; 

    int pipe_return = pipe(fd);
    if(pipe_return == -1) { perror("erro na criação do pipe"); }

    pid_t pid = fork();
    
    if(pid == 0) {  //filho -> wc
        close(fd[1]); 
        dup2(fd[0],0); 
        close(fd[0]);   
        
        execlp("wc","wc","-l",NULL);     //só acrescentamos -l neste exercicio    
        _exit(0); //para o filho terminar
    }
    else{ //pai

        close(fd[0]);
        dup2(fd[1],1);
        close(fd[1]);
        execlp("ls", "ls", "/etc", NULL);
        //close(fd[1]);  nao é necessário pois execlp nao deixa fazer mais nada depois dele como já é sabido

        wait(&status);

    }

    return 0;
}

//pai executa um dos execs e o filho outro deles, um faz o ls e o outro o wc
//pai so pode fazer exec depois do processo filho