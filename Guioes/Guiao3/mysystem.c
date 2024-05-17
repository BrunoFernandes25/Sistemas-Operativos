#include "mysystem.h"

// recebe um comando por argumento
// returna -1 se o fork falhar
// caso contrario retorna o valor do comando executado

int mysystem (const char* command) {
	int status;
	int res = -1;
	int exec_ret;
	// Estamos a assumir numero maximo de argumentos
	// isto teria de ser melhorado com realloc por exemplo
	char *exec_args[20];
	char *string, *cmd, *tofree;
	int i=0;
	tofree = cmd = strdup(command);
	while((string = strsep(&cmd," "))!=NULL){
	   exec_args[i]=string;
	   i++;
	}

	exec_args[i]=NULL;

	pid_t pid = fork();
	switch (pid) {
		case -1: // correu mal o fork
			break;

		case 0:  // codigo processo filho
			exec_ret = execvp(exec_args[0], exec_args);
			perror("Erro no exec");

			_exit(exec_ret);		//só retorna SE o execvp nao correr bem nao esquecer isso !!
			break;

		default: // codigo processo pai
			wait(&status);					//esperar que o processo filho termine	
			if(WIFEXITED(status)) {			//caso termine de forma graciosa:
				if (WEXITSTATUS(status) == 255) {
					printf("- Nao funcionou \n");
					res = -1;
				} else {
					printf("- Executamos o comando system com o valor de saida: %d \n", WEXITSTATUS(status));	
					res = WEXITSTATUS(status);
				}
			}
			break;
	}	
	free(tofree);
	return res;
}