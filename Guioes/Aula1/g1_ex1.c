#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

/*
Implemente em C um programa mycat com funcionalidade similar ao comando cat, que lê do seu stdin e escreve para o seu stdout.
*/

// 0 -> STDIN
// 1 -> STDOUT

int main(){
    
    int buffer_size = 1024;
    char *buffer = malloc(sizeof(char) * buffer_size);  //buffer tamanho de 1024 bytes
    //ou char buffer[1024] de forma estatica e nao dinámica

    ssize_t bytes_read;

    int i=0;
    while ((bytes_read = read(0,buffer,buffer_size)) > 0){
    //queremos ler algo e colocar no buffer e queremos maximo de memoria lida de 1024
    //como read obtem o numero de bytes lido fazemos size_t bytes_read para depois escrever num maximo desses bytes
        i++;
        printf("iteration: %d\n",i);
        write(1,buffer,bytes_read);

    } 
    free(buffer);
    
    return 0;
}