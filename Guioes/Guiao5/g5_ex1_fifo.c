#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>


#define MAX_LINE_SIZE 1024

int main (int argc, char** argv){

    const char* name = "fifo";
    int res = mkfifo(name, 0666);

    if(res == -1){
        perror("Erro na criação do FIFO");
        return -1;
    }
    
    printf("Fifo foi criado com o nome \"%s\" \n", name);
    return 0;
}



// gcc g5_ex1.c -o g5_ex1
// ./g5_ex1 


//Se tentarmos invocar outra vez dá erro FILES EXISTS







// -------------------------------------------------------------- FIFO (named pipe) -----------------------------------------------------------------------------------
// Semantica de comunicação num so sentido (fifo)
// Consumidor bloqueia na leitura se nao houver nada para ler e Produtor bloqueia na escrita se nao houver nada para escrever 

// int mkfifo(const char *path, mode_t mode);  // Retorna 0 caso a criação seja bem sucedia e -1 caso contrario
// Utilização semelhante à de ficheiros regulares (open, close, read, write, …) mas não é poosivel abrir para ler e escrever em simultaneo e nao é possivel usar lseek

// int open(const char *path, int oflag, permissoes);  // Devolve o fildes

// ssize_t read(int fildes, void *buf, size_t nbyte); 
// ssize_t write(int fildes, const void *buf, size_t nbyte);
// int close(int fildes) 

// unlink(path)    // Precisam de ser explicitamente removidos ja que nao desaparecem quando os processos terminam