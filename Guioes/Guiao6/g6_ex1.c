#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/*
int main() {

    //redirecionar descritores com dup2

    //criação files descriptores respetivos
    int fd_input = open("/etc/passwd", O_RDONLY);
    int fd_output = open("saidas.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int fd_error = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    
    dup2(fd_input, 0);          //copia de um descritor para outro dup2(...)
    close(fd_input);            //não precisando do descritor é boa pratica fechar
    dup2(fd_output, 1);
    close(fd_output);
    dup2(fd_error, 2);
    close(fd_error);

    //isto é o mycat rever aula1
    char buffer[1024];
    int read_res;

    while((read_res = read(0,buffer,1024)) > 0){
        write(1,buffer,read_res);       //para o std output
        write(2,buffer,read_res);       //para o error
    }

    printf("TERMINEI !! \n");   //Este vai para saidas.txt
    perror("bla");  //este vai para o erros.txt


    return 0;
}*/




/*
Codigo que permite ter copias para mais tarde usar, e nao acontecer como acima que aparecendo terminei, nao voltará a aparecer
Com este abaixo ao executar iremos ter sempre o terminei "printado"*/


int main() {

    //redirecionar descritores com dup2

    //criação files descriptores respetivos
    int fd_input = open("/etc/passwd", O_RDONLY);
    int fd_output = open("saidas.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);
    int fd_error = open("erros.txt", O_CREAT | O_TRUNC | O_WRONLY, 0666);

    //guardamos assim o descritor para input,output e error caso queiramos mudar mais adiante as coisas
    int fdi = dup(0);
    int fdo = dup(1);
    int fde = dup(2);



    dup2(fd_input, 0);
    close(fd_input);
    dup2(fd_output, 1);
    close(fd_output);
    dup2(fd_error, 2);
    close(fd_error);

    //isto é o mycat rever aula1
    char buffer[1024];
    int read_res;

    while((read_res = read(0,buffer,1024)) > 0){
        write(1,buffer,read_res);       //para o std output
        write(2,buffer,read_res);       //para o error
    }

    //apos fdi, etc.. fizemos as linhas imediantamente a seguir e antes do printf e perror
    dup2(fdi,0);
    dup2(fdo,1);
    dup2(fde,2);

    close(fdi);
    close(fdo);
    close(fde);
    //Com estas linahs acima já reaparece no terminal

    printf("TERMINEI !! \n");   //Este vai para saidas.txt
    perror("bla");  //este vai para os Erros

    return 0;
}