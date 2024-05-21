#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int mensagens(char* palavra, char* ficheiro) {
    int fd[2];
    int status;

    if (pipe(fd) == -1) {
        perror("Erro ao criar o pipe"); 
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("Erro ao criar o processo filho");
        return -1;
    }

    if (pid == 0) {             // Processo filho 
        close(fd[0]);           // Fecha a leitura do pipe pois queremos escrever para o pai
        dup2(fd[1], 1); 
        close(fd[1]); 

        execlp("grep", "grep", "-o", palavra, ficheiro, NULL);

        _exit(0);
    } else {                    // Processo pai 
        close(fd[1]);           // Fecha a escrita do pipe pois ler o que vem do filho
        dup2(fd[0],0); 
        close(fd[0]); 

        execlp("wc", "wc", "-l", NULL);         //conta numero de linhas da palavra padrao que vêm da eecução de grep

        wait(&status);
    }

    return 0;
}

int autores_que_usaram (char* palavra, int n, char* autores [n]){

    int total;    
    int status;
    
    pid_t pid;

    for(int i=0;i<n;i++){
        pid=fork();
        if(pid == 0){
            int num_ocorrencias = mensagens(palavra,autores[i]);
            if(num_ocorrencias > 0){
                exit(i);
            }
            else{
                exit(-1);
            }
        }
    }

    for(int i=0;i<n;i++){
        pid_t terminated_pid = wait(&status);

        if(WIFEXITED(status)){
            if(WEXITSTATUS(status) != 255){
                total++;
            }
        }
    }

    return total;
}



//Exercício 1
/*
int main() {
    char palavra[] = "exemplo"; // Palavra a ser procurada
    char ficheiro[] = "mensagens.txt"; // Nome do arquivo onde procurar a palavra

    int num_ocorrencias = mensagens(palavra, ficheiro);
    printf("Número de ocorrências da palavra '%s' no arquivo '%s': %d\n", palavra, ficheiro, num_ocorrencias);

    return 0;
}*/


int main() {
    // Arquivos de exemplo
    char* autores[] = {"autor1.txt", "autor2.txt", "autor3.txt"};
    char palavra[] = "exemplo";

    // Criação dos arquivos de exemplo
    FILE *file1 = fopen("autor1.txt", "w");
    fprintf(file1, "Olá, este é um exemplo.\nEste é outro exemplo de mensagem.\n");
    fclose(file1);

    FILE *file2 = fopen("autor2.txt", "w");
    fprintf(file2, "Este autor não usou a palavra chave.\n");
    fclose(file2);

    FILE *file3 = fopen("autor3.txt", "w");
    fprintf(file3, "Exemplo de mensagem para teste.\nOutra linha de exemplo.\n");
    fclose(file3);

    int total_autores = autores_que_usaram(palavra, 3, autores);
    printf("Número de autores que usaram a palavra '%s': %d\n", palavra, total_autores);

    // Remover arquivos de exemplo após o teste
    remove("autor1.txt");
    remove("autor2.txt");
    remove("autor3.txt");

    return 0;
}