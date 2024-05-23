#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int n = 20;

void defeitos(char* imagens[n], int n, int max){
    int fd[2];
    int status;
    if(pipe(fd) == -1) perror("Erro na criação do fifo");

    for(int i = 0;i<max;i++){
        pid_t pid = fork();
        
        if(pid == 0){   //queremos que o filho escreva os erros atraves da execução de Anom
            close(fd[0]);
            dup2(fd[1],1);  //escreve para o stdout dito no enunciado
            close(fd[1]);
            
            for(int j = i*n/max;j<(i+1)*n/max;j++){
                pid_t sub_pid = fork(); //criado pois exec muda a imagem e entao precisamos de fazer isto
                if(sub_pid == 0){
                    execlp("./detectAtom","detectAtom", imagens[j], NULL);
                    _exit(0);
                }
                else{
                    wait(&status);
                }
            }
            _exit(0);
        }
    }
    
    //pai vai ler anomalias recebidas do filho
    char anom[300];
    ssize_t read_bytes;

    close(fd[1]);
    while((read_bytes = read(fd[0],&anom,sizeof(anom))) > 0){
        printf("%s \n", anom);
    }
    close(fd[0]);

    for(int i=0;i<max;i++){
        wait(&status);
    }
}

void conta(char* imagens[n], int n){
    int fd[2];
    int status;
    if(pipe(fd) == -1) perror("Erro na criação do fifo");
  
    pid_t pid = fork();
    if(pid ==-1) perror("Fork falhou");
    
    if(pid == 0){  //filho contabiliza totalidade com wc -l
      close(fd[1]);
      dup2(fd[0],0);
      close(fd[0]);
  
      execlp("wc","wc","-l",NULL);
    
      _exit(0);
    }
    else{    //pai faz uso da função defeitos
      close(fd[0]);
      dup2(fd[1],1);
      close(fd[1]);
      
      defeitos(imagens,n,n/10);

      wait(&status);  // espera o filho terminar
    }
}