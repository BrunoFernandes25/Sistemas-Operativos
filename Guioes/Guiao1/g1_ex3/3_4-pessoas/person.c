#include "person.h"
#include <stdio.h>

/*
Considere uma estrutura (struct) com dados de uma pessoa (nome, idade, ...) e um ficheiro binário que
contém registos organizados segundo esta estrutura. 
Não assuma que o ficheiro cabe todo em memória.

Escreva um programa que, consoante a opção, permita:

-i - Acrescentar pessoas a um ficheiro de dados binário.
-l - Listar as N primeiras pessoas no ficheiro de dados binário.
-u - Atualizar a idade de uma determinada pessoa no ficheiro de dados.

Exemplos:

$ pessoas -i "José Mourinho" 59
$ pessoas -l 10
$ pessoas -u "José Mourinho" 60

Meça o tempo que demora a inserir 1.000.000 de pessoas (nomes e idades gerados por si).

*/

/*
typedef struct person {
    char name[200];
    int age;
} Person;
*/

int new_person (char *name, int age) {
    Person p;
    p.age = age;
    strcpy(p.name, name);
     
    int fd = open(FILENAME, O_CREAT | O_WRONLY, 0644);          //Ficheiro definido para ter a informação sobre as Pessoas
    if(fd <= -1) {
        perror("Erro no open");
        return -1;
    }

    off_t offset = lseek(fd, 0, SEEK_END);                      // Move o ponteiro de arquivo para o final do arquivo, i.e, End é o nosso novo Inicio de ficheiro(0)
    ssize_t bytes_writen = write(fd, &p, sizeof(Person));       // Escreve a nova pessoa no final do arquivo
    printf("id: %d \n", offset/sizeof(Person));                 // Calcula e imprime o ID da pessoa com base no deslocamento
    
    int close_result = close(fd);
    if(close_result == -1) {
        perror("Erro no close");
        return -1;
    }

    return close_result;
}

int list_n_persons(int n) {
    Person p;

    int fd = open(FILENAME, O_RDONLY);

    int i = 0;
    while(read(fd, &p, sizeof(Person)) > 0 && i<n) {            //tamanho de bytes que queremos ler é o tamanho de Person e o ponteiro para um buffer onde os dados lidos serão armazenados é p
        printf("Registo %d {name: %s, age: %d} \n", i, p.name, p.age);
        i++;
    }

    int close_result = close(fd);
    if(close_result == -1) {
        perror("Erro no close");
        return -1;
    }

    return close_result;

}

//para procurar o nome é percorrer as pessoas ate encontrar o nome usando "strcmp"
int person_change_age(char *name, int age) {
    Person p;
    int fd = open(FILENAME, O_RDWR, 0600);

    while(read(fd, &p, sizeof(Person)) > 0) {
        if(!strcmp(p.name,name)) {
            p.age = age;
            lseek(fd, -(sizeof(Person)), SEEK_CUR);     // move o ponteiro fd de volta para o início do registro atual relativamente à posição atual do ponteiro(SEEK_CUR)
            write(fd, &p, sizeof(Person));              // sizeof(Person) move o ponteiro para trás do tamanho de uma estrutura Person, posicionando-o no início do registro atual.
        }
    }

    int close_result = close(fd); 
    if(close_result == -1) {
        perror("Erro no close");
        return -1;
    }

    return close_result;

}

/*

Faça com que a opção -i diga qual a posição no ficheiro do registo inserido e acrescente a possibilidade de atualizar a idade de registos por essa posição 
com a opção -o.

Exemplo:
$ pessoas -i "José Mourinho" 59
registo 973
$ pessoas -o 973 60
*/

int person_change_age_v2(int idx, int age) {
    int fd = open(FILENAME, O_RDWR, 0600);
    Person p;
    
    lseek(fd, sizeof(Person)*(idx), SEEK_SET);      // Leva-nos o cursor para a posicao indicada correspondente por idx   (SEEK_SET referancia como o início do arquivo)
    read(fd, &p, sizeof(Person));                   // Lê o registo da posição atual do cursor
    
    p.age = age;                                    // Atualizamos a idade nessa pessoa
    
    lseek(fd, -sizeof(Person), SEEK_CUR);           // Move o cursor de volta ao início do registo atual
    write(fd,&p,sizeof(Person));                    // Escreve o registo atualizado de volta ao arquivo
    
    int close_result = close(fd); 
    if(close_result == -1) {
        perror("Erro no close");
        return -1;
    }

    return close_result;

}


//cd 3_4-pessoas
//make
//./psript.sh 10 cria 10 pessoas
//./pessoas -i "..." 10 cria pessoa ... com idade 10
//./pessoas -l 11 lista 11 nomes 
//./pessoas -u "..." 15 altera idade da pessoa ... para 15
//./pessoas -o 10 100 altera a idade do registo 10 para 100