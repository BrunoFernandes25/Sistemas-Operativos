#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>


#define MAX_COMMANDS 10

// parse do argumento e execvp
// devolve o resultado do exec
int exec_command(char* arg){

	//Estamos a assumir numero maximo de argumentos
	char *exec_args[10];

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


int main(int argc, char** argv){
	
	int number_of_commands=4;
	
	char * commands[] = {
		"grep -v ^# /etc/passwd",
		"cut -f7 -d:",
		"uniq",
		"wc -l"
	};

	int pipes[number_of_commands-1][2];
	
	for(int i = 0; i < number_of_commands; i++) {
		
		if(i == 0){
			pipe(pipes[i]);

			switch(fork() == 0) {
			case 0:
				close(pipes[i][0]);
				dup2(pipes[i][1], 1);
				close(pipes[i][1]);
				exec_command(commands[i]);
				_exit(0);
				break;
			default:
				close(pipes[i][1]);
				break;
			}
		}
		else if(i == number_of_commands-1) {
			switch(fork() == 0) {
			case 0:
				dup2(pipes[i-1][0], 0);
				close(pipes[i-1][0]);
				exec_command(commands[i]);
				_exit(0);
				break;
			default:
				close(pipes[i-1][0]);
				break;
			}
		}
		else {
			pipe(pipes[i]);
			switch(fork() == 0) {
			case 0:
				dup2(pipes[i-1][0], 0);
				close(pipes[i-1][0]);
				dup2(pipes[i][1], 1);
				close(pipes[i][1]);
				exec_command(commands[i]);
				_exit(0);
				break;
			default:
				close(pipes[i-1][0]);
				close(pipes[i][1]);
				break;
			}
		}
	}

	for(int i = 0; i < number_of_commands; i++) {
		wait(NULL);
	}

	return 0;
}

