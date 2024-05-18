#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

int main() {

    int fd_input = open("/etc/passwd", O_RDONLY);
    int fd_output = open("saidas.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int fd_error = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    dup2(fd_input, 0);
    close(fd_input);
    dup2(fd_output, 1);
    close(fd_output);
    dup2(fd_error, 2);
    close(fd_error);

    execlp("wc","wc",NULL);

    printf("TERMINEI !! \n");
    return 0;
}


//executar e depois cat saidas.txt

//nao imprime terminei pois o exec executa o programa e depois o codigo seguinte já nao é executado

//mas se correr execlp dentro de um processo filho, já iria printar terminei 