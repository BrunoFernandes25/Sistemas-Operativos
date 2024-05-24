#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAX_PROG 300    //Assumimos que o tamanho total dos argumentos passados à opção execute não excede os 300 bytes, como é referido no enunciado

struct program
{
    int type;               // Execute -u (0) || Execute -p (1) || Status (2) || request (3)
    int state;
    char args[MAX_PROG];
    int time;
    int start_time;
    int id;
    int pid;
    struct program* next;  
};

typedef struct msg{
    int state;
    int pid;
    char args[MAX_PROG];
    int time;
} Msg;


int main (int argc, char** argv){

    if( argc<2){
        printf("Input Error !!\n");
        return -1;
    }
    
    int fifo_fd = open("fifo",O_WRONLY);
    if (fifo_fd < 0) { 
        perror("Erro na criação do FIFO:");
        return -1;
    }
    
    if(!strcmp(argv[1],"execute")){
        
        struct program* info = (struct program*) malloc(sizeof(struct program));

        info->id = getpid();
        info->pid = 0;

        //Criamos um fifo de retorno com nome pid de processo para nao haver trocas no destinatario de envio pelo servidor
        char fifoc_name[30];
	    sprintf(fifoc_name, "%d", info->id );
	    if(mkfifo(fifoc_name, 0666) == -1) perror("Erro na criação do FIFO: ");

        info->state = 0;
        info->start_time = 0;
        strcpy(info->args, argv[4]);
        info->time = atoi(argv[2]);

        if (!strcmp(argv[3],"-u")){     //unico programa: ./client execute 100 -u "prog-a arg-1 (...) arg-n"
            info->type = 0;
        }
        else if(!strcmp(argv[3],"-p")){     //varios programas: ./client execute 3000 -p "prog-a arg-1 (...) arg-n | prog-b arg-1 (...) arg-n | prog-c arg-1 (...) arg-n"
            info->type = 1;
        }
        else{
            perror("Input Error");
        }
        info->next = NULL;

        int written_bytes = write(fifo_fd, info, sizeof(struct program));
        if(written_bytes < 1) {
            printf("Nao escreveu \n");
            return -1;
        }
        close(fifo_fd);

        int fdL = open(fifoc_name, O_RDONLY);
        int id;
        int read_bytes = read(fdL, &id, sizeof(int));
	    if(read_bytes < 0) {perror("Nao leu: ");}
        printf("TASK %d Received \n", id);
        close(fdL);
        unlink(fifoc_name);
    }

    else if(!strcmp(argv[1],"status")) {
        struct program* info = (struct program*) malloc(sizeof(struct program));

        info->id = getpid();
        info->pid = 0;

        //Criamos um fifo de retorno com nome pid de processo para nao haver trocas no destinatario de envio pelo servidor
        char fifo_name[30];
	    sprintf(fifo_name, "%d", info->id );
	    if(mkfifo(fifo_name, 0666) == -1) { 
            perror("Erro na criação do FIFO:");
            return -1;
        }
        
        info->state = 0;
        strcpy(info->args, "status");
        info->time = 0;
        info->type = 2;
        info->next = NULL;
        
        int written_bytes = write(fifo_fd, info, sizeof(struct program));
        if(written_bytes < 1) {
            printf("Nao escreveu \n");
            return -1;
        }

        close(fifo_fd);


        int fd = open(fifo_name, O_RDONLY);
        int read_bytes;

        Msg m;
        int state = -1;
        while((read_bytes = read(fd, &m, sizeof(Msg))) > 0){

            if(m.state == 1) {
                if(m.state != state) {
                    printf("Executing \n");
                    state = 1;
                }
                printf(" %d %s \n", m.pid, m.args);
            }

            if(m.state == 0) {
                if(m.state != state) {
                    printf("Scheduled \n");
                    state = 0;
                }
                printf(" %d %s \n", m.pid, m.args);
            }
            
            if(m.state == 2) {
                if(m.state != state) {
                    printf("Completed \n");
                    state = 2;
                }
                printf(" %d %s %d ms \n", m.pid, m.args, m.time);
            }
        }
        close(fd);
        unlink(fifo_name);
    }
    
    return 0;
}

