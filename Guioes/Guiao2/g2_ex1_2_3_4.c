#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

//exercício 1
int main(){

    printf("PID(Meu Pid): %d -- PID do meu pai: %d\n",getpid(),getppid());
    sleep(20);      //apenas para permitir ver (***)
    printf("Terminei\n");
    return 0;
}

//exercício 2
int main(){
    pid_t pid;
    int status;
    int i = 0;

    pid = fork();  

    if(pid == 0){ // processo- filho      
        i--;
        printf("Filho: %d(meu) -- %d(pai)\n",getpid(),getppid());
        _exit(2000); // possivel de 0-254 se colocar 2000 ele vai dar o modulo de 2000/255, ele vai retornar o valor colocado entre parentesses se for entre 0-254
    }
    else{ //processo-pai
        i++;
        sleep(10);

        pid_t terminated_pid = wait(&status);

        if(WIFEXITED(status)){
            printf("Pai: %d(meu) -- %d(pai) -- %d(filho) -- %d(status) -- %d(i)\n",getpid(),getppid(),terminated_pid,WEXITSTATUS(status), i);

        }

    }

    printf("Terminei\n");

    return 0;

}

//exercício 3
int main(){                     //Temos algo sequencial executa um filho a cada iteração (10 segundos de execução)

    pid_t pid;
    int status;
    int i=0;

    for(int j = 0;j<10;j++){            //Total 10 filhos
        
        pid = fork();                   //Criação de filho

        if(pid == 0){ // processo- filho      
            printf("Filho: %d(meu) -- %d(pai)\n",getpid(),getppid());
            sleep(1); //1 seg para cada filho, 10 seg total
            _exit(j);
        }
        else{ //processo-pai
            pid_t terminated_pid = wait(&status);

            if(WIFEXITED(status)){
                printf("Iteração: %d Filho com pid -- %d terminou %d(status)\n",j,terminated_pid,WEXITSTATUS(status));

            }

        }
    }
    
    printf("Terminei\n");

    return 0;

}

//exercício 4
int main() {                //Aqui já temos algo concorrente cria 10 filhos e executa os 10 (+/- 2 segundo de execução)
    pid_t pid;
    int status;
    
    for(int i=0; i<10; i++) {
        
        pid = fork();

        if(pid == 0) {
            printf("Filho: %d \n", getpid());
            sleep(1);
            _exit(i+1);
        }

    }

    for(int i = 0; i < 10; i++) {
        pid_t terminated_pid = wait(&status);

        if(WIFEXITED(status)) {
                printf("Iteração: %d Filho com pid -- %d terminou %d(status)\n", i, terminated_pid, WEXITSTATUS(status));
                printf("Iteração: %d :: Filho com pid -- %d terminou %d(status)\n", i, terminated_pid, WEXITSTATUS(status));

            }
    }

    return 0;

}



//_exit() faz o pai esperar que o filho termine

//fazendo ps no terminal o pid pai nao muda, so muda o do filho correndo varias vezes o template

//Caso isto demore 10 segundos no exercicio 4 com 10 filhos ao mesmo tempo a correr teremos 1 segundo no total apenas +/-



// id_t getpid(void);      //retorna o PID do filho   (PID - Identificador de processo)

// pid_t getppid(void);     //retorna o PID do pai

// pid_t fork(void);        //cria um processo filho retorna um pid, se pid = 0 estamos no processo filho senao estamos no processo paiu

// void _exit(int status);      // termina o processo filho retorna um valor 0-255 em que 255 equivale a -1

// pid_t wait(int *status);       // espera o processo filho termine e retorna o pid do processo filho que terminou e o valor de retorno do status do processo

// pid_t waitPID(pid_t pid, int *status, int options);    // damos o PID e esperamos que o processo com esse pid termine

// int WIFEXITED(int status);    // status é o valor que pomos no _exit  e WIFEXITED verifica se o processo terminou bem

// int WEXITSTATUS(int status);   // dá o valor de retorno do filho (o valor de dentro do _exit)





/* (***)
PID(Meu Pid): 16532 -- PID do meu pai: 16264

Noutro terminal enquanto o programa espera 20 segundos para terminar

ps -p 16532 -p 16264 
    PID TTY          TIME CMD
  16264 pts/0    00:00:00 bash
  16532 pts/0    00:00:00 aula2 
*/
