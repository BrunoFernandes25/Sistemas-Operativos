#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include "defs.h"

int main (int argc, char * argv[]){

	if (argc < 2) {
		printf("Missing argument.\n");
		_exit(1);
	}

	Msg m;
	m.needle = atoi(argv[1]);
	m.occurrences = 0;
	m.pid = getpid();

	char fifoc_name[30];
	sprintf(fifoc_name, CLIENT "%d",m.pid );								// Cria o nome do FIFO do cliente baseado no seu PID
	mkfifo(fifoc_name, 0666);

	int fdE = open(SERVER , O_WRONLY);										// Abre o FIFO do servidor para escrita
	if (fdE < 0) {
        perror("Nao conseguiu abrir o FIFO do servidor");
        return 1;
    }
	int written_bytes = write(fdE, &m, sizeof(Msg));						// Escreve a mensagem no FIFO do servidor
	if(written_bytes < 0) perror("Nao escreveu: ");
	close(fdE);																// Fecha o descritor de arquivo do FIFO do servidor
	
	int fdL = open(fifoc_name, O_RDONLY);									// Abre o FIFO do cliente para leitura
	if (fdL < 0) {
        perror("Nao conseguiu abrir o FIFO do cliente");
        return 1;
    }

	int read_bytes = read(fdL, &m, sizeof(Msg));							// Lê a resposta do servidor
	if(read_bytes < 0) perror("Nao leu: ");
	close(fdL);																// Fecha o descritor de arquivo do FIFO do cliente

	printf("O valor %d tem %d ocorrencias \n", m.needle, m.occurrences);
	unlink(fifoc_name);														// Remove o FIFO do cliente
	return 0;
}
