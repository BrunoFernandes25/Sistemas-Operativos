#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>


int main() {

    int fd[2];
    int status; 

    int pipe_return = pipe(fd);
    if(pipe_return == -1) { perror("erro na criação do pipe"); }


    pid_t pid = fork();
    
    if(pid == 0) {  //filho -> wc
        close(fd[1]);  // pois é consumidor, embora nao haja read diretamente escrito ele vai acontecer
        dup2(fd[0],0); //tem que ler do pipe, escrever nao precisa de redirecionamento
        close(fd[0]);   //fechamos caso contrario irá ficar sempre aberto e dá asneira
        
        execlp("wc","wc",NULL);        
        _exit(0); //para o filho terminar
    }
    else{ //pai
        char buffer[1024];
        ssize_t read_res;

        close(fd[0]);
        while((read_res = read(0,buffer,1024)) > 0){
            write(fd[1], &buffer, read_res); //read_res pois é quantos caracteres conseguimos ler que serao escritos
        }
        close(fd[1]);

        wait(&status);
    }



    printf("TERMINEI !! \n");
    return 0;
}


// executar ./g6_ex4 escrever texto e enter (varias vezes) e cntr+D e tá feito


/*

wait antes do close daria deadlock  pois:

o filho estava a espera que o pai fechasse o extremo de escrita(??)
e o pai que o filho fechasse o de leitura(??)

*/