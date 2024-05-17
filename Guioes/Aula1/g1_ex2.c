#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

/*
Implemente em C um programa mycp com funcionalidade similar ao comando cp. 
Varie o tamanho do buffer usado e meça o tempo necessário para copiar um ficheiro de grande dimensão
*/

int main() {
    int fd_origin = open("test.txt", O_RDONLY);     //Ficheiro de leitura O_RDONLY
    
    if(fd_origin == -1) {
        perror("fd: ");
    }

    int fd_dest = open("escrita.txt", O_CREAT | O_WRONLY, 0644);    //Ficheiro de leitura O_RDONLY   
    // 0644 para dar para dar permissão para ler e escrever

    int buffer_size = 10*1024*1024;                                 // demos este tamanho devido ao gerador criado
    char* buffer = malloc(sizeof(char) * buffer_size);
    
    ssize_t bytes_read;
    
    while ((bytes_read = read(fd_origin, buffer, buffer_size)) > 0) {       //lê o origin até ao fim
        write(fd_dest, buffer, bytes_read);                                 //escreve no dest
    }

    free(buffer);

    close(fd_origin);
    close(fd_dest);
}