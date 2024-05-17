#include <stdio.h>
#include "mysystem.h"

void controller(int N, char** commands) {
	
	pid_t pid;
	int res,status;

	for(int i = 0;i<N;i++){
		pid = fork();
		if(pid == 0){
			res = mysystem(commands[i]);
		
			if (res == -1){
            	perror("Erro no exec !");
        	}
			_exit(res);
		}
	}

	for(int i=0;i<N;i++){
		wait(&status);
		if(WIFEXITED(status)){	//caso termine de forma graciosa:
            if(WEXITSTATUS(status) != 255){
				res = WEXITSTATUS(status);
			}
			else{
				res = -1;
			}
        }
	}

}

int main(int argc, char* argv[]) {

	char *commands[argc-1];
	int N = 0;
	for(int i=1; i < argc; i++){
		commands[N] = strdup(argv[i]);
		printf("command[%d] = %s\n", N, commands[N]);
		N++;
	}

	controller(N, commands);

	return 0;
}


//ao executar makefile automaticamente controlador.c é executado e sao criados a.out b.out e c.out