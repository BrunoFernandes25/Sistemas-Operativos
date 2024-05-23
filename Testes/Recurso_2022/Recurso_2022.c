#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>

int comando(const char* filtro, const char* entrada, const char* saida) {
    int status;
    int fd_in, fd_out;
    int fd[2][2];
    pid_t pid;
    char buffer[1024];
    int read_bytes;

    pipe(fd[0]);
    pipe(fd[1]);

    pid=fork();
    if(pid == 0){
        close(fd[0][0]);            //Fechamos descritores de leitura para o stdout(0) e stderr(1), 0 e 1 defenido em fd usualmente é 1 e 2 respetivamente
        close(fd[1][0]);

        fd_in = open(entrada,O_RDONLY);
        fd_out = open(saida, O_WRONLY | O_CREAT | O_TRUNC,0666);

        dup2(fd_in,0);
        dup2(fd_out,1);
        dup2(fd[1][1],2);
        close(fd_in);
        close(fd_out);
        close(fd[1][1]);

        execlp(filtro,filtro,NULL);
        _exit(0);
    }
    else{
        close(fd[0][1]);        //Fechamos descritores de escrita para o stdout(0) e stderr(1)
        close(fd[1][1]);

        read_bytes = read(fd[1][0],buffer,sizeof(buffer));      //lemos do stderr para saber o que retornar
        close(fd[1][0]);

        wait(&status);

        if(WIFEXITED(status)){      //Caso corra bem e nao escreva no stderr apenas no stdout
            if(WEXITSTATUS(status) == 0){
                return 0;
            }
        }
        if(read_bytes > 0){
            return atoi(buffer); //retorna 1 caso escreva no stderr
        }
    }
}
//Main ex1
/*int main() {
    // Exemplo de uso
    const char* filtro = "cat";
    const char* entrada = "entrada.txt";
    const char* saida = "saida.txt";
    
    int result = comando(filtro, entrada, saida);
    
    if (result == 0) {
        printf("O filtro foi executado com sucesso.\n");
        printf("%d\n",result);
    } else {
        printf("Houve um erro ao executar o filtro.\n");
    }

    return 0;
}*/


//SERVIDOR
#define SERVER "Server_fifo" 
#define Max_args 5

typedef struct{
    char filtro[20];
    char entrada[20];
    char saida[20];
}pedido_client;

void processa_pedido(pedido_client pedido) {
    if (comando(pedido.filtro, pedido.entrada, pedido.saida)) {
        int log_fd = open("comando.log", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (log_fd == -1) { perror("Erro abrir log");}

        char log_entry[100];
        sprintf(log_entry, "Filtro: %s, Entrada: %s, Saída: %s\n", pedido.filtro, pedido.entrada, pedido.saida);
        write(log_fd, log_entry, strlen(log_entry));
        close(log_fd);
    }
}

int main() {
    // Inicializa o pipe nomeado (FIFO)
    int fifo_ret = mkfifo(SERVER, 0666);
    if (fifo_ret == -1) {
        perror("Erro ao criar o fifo:");
        return -1;
    }

    while (1) {
        pedido_client pedidos[Max_args];
        int num_p = 0;

        // Abre o pipe para leitura do pedido cliente
        int fdL = open(SERVER, O_RDONLY);
        if (fdL == -1) {
            perror("Erro ao abrir o pipe:");
            return -1;
        }

        // Lê até Max_args pedidos do pipe
        while (num_p < Max_args && (read(fdL, &pedidos[num_p], sizeof(pedido_client)) > 0)) {
            num_p++;
        }
        close(fdL);

        // Processa os pedidos de forma concorrente criando filhos através de um for
        pid_t pids[Max_args];
        for (int i = 0; i < num_p; i++) {
            pids[i] = fork();
            if (pids[i] == 0) {
                processa_pedido(pedidos[i]);
                exit(0);
            }
        }

        // Espera todos os processos filhos terminarem
        for (int i = 0; i < num_p; i++) {
            waitpid(pids[i], NULL, 0);
        }
    }

    // Remove o pipe nomeado
    unlink(SERVER);
    return 0;
}