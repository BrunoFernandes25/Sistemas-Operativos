#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>


#define MAX_LINE_SIZE 1024

int main (int argc, char** argv){
    int fifo_fd = open("fifo",O_WRONLY);
    printf("Abri o FIFO para escrita ... \n");

    char buffer[MAX_LINE_SIZE];                                 // buffer para colocar o que for lido
    ssize_t read_bytes;
                                                                // 0 porque queremos ler
    while((read_bytes = read(0,&buffer,MAX_LINE_SIZE)) > 0){    // leitura do standard input
        write(fifo_fd,&buffer,read_bytes);                      // e escrevemos
    }

    close(fifo_fd);

    return 0;
}