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

void lookupNumber(int** matrix, int value, int* vector){

    int fd[2];
    int pipe_ret = pipe(fd);

    for (int i = 0; i < ROWS; i++) {    
        pid_t pid = fork();     //a cada linha da matriz temos um processo que ira procurar o value
        if(pid == 0) {
            //printf("[child-%d] searching row %d\n",getpid(),i);
            close(fd[0]);       
            Minfo mat;  //estrutura usada 
            mat.line_nr = i;  //indicador de linha atual
            mat.ocur_nr = 0;  //numero ocorrencias inicialmente é nula
            //int counter = 0;
            //mat.line_nr = i;
            for(int j = 0; j<COLUMNS; j++) {    //iteraçao pelas colunas da matriz
                if( matrix[i][j] == value) mat.ocur_nr++;   //caso seja igual num_ocur é incrementado
                //if( matrix[i][j] == value) counter++;
            }
            write(fd[1], &mat, sizeof(Minfo));  //com a struct a ser enviada sabemos que filho é que enviou e o num_ocur
            //coisa que assim nao conseguiriamos
            //mat.ocur_nr = counter;
            //write(fd[1], &counter, sizeof(int));   aqui apenas iriamos saber o num_ocur e o filho?? nada se sabia
            close(fd[1]);   //terminando de escrever close(fd[1])
            //printf("[child-%d] done (found=%d)\n",getpid(),mat.ocur_nr);
            _exit(0);
        } 
    }
    close(fd[1]);

    int bytes_read;
    Minfo matr;
    while((bytes_read = read(fd[0], &matr, sizeof(Minfo))) > 0) {   //ciclo para ler e colocar no vetor o num_ocur
        vector[matr.line_nr] = matr.ocur_nr;
    }
    close(fd[0]);

    int status;        //no fim nao esquecer do wait para cada processo
    for(int i = 0; i < ROWS; i++) {
        pid_t terminated_pid = wait(&status);
    }

}