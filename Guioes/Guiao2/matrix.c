#include "matrix.h"


int **createMatrix() {

    // seed random numbers
    srand(time(NULL));

    // Allocate and populate matrix with random numbers.
    printf("Generating numbers from 0 to %d...", MAX_RAND);
    int **matrix = (int **) malloc(sizeof(int*) * ROWS);
    for (int i = 0; i < ROWS; i++) {
        matrix[i] = (int*) malloc(sizeof(int) * COLUMNS);
        for (int j = 0; j < COLUMNS; j++) {
            matrix[i][j] = rand() % MAX_RAND;
        }
    }
    printf("Done.\n");

    return matrix;
}

void printMatrix(int **matrix) {

    for (int i = 0; i < ROWS; i++) {
        printf("%2d | ", i);
        for (int j = 0; j < COLUMNS; j++) {
            printf("%7d ", matrix[i][j]);
        }
        printf("\n");
    }
}

// ex.5
int valueExists(int **matrix, int value) {

    pid_t pid;
    int status;

    for(int i=0; i<ROWS; i++) {              // nao pomos o codigo do pai dentro do for para ser paralelo (nao ser sequencial), assim lançamos logo os "ROW" processos
        
        pid = fork();
        if(pid == 0) {
            for(int j = 0; j<COLUMNS; j++) {
                if(matrix[i][j] == value) _exit(i);
            }
            _exit(-1);
        }

    }

    for(int i = 0; i < ROWS; i++) {
        pid_t terminated_pid = wait(&status);
        
        if(WIFEXITED(status)) {                // Para ver se o processo terminou bem ou mal mal, nada tem haver com o exit status
            if(WEXITSTATUS(status) != 255) {
                printf("Iteração: %d encontrou o valor %d na linha %d \n", i, value, WEXITSTATUS(status));
            }
            else {
                printf("O processo %d nao encontrou o valor \n", terminated_pid);
            }
        }    
    }
    return 1;
}


// ex.6
void linesWithValue(int **matrix, int value) {

    pid_t pid;
    int status;
    int matches[ROWS] = { 0 };      //array que contem linhas que encontraram value
    int linha;

    for(int i=0; i<ROWS; i++) {          // nao colocamos o codigo do pai dentro do for para ser paralelo (e nao ser sequencial), assim lançamos logo os "ROW" processos
        
        pid = fork();
        if(pid == 0) {
            for(int j = 0; j<COLUMNS; j++) {
                if(matrix[i][j] == value) _exit(i);     // Sai com o número da linha se o valor for encontrado
            }
            _exit(-1);              // Sai com -1 se o valor for encontrado
        }

    }

    for(int i = 0; i < ROWS; i++) {
        pid_t terminated_pid = wait(&status);
        
        if(WIFEXITED(status)) {                // Para ver se o processo terminou bem ou mal mal, nada tem haver com o exit status
            linha = WEXITSTATUS(status); 
            if( linha != 255) {
                //printf("Iteração: %d encontrou o valor %d na linha %d \n", i, value, WEXITSTATUS(status));
                matches[linha] = 1;
            }
        }    
    }

    printf("Linhas com o valor %d:[", value);
    for (int i = 0; i < ROWS; i++) {
        if (matches[i]) {
            printf(" %d ", i);
        }
    }
    printf("]\n");
}

/*
void linesWithValue(int **matrix, int value) {
    
    pid_t pid;
    int status;
    pid_t children[ROWS]; 

    for(int i=0; i<ROWS; i++) {              // Nao pomos o codigo do pai dentro do for para ser paralelo (nao ser sequencial), assim lançamos logo os "ROW" processos
        
        pid = fork();
        if(pid == 0) {
            for(int j = 0; j<COLUMNS; j++) {
                if(matrix[i][j] == value) _exit(i);         //Valor encontrado  WEXITSTATUS != 255
            }
            _exit(-1);                                      //Valor não encontrado  WEXITSTATUS = 255
        } else {
            children[i] = pid;
        }

    }

    for(int i = 0; i < ROWS; i++) {
        pid_t terminated_pid = waitpid(children[i], &status, 0);    //children[i] = pid permitiu nos assim esperar corretamente a conclusão de cada processo filho
        
        if(WIFEXITED(status)) {                // Para ver se o processo terminou bem ou mal mal, nada tem haver com o exit status
            if(WEXITSTATUS(status) != 255) {
                printf("Iteração: %d encontrou o valor %d na linha %d \n", i, value, WEXITSTATUS(status));
            }
            else {
                printf("O processo %d nao encontrou o valor \n", terminated_pid);
            }
        }    
    }
}*/


//make 
// ./searchM