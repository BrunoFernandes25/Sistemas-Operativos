#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#define MAX_LINE_SIZE 1024

/*
//Nesta versão terminando a escrita(ou seja terminada a escrita por parte do utilizador ctrl+D, fifo de leitura fecha tambem)
int main (int argc, char** argv){


    int fifo_fd = open("fifo",O_RDONLY);
    printf("Abri o FIFO para leitura ... \n");

    char buffer[MAX_LINE_SIZE]; //buffer para colocar o que for lido
    ssize_t read_bytes;

    while((read_bytes = read(fifo_fd,&buffer,sizeof(buffer))) > 0){
        write(1,&buffer,read_bytes);
    }

    close(fifo_fd);

    return 0;
}*/


//Mas terminava ao terminar o de escrita entao como contornar isso? 
int main (int argc, char** argv){

    while(1){
        int fifo_fd = open("fifo",O_RDONLY);
        printf("Abri o FIFO para leitura ... \n");

        char buffer[MAX_LINE_SIZE]; //buffer para colocar o que for lido
        ssize_t read_bytes;

        while((read_bytes = read(fifo_fd,&buffer,sizeof(buffer))) > 0){
            write(1,&buffer,read_bytes);
        }

        close(fifo_fd);

    }
    return 0;
}

//Nesta versao com while(1) fica aberto à espera de novas escritas para ler



//Abrir 2 terminais separados e fazer o seguinte:

// gcc g5_ex1_write.c -o g5_ex1_write  ||  gcc g5_ex1_read.c -o g5_ex1_read
// ./g5_ex1_write                      ||  ./g5_ex1_read
// escrever e ver o que acontece 
// voltar a executar ./g5_ex1_write e voltar a escrever
