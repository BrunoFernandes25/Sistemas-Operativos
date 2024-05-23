#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>

struct RegistoF {
    char nome[20];
    char cargo[20];
    int salario;
};

void aumentaSalarios(char* ficheiro, char* cargo, int valor, long N, int P){
    int status;
    int fd= open(ficheiro,O_RDWR);

    for(int i=0;i<P;i++){
        pid_t pid = fork();

        if(pid == 0){       //filho
            struct RegistoF r;

            for(int j=i*(N/P);j<(i+1)*(N/P);j++){
                lseek(fd,j*sizeof(struct RegistoF),SEEK_SET);
                read(fd,&r,sizeof(struct RegistoF));
                
                if(!strcmp(cargo,r.cargo)){
                    r.salario += valor;
                    lseek(fd,-sizeof(struct RegistoF),SEEK_CUR);
                    write(fd,&r,sizeof(struct RegistoF));
                }
            }
        _exit(0);
        }
    }

    for(int i=0;i<P;i++){
        wait(&status);
    }

    close(fd);
}


int validaSalarios(char* ficheiro, char* cargo) {
    int fd[2][2];
    pipe(fd[0]);

    if(fork()==0){                  //filho 1 quer escrever para o filho 2
        close(fd[0][0]);
        dup2(fd[0][1],1);
        close(fd[0][1]);

        execlp("./filtracargo","filtracargo",ficheiro,cargo,NULL);
        _exit(0);
    }
    //pai- resposta ao filho 1
    close(fd[0][1]);                   // O pai não vai escrever no pipe, então fecha o descritor de escrita.
    wait(NULL);

    pipe(fd[1]);
    if(fork() == 0){                   //filho 2 quer ler do filho 1 o que ele filtrou e escrever para o pai o que este validou
        close(fd[1][0]);               //como queremos escrever para o pai convem fechar leitura

        dup2(fd[0][0],0);               //para ler o que filho1 filtrou
        close(fd[0][0]);

        dup2(fd[1][1],1);               //para escrever para depois o pai ler (linha 117)
        close(fd[1][1]);

        execlp("./validaMin","validaMin", NULL);
        _exit(0);
    }
    //pai resposta ao filho 2
    close(fd[0][0]);                    // O pai não vai mais ler do primeiro pipe, então fecha o descritor de leitura.
    wait(NULL);
    
    char buffer[10];
    close(fd[1][1]);                   //pai quer ler o que vem do filho2 logo fecha fd[1][1]
    read(fd[1][0], &buffer, sizeof(buffer));
    close(fd[1][0]);

    return atoi(buffer);

}

/*  
//Se nao fosse necessário a função retornar algo isto estaria correto
int validaSalarios(char* ficheiro, char* cargo) {
    int fd[2], status;
    pipe(fd);
    
    pid_t pid_filtra = fork();
    if(pid_filtra == 0) { // eescreve para o outro filho por fd[1] o resultado o ./filtra....
        close(fd[0]);
        dup2(fd[1], 1); 
        close(fd[1]);

        execlp("./filtracargo","filtracargo",ficheiro, cargo, NULL);
        _exit(1);
    }

    pid_t pid_valida = fork();
    if(pid_valida == 0) { //lde o fd[0] o que recebeu 
        close(fd[1]);
        dup2(fd[0], 0);
        close(fd[0]);

        execlp("./validaMin","validaMin", NULL);
        _exit(1);
    }

    //pai espera filhos
    waitpid(pid_filtra, &status, 0);
    waitpid(pid_valida, &status, 0);

    if (WIFEXITED(status)) {
        return WEXITSTATUS(status);
    } else {
        return 0;
    }

}*/

// NAO ERA PRECISO ISTO NO TESTE, FEITO APENAS PARA TESTAR MAIN
void filtracargo(const char* ficheiro, const char* cargo) {
    int fd = open(ficheiro, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        _exit(1);
    }

    struct RegistoF r;
    while (read(fd, &r, sizeof(struct RegistoF)) > 0) {
        if (strcmp(r.cargo, cargo) == 0) {
            write(STDOUT_FILENO, &r, sizeof(struct RegistoF));
        }
    }

    close(fd);
    _exit(0);
}

void validaMin() {
    struct RegistoF r;
    int minSalario = 2000; // Exemplo de valor mínimo de salário
    int valid = 1;

    while (read(STDIN_FILENO, &r, sizeof(struct RegistoF)) > 0) {
        if (r.salario < minSalario) {
            valid = 0;
            break;
        }
    }

    _exit(valid ? 0 : 1);
}


int main() {
    struct RegistoF registros[] = {
        {"Alice", "Engenheiro", 3000},
        {"Bob", "Engenheiro", 3500},
        {"Charlie", "Tecnico", 2000},
        {"David", "Engenheiro", 3200},
        {"Eve", "Tecnico", 2100},
        {"Frank", "Engenheiro", 3300},
        {"Grace", "Tecnico", 2200},
        {"Heidi", "Engenheiro", 3100}
    };

    long N = sizeof(registros) / sizeof(registros[0]);
    int P = 2;  // Número de processos a serem criados
    char ficheiro[] = "salarios.dat";

    // Criação do arquivo binário com os registros de exemplo
    int fd = open(ficheiro, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return -1;
    }

    // Escrever os registros no arquivo
    write(fd, registros, sizeof(registros));
    close(fd);

    char cargo[] = "Engenheiro";
    int valorAumento = 500;

    // Chamada da função aumentaSalarios
    aumentaSalarios(ficheiro, cargo, valorAumento, N, P);
    printf("Salários atualizados.\n");

    // Validação dos salários
    int resultadoValidacao = validaSalarios(ficheiro, cargo);
    printf("Resultado da validação dos salários: %d\n", resultadoValidacao);

    // Leitura e exibição dos registros atualizados
    fd = open(ficheiro, O_RDONLY);
    if (fd == -1) {
        perror("Erro ao abrir o arquivo");
        return -1;
    }

    printf("Registros atualizados:\n");
    struct RegistoF reg;
    for (int i = 0; i < N; i++) {
        read(fd, &reg, sizeof(reg));
        printf("Nome: %s, Cargo: %s, Salário: %d\n", reg.nome, reg.cargo, reg.salario);
    }
    close(fd);

    return 0;
}
