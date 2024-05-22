#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

int mensagens(char* palavra, char* ficheiro) {
    int fd[2][2];
    
    pipe(fd[0]);
    if(fork() == 0) {
        close(fd[0][0]);                                    // Fecha a leitura do pipe 0, pois o filho vai escrever nele para o filho seguinte
        dup2(fd[0][1], 1);                                  // Coloca fd[0][1] em stdout(1)
        close(fd[0][1]);
        execlp("grep", "grep", palavra, ficheiro, NULL);    //retorna linhas com palavra encontrada por linha
        _exit(0);
    } 
    //resposta pai ao filho 1
    close(fd[0][1]);                                        // Fecha a escrita do pipe 0 no pai
    wait(NULL);                                             //espera filho

    pipe(fd[1]);
    if(fork() == 0) {                                       //Quer ler o que o fd[0] escreveu, atraveś do fd[0][0] e escrever o resultado ao pai pelo fd[1][1]
        //close(fd[1][0]);                                    // Fecha a leitura do pipe 1, pois o filho vai ler dele
        dup2(fd[0][0], 0);                                  // Colocamos fd[0][0] no stdin para ler o que lá tem
        close(fd[0][0]);                                 
        dup2(fd[1][1], 1);                                  // Redireciona a saída padrão para o pipe 1
        close(fd[1][1]);
        execlp("wc", "wc", "-l", NULL);                     // contar o número de linhas resultante do filho acima
        _exit(0);
    }
    //resposta pai ao filho 2
    close(fd[0][0]);                                        // Fecha a leitura do pipe 0 no pai
    wait(NULL);

    char buff[10];                                      // Buffer para armazenar a saída do comando wc -l

    close(fd[1][1]);
    read(fd[1][0], buff, sizeof(buff));                 // Lê a saída do comando wc -l do pipe 1
    printf("%s", buff);
    close(fd[1][0]);                                    // Fecha a leitura do pipe 1
    
    return atoi(buff);
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
int main() {
    char palavra[] = "exemplo"; // Palavra a ser procurada
    char ficheiro[] = "mensagens.txt"; // Nome do arquivo onde procurar a palavra

    int num_ocorrencias = mensagens(palavra, ficheiro);
    printf("Número de ocorrências da palavra '%s' no arquivo '%s': %d\n", palavra, ficheiro, num_ocorrencias);

    return 0;
}


/*
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
}*/


//Desta forma nao retornaria um inteiro pois exec nao iria deixar fazer  o return da funçã, apenas ia escrever no terminal
/*
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
}*/
