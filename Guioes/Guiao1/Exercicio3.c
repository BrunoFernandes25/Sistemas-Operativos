#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(){
    int i = 10;
    int j = 20;
    int h = 0;

    int fd = open("teste2.txt",O_RDWR | O_CREAT | O_TRUNC, 0600);

    write(fd,&i,sizeof(int));
    write(fd,&j,sizeof(int));

    ssize_t res = read(fd,&h,sizeof(int));
    printf("res: %zu, h %d\n",res,h);

    //close(fd);

    return 0;
}