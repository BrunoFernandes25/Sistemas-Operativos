#include <stdio.h>
#include<sys/wait.h>
#include <unistd.h>
#include <fcntl.h>



int main() {

    int fd_input = open("/etc/passwd", O_RDONLY);
    int fd_output = open("saidas.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int fd_error = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    int fdi = dup(0);
    int fdo = dup(1);
    int fde = dup(2);


    dup2(fd_input, 0);
    close(fd_input);

    dup2(fd_output, 1);
    close(fd_output);
    
    dup2(fd_error, 2);
    close(fd_error);

    //fizemos fork e wait(NULL) para o pai
    if(fork() == 0){
        
        char buffer[1024];
        int read_res; //ssize_t read_res; 

        while((read_res = read(0,buffer,1024)) > 0){
            write(1,buffer,read_res);
            write(2,buffer,read_res);      
        }
        _exit(0); //para o filho terminar
    }

    wait(NULL); //sem este wait ele iria chegar a um ponto que nao deixaria criar mais processos

    dup2(fdi,0);
    dup2(fdo,1);
    dup2(fde,2);

    printf("TERMINEI !! \n");  
    perror("bla"); 

    return 0;
}
