#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/wait.h>

//Exercício 1 
/*
int main() {

    printf("mensagem \n");

    int res = execl("/bin/ls", "ls", "-l", NULL);

    if (res == -1) printf("Erro no exec");

    printf("mensagem 2 \n");

}
*/

//Exercício 2
int main() {

    printf("mensagem 1 \n");

    pid_t pid = fork();
    if(pid == 0) {

        int res = execlp("ls", "ls", "-l", NULL);

        if (res == -1) {
            printf("Erro no exec");
        }
        _exit(res);

    } else {

        int status;
        wait(&status);
        if(WIFEXITED(status)) {
            printf("O processo filho terminou com codigo de saida %d \n", WEXITSTATUS(status));
        }
    }
    printf("mensagem 2 \n");
    
    return 0;

}

// gcc g3_ex1_2.c -o g3_ex1_2
// ./g3_ex1_2


//                           TEORIA
// int execl(const char *path, const char *arg0, ..., NULL);        // exemplo: int res = execl("/bin/ls", "ls", "-l", NULL);
// tem de receber o path do primeiro comando e os seguintes separados com o ultimo comando a NULL

// int execlp(const char *file, const char *arg0, ..., NULL);       // exemplo: int res = execlp("ls", "ls", "-l", NULL);
// so precisa de receber o nome do primeiro comando e os seguintes separados, com o ultimo comando a NULL

// int execv(const char *path, char *const argv[]);                 // exemplo: int res = execv("/bin/ls", ["ls", "-l"]);
// tem de receber o path do primeiro comando e todos num array

// int execvp(const char *file, char *const argv[]);                // exemplo: int res = execvp("ls", ["ls", "-l"]);
// so precisa de o nome do primeiro comando e todos (incluindo o primeiro) num array