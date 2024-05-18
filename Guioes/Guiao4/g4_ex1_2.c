#include <time.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/wait.h>

//exercício 1
/*
int main (int argc, char** argv) {
    int fd[2];
    int status;

    int pipe_return = pipe(fd);
    if(pipe_return == -1) { perror("erro na criação do pipe"); }

    pid_t pid = fork();
    
    if(pid == 0) { // Filho: consumidor
        close(fd[1]);
        int res;
        ssize_t read_bytes = read(fd[0], &res, sizeof(int));

        printf("Filho: recebi o valor: %d (%zd bytes) \n", res, read_bytes);
    
        close(fd[0]);

        _exit(0);
    } else { // Pai : produtor
        close(fd[0]);
        int value = 300;

        ssize_t written_bytes = write(fd[1], &value, sizeof(int));
        printf("Pai: escrevi o valor %d (%zd bytes) \n", value, written_bytes);

        close(fd[1]);

        pid_t terminated_pid = wait(&status);
        printf("Pai: o processo-filho terminou com o codigo de saida %d.  (status -> %d) \n", terminated_pid, WEXITSTATUS(status));
    }

}*/

//Com sleep pedido no enunciado
// Alínea (a)

int main (int argc, char** argv) {
    int pd[2];
    int value;

    int pipe_ret = pipe(pd);
    if (pipe_ret == -1) perror("Erro na criação do pipe");

    pid_t pid = fork();

    if(pid == 0) {  // filho: consumidor 
        close(pd[1]);
        int res;
        ssize_t bytes_read = read(pd[0], &res, sizeof(int)); 
        printf("Filho: Recebi o valor %d do meu processo pai\n", res);
        close(pd[0]);
        _exit(0);
    } 
    else { // pai: produtor
        close(pd[0]);
        value = 2000;
        sleep(5);
        ssize_t bytes_written = write(pd[1], &value, sizeof(int));
        printf("Pai: Escrevi o valor %d para o meu processo filho\n", value);
        close(pd[1]);
        
        int status;
        pid_t terminated_pid = wait(&status);       //Recolher o estado
        printf("Pai: o processo-filho terminou com o codigo de saida %d \n", WEXITSTATUS(status));
    }
}


//exercício 2

int main (int argc, char** argv) {
    int fd[2];
    int status;

    int pipe_return = pipe(fd);
    if(pipe_return == -1) { perror("erro na criação do pipe"); }

    pid_t pid = fork();
    
    if(pid == 0) { // Filho: produtor
        close(fd[0]);

        for(int i = 300; i<3500; i++) {
            ssize_t written_bytes = write(fd[1], &i, sizeof(int));
            printf("Filho: escrevi o valor: %d (%zd bytes) \n", i, written_bytes);

        }

        close(fd[1]);

        _exit(0);
    } else { // Pai : consumidor
        close(fd[1]);
        int res;
        ssize_t read_bytes;
        sleep(3);
        while((read_bytes = read(fd[0], &res, sizeof(int))) > 0){
            printf("Pai: recebi o valor %d (%zd bytes) \n", res, read_bytes);
        }

    
        close(fd[0]);

        pid_t terminated_pid = wait(&status);
        printf("Pai: o processo-filho (%d) terminou com o codigo de saida %d \n", terminated_pid, WEXITSTATUS(status));
    }
}
//Filho escreve valores (ciclo for), pai espera 3 segundos e so depois le os valores recebidos


// gcc g4_ex1_2.c -o g4_ex1_2
// ./g4_ex1_2








//ex.4 Adicional
/*
linha termina com \n cada processo ler linha, mas como é que ele sabe? se fizermos a cada \n temos algo sequencial 
nao é tao eficiente.

E nao vamos contar quantas linhas temos pois com 1 TB seria tempo desperdiçado
Splits nao podem ser feitos para guardar em memória

ENTAO O QUE FAZER ????

-> Criar N processos e depois o pai vai ler linha a linha o ficheiro e no fim da linha manda o filho processar 
(usando strstr())

E o pai pode avançar para a próxima linha 

(ROUND ROBBIN)

*/
