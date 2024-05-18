#include <stdio.h>
#include "mysystem.h"

void controller(int N, char** commands) {
	pid_t pid;
	int res, status;
	int pids[N];
	int counter;

	for(int i = 0; i < N; i++) {
		pid = fork();
		res = 1;
		counter = 0;
		if(pid == 0) {
			while(res>0) {
				res = mysystem(commands[i]);
				counter++;
			}
			_exit(counter);
		} else {
		pids[i] = pid;
		} 
	}
	for(int i = 0; i < N; i++) {
		waitpid(pids[i], &status, 0);
		if(WIFEXITED(status)) {
			printf("Command %s executed %d times", commands[i], WEXITSTATUS(status));
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