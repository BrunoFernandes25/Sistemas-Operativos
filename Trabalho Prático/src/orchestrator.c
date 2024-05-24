#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAX_PROG 300
#define TERMINADOS "terminados.txt"

struct program
{
    int type;                    // Execute -u (0) || Execute -p (1) || Status (2) || request (3)
    int state;
    char args[MAX_PROG];
    int time;
    int start_time;
    int id;
    int pid;
    struct program* next;  
};

typedef struct wrt{
    int pid;
    char args[MAX_PROG];
    int time;
} Wrt;

typedef struct msg{
    int state;
    int pid;
    char args[MAX_PROG];
    int time;
} Msg;

int executec (const char* command) {
	int status;
	int res = -1;
	int exec_ret;
    
	char *exec_args[20];
	char *string, *cmd, *tofree;
	int i=0;
    tofree = cmd = strdup(command);
    string = strtok(cmd," ");         //alteraao da função feita em aula para nao ter espaços antes e depois da barra "|"
    while(string !=NULL){
        exec_args[i]=string;
        string = strtok(NULL," ");
        i++;
    }

	exec_args[i]=NULL;

	pid_t pid = fork();
	switch (pid) {
		case -1: // correu mal o fork
			break;

		case 0:  // codigo processo filho
			exec_ret = execvp(exec_args[0], exec_args);
			if (exec_ret == -1) perror("Erro no exec");

			_exit(exec_ret);
			break;

		default: // codigo processo pai
			wait(&status);
			if(WIFEXITED(status)) {
				if (WEXITSTATUS(status) == 255) res = -1;
                else res = WEXITSTATUS(status);
			}
			break;
	}	
	free(tofree);
	return res;
}

int exec_command(char* arg){

    //Estamos a assumir numero maximo de argumentos
    char *exec_args[20];

    char *string;    
    int exec_ret = 0;
    int i=0;

    char* command = strdup(arg);

    string=strtok(command," ");
    
    while(string!=NULL){
        exec_args[i]=string;
        string=strtok(NULL," ");
        i++;
    }

    exec_args[i]=NULL;
    
    exec_ret=execvp(exec_args[0],exec_args);
    
    return exec_ret;
}

int executep(const char* command){
	
    int number_of_commands = 0;
    char* commands[50];

    char *cmd = strdup(command);
    cmd = strtok(cmd, "|");
    int i = 0;

    while (cmd != NULL) {
        commands[i++] = cmd;
        cmd = strtok(NULL, "|");
        number_of_commands++;
    }

	int pipes[number_of_commands-1][2];
	
	    for(int i = 0; i < number_of_commands; i++) {
        if(i==0) {
            pipe(pipes[i]);

            switch(fork()) {
            case 0:
                close(pipes[i][0]);
                dup2(pipes[i][1],1);
                close(pipes[i][1]);
                exec_command(commands[i]);
                _exit(0);
                break;

            default:
                close(pipes[i][1]);
                break;
            }
        }
        else if(i==number_of_commands-1) {
            switch(fork()) {
            case 0:
                dup2(pipes[i-1][0],0);
                close(pipes[i-1][0]);
                exec_command(commands[i]);
                _exit(0);
                break;
            
            default:
                {close(pipes[i-1][0]);
                break;}
            }
        }
        else {
            pipe(pipes[i]);

            switch(fork()) {
            case 0:
                close(pipes[i][0]);
                dup2(pipes[i][1],1);
                close(pipes[i][1]);
                dup2(pipes[i-1][0],0);
                close(pipes[i-1][0]);
                exec_command(commands[i]);
                _exit(0);
                break;
            
            default:
                {close(pipes[i][1]);
                close(pipes[i-1][0]);
                break;}
            }
        }
    }

	for(int i = 0; i < number_of_commands; i++) {
		wait(NULL);
	}

	return 0;
}


int send_completed(int fdC) {
    int fdes = open(TERMINADOS, O_RDONLY);
    if(fdes == - 1) {
        return -1;
    }

    int read_bytes;
    Msg m;
    Wrt w;

    while((read_bytes = read(fdes, &w, sizeof(Wrt))) > 0) {
        m.state = 2;
        m.pid = w.pid;
        m.time = w.time;
        strcpy(m.args, w.args);
        write(fdC, &m, sizeof(Msg));
    }

    int close_result = close(fdes);
    if(close_result == -1) {
        perror("Erro no close:");
    }
    return 0;
}

void send_scheduled(int fdC, struct program** ready) {
    struct program *current = *ready;
    Msg m;

    while(current != NULL) {
        m.state = 0;
        m.pid = current->id;
        m.time = 0;
        strcpy(m.args, current->args);
        write(fdC, &m, sizeof(Msg));
        current = current->next;
    }
}

void send_executing(int fdC, struct program** executing) {
    struct program *current = *executing;
    Msg m;

    while(current != NULL) {
        m.state = 1;
        m.pid = current->id;
        m.time = 0;
        strcpy(m.args, current->args);
        write(fdC, &m, sizeof(Msg));
        current = current->next;
    }

}

void insere(struct program** lista, struct program* new, char* policy) {
    if(!strcmp(policy, "fcfs")) {
        new->next = NULL;
        if(*lista == NULL) {
            *lista = new;
        } 
        else {
            struct program* aux = *lista;
            while(aux->next != NULL) {
                aux = aux->next;
            }
            aux->next = new;
        }
    }
    else if(!strcmp(policy, "sjf")) {
        struct program* current;

        if (*lista == NULL || (*lista)->time > new->time) {
            new->next = *lista;
            *lista = new;
        } else {
            current = *lista;
            while (current->next != NULL && current->next->time <= new->time) {
                current = current->next;
            }
            new->next = current->next;
            current->next = new;
        }
    }
}


struct program* removeq(struct program** lista) {                   //Retira o primeiro nodo da lista e retorna esse nodo
    struct program* new_node = (struct program*) malloc(sizeof(struct program));
    new_node->type = (*lista)->type;
    new_node->state = (*lista)->state;
    strcpy(new_node->args, (*lista)->args);
    new_node->time = (*lista)->time;
    new_node->start_time = (*lista)->start_time;
    new_node->id = (*lista)->id;
    new_node->pid = (*lista)->pid;
    new_node->next = NULL;

    (*lista) = (*lista)->next;

    return new_node;
}


void removeExecuting(struct program** executing, int pid) {   //Retira o nodo da lista com o pid correspondente

    struct program *current = (*executing);
    struct program *prev = NULL;

    while (current != NULL) {
        if (current->id == pid) {
            if (prev == NULL) {
                (*executing) = current->next;
            } else {
                prev->next = current->next;
            }
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }

}


int main (int argc, char *argv[] ){

    if(argc != 4) {
        printf("Input Error \n");
        return -1;
    }

    char* output_folder = argv[1];
    int slots = atoi(argv[2]);
    char* sched_policy = argv[3];
    
//Criação do fifo para receber pedidos do cliente
    if (mkfifo("fifo", 0666) == -1){ 
        perror("Erro na criação do FIFO:");
        return -1;
    }

    struct program* ready = (struct program*) malloc(sizeof(struct program));
    struct program* executing = (struct program*) malloc(sizeof(struct program));
    ready = NULL;
    executing = NULL;

    struct timeval start_time;
    struct timeval end_time;

    while(1){
        struct program* new = (struct program*) malloc(sizeof(struct program));
        int fd = open("fifo", O_RDONLY);
        if (fd == -1){ 
            perror("Erro no open do FIFO Servidor: ");
            return -1;
        }

        int read_bytes = read(fd, new, sizeof(struct program));
        if(read_bytes <= 0) perror("Nao leu: ");
        close(fd);
        new->next = NULL;

        if(new->type == 2) {        //É um status
            pid_t pid = fork();
            if(pid == 0) {

                char fifoc_name[30];
                sprintf(fifoc_name, "%d", new->id);

                int fdC = open(fifoc_name, O_WRONLY);
                if(fdC == -1) {
                    perror("Erro no open do FIFO cliente: ");
                    return -1;
                }
                
                send_executing(fdC, &executing);
                send_scheduled(fdC, &ready);
                send_completed(fdC);

                close(fdC);
                new->pid = getpid();
                new->type = 3;
                new->id = -1;

                int fdE = open("fifo", O_WRONLY);
                if(fdE == -1) {
                    perror("Erro no open do fifo do Servidor: ");
                    return -1;
                }

                write(fdE, new, sizeof(struct program));
                close(fdE);
                
                _exit(0);
            }
            
        }
        else if(new->type == 0 || new->type == 1) {         //É um execute
            char fifoc_name[30];
            sprintf(fifoc_name, "%d", new->id);

            int fdC = open(fifoc_name, O_WRONLY);       // Retorna ao cliente o identificador do programa
            if(fdC == -1) {
                perror("Erro no open do FIFO cliente: ");
                return -1;
            }

            if(write(fdC, &new->id, sizeof(int)) < 0) {
                perror("Nao escreveu: ");
            }
            close(fdC);          

            if(slots > 0) {
                printf("%d slots livres \n", slots);
                slots--;

                gettimeofday(&start_time, NULL);
                new->start_time = start_time.tv_sec * 1000 + start_time.tv_usec / 1000;

                insere(&executing, new, "fcfs");

                pid_t pid = fork();
                if(pid == 0) {
                    
                    struct program* retorno = (struct program*) malloc(sizeof(struct program));
                    retorno->type = 3;
                    retorno->state = new->state;
                    strcpy(retorno->args, new->args);
                    retorno->time = new->time;
                    retorno->start_time = new->start_time;
                    retorno->id = new->id;
                    retorno->pid = getpid();
                    retorno->next = NULL;

                    printf("Vamos executar o %d \n", new->id);

                    char outputfile[20];
                    sprintf(outputfile, "../%s/%d.txt", output_folder, new->id);
                    //printf("%s \n", outputfile);
                    int out_fd = open(outputfile, O_CREAT | O_TRUNC | O_WRONLY, 0666);
                    if(out_fd == -1) {
                        perror("Erro no open do ficheiro que contem os programas terminados: ");
                        return -1;
                    }

                    int fdout = dup(1);
                    int fderr = dup(2);

                    dup2(out_fd, 1);
                    dup2(out_fd, 2);
                    close(out_fd);

                    int res;
                    if(new->type == 0) {
                        res = executec(new->args);
                    } else {
                        res = executep(new->args);
                    }
                    
                    dup2(fdout, 1);
                    dup2(fderr, 2);
                    close(fdout);
                    close(fderr);

                    int fdE = open("fifo", O_WRONLY);
                    if(fdE == -1) {
                        perror("Erro no open do fifo do Servidor: ");
                        return -1;
                    }
                    write(fdE, retorno, sizeof(struct program));
                    close(fdE);

                    free(retorno);
                    _exit(res);
                }
            }
            else {
                gettimeofday(&start_time, NULL);
                new->start_time = start_time.tv_sec * 1000 + start_time.tv_usec / 1000;

                insere(&ready, new, sched_policy);
            }               

        }
        else if(new->type == 3) {            //Retorno para recolher o estado
            waitpid(new->pid, NULL, 0);

            if(new->id != -1) {              // Caso em que é o retorno de um execute e nao de um status
                gettimeofday(&end_time, NULL);
                int time_end = end_time.tv_sec * 1000 + end_time.tv_usec / 1000;
                
                Wrt w;
                w.pid = new->id;
                strcpy(w.args, new->args);
                w.time = time_end - new->start_time;

                int fdes = open(TERMINADOS, O_CREAT | O_WRONLY, 0666);
                if(fdes <= -1) {
                    perror("Erro no open do ficheiro que contem os programas terminados:");
                    return -1;
                }

                lseek(fdes, 0, SEEK_END);
                write(fdes, &w, sizeof(Wrt));
                
                int close_result = close(fdes);
                if(close_result == -1) {
                    perror("Erro no close");
                    return -1;
                }
                printf("Terminou o %d. \n", new->id);
                removeExecuting(&executing, new->id);

                if(ready == NULL) {
                    slots++;
                    printf("%d slots livres \n", slots);
                }
                else {
                    new = removeq(&ready);
                    
                    printf("Vamos executar o %d \n", new->id);

                    insere(&executing, new, "fcfs");

                    pid_t pid = fork();
                    if(pid == 0) {
                        struct program* retorno = (struct program*) malloc(sizeof(struct program));
                        retorno->type = 3;
                        retorno->state = new->state;
                        strcpy(retorno->args, new->args);
                        retorno->time = new->time;
                        retorno->start_time = new->start_time;
                        retorno->id = new->id;
                        retorno->pid = getpid();
                        retorno->next = NULL;

                        char outputfile[20];
                        sprintf(outputfile, "../%s/%d.txt", output_folder, new->id);
                        //printf("%s \n", outputfile);
                        int out_fd = open(outputfile, O_CREAT | O_TRUNC | O_WRONLY, 0666);
                        if(out_fd == -1) {
                            perror("Erro no open: ");
                            return -1;
                        }

                        int fdout = dup(1);
                        int fderr = dup(2);

                        dup2(out_fd, 1);
                        dup2(out_fd, 2);
                        close(out_fd);

                        int res;
                        if(new->type == 0) {
                            res = executec(new->args);
                        } else {
                            res = executep(new->args);
                        }
                        
                        dup2(fdout, 1);
                        dup2(fderr, 2);
                        close(fdout);
                        close(fderr);

                        int fdE = open("fifo", O_WRONLY);
                        if(fdE == -1) {
                            perror("Erro no open do fifo do Servidor: ");
                            return -1;
                        }
                        write(fdE, retorno, sizeof(struct program));
                        close(fdE);
                        free(retorno);

                        _exit(res);
                    }
                }
            }

        }
        
    }
    return 0;
}