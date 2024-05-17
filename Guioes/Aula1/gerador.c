#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(){

    int fd = open("test.txt", O_CREAT | O_WRONLY,0644);


    printf("file descriptor: %d\n",fd);

    for(int i=0;i<10*1024*1024;i++){//10*1024*1024 = 10 MB
        write(fd,"a",1);
    }

    close(fd);

    return 0;
}

//gcc -o gerador gerador.c
// ./gerador