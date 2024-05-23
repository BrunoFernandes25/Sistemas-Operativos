#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#define SERVER "Server_fifo" 

typedef struct {
    char filtro[20];
    char entrada[20];
    char saida[20];
} pedido_client;

int main(int argc, char *argv[]) {

    pedido_client pedido;
    strncpy(pedido.filtro, argv[1], sizeof(pedido.filtro) - 1);
    strncpy(pedido.entrada, argv[2], sizeof(pedido.entrada) - 1);
    strncpy(pedido.saida, argv[3], sizeof(pedido.saida) - 1);

    int fd = open(SERVER, O_WRONLY);
    if (fd == -1) {
        perror("Erro ao abrir o pipe:");
        return -1;
    }

    write(fd, &pedido, sizeof(pedido_client));
    close(fd);

    return 0;
}