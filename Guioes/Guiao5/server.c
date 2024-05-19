#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"
#include "vector.h"

//FIFO criado pelo servidor
//Cliente pode receber um sigpipe (concorrência!)

int main (int argc, char * argv[]){

	init_vector();
	print_vector();

	int fifo_ret = mkfifo(SERVER, 0666);
    if(fifo_ret == -1) {
        perror("Erro ao criar o fifo:");
        return -1;
    }

	while(1) {
		Msg m;
		int status;

		int fdL = open(SERVER, O_RDONLY);							// Abre o FIFO do servidor para leitura e lê a mensagem do cliente
		int read_bytes = read(fdL, &m, sizeof(Msg)); 
		if(read_bytes <= 0) perror("ERRO: ");

		if(m.type == 0) {											// Verifica se a mensagem é um novo pedido (type == 0)
			if(fork() == 0) {			     						// Cria um novo processo filho para tratar o pedido
				int count = count_needle(m.needle);					// Conta as ocorrências do valor "needle" no vetor
				m.occurrences = count;
				
				//Escrita para o Cliente
				char fifoc_name[30];		
				sprintf(fifoc_name, CLIENT "%d",m.pid);				// Prepara o nome do FIFO do cliente usando o PID do cliente
				printf("--- %s \n", fifoc_name);
				int fdC = open(fifoc_name, O_WRONLY);				// Abre o FIFO do cliente para escrita 
				int written_bytes = write(fdC, &m, sizeof(Msg));	// Envia a resposta
				if(written_bytes < 0) {perror("Nao escreveu: ");}
				close(fdC);

				//Escrita para o Server
				int fdw = open(SERVER, O_WRONLY);			 		// Abre o FIFO do servidor para escrita para enviar uma mensagem de status
				m.pid = getpid();
				m.occurrences= count;
				m.type = 1;
				write(fdw, &m, sizeof(Msg));
				close(fdw);

				
				_exit(0);

			}
		}
		else {
			waitpid(m.pid, &status, 0);								// Espera pelo término do processo filho e recolhe o status
			printf("Recolher estado do %d \n", m.needle);
			int file = open("log.txt", O_CREAT | O_WRONLY, 0666);	// Abre o arquivo de log para escrita e grava a mensagem
			lseek(file, SEEK_END, 0);								// Move o ponteiro para o final do arquivo
			int bytes_written = write(file, &m, sizeof(Msg));
			if(bytes_written > 0) printf("Recolhido \n");
			close(file);
			
		}
		close(fdL);
	}
	return 0;
}


//OPEN é bloqueante servidor à espera de clientes que se, no read o servidor tá bloqueado à espera de clientes ligados
