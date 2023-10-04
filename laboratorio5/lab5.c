/* Disciplina: Programação Concorrente */
/* Prof.: Silvana Rossetto */
/* Aluno : Rafael Ribeiro */
/* Projetar e implementar um programa concorrente em C que usa o padrão produtor/consumidor. */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h> // pra função strdup  
#include <unistd.h> // Pra função sleep

#define N 5 // Tamanho do buffer
#define MAX_LINHAS_TAM 100 // Tamanho máximo das linhas

char Buffer[N][MAX_LINHAS_TAM]; // Buffer
int in = 0; // Índice de inserção no buffer
int out = 0; // Índice de remoção no buffer
int fim_arquivo = 0; // Se o arquivo foi lido completamente

sem_t mutexGeral; // Semáforo para exclusão mútua
sem_t slotCheio; // Semáforo para indicar que há slots cheios no buffer
sem_t slotVazio; // Semáforo para indicar que há slots vazios no buffer

// Imprime o buffer
void printBuffer() {
    for (int i = 0; i < N; i++){
        printf("%s", Buffer[i]);
    }
}        

// Função para inserir uma linha no buffer
void Insere(char *linha) {
    sem_wait(&slotVazio); // Aguarda um slot vazio no buffer para inserção
    sem_wait(&mutexGeral); 
    strcpy(Buffer[in], linha); // Copia a linha para o buffer
    in = (in + 1) % N; // Atualiza o índice
    sem_post(&mutexGeral); 
    sem_post(&slotCheio); // Sinaliza que há um slot cheio no buffer
}

// Função para retirar uma linha do buffer
void Retira(char *linha) {
    sem_wait(&slotCheio); // Aguarda um slot cheio no buffer para retirada
    sem_wait(&mutexGeral); 
    strcpy(linha, Buffer[out]); // Copia a linha do buffer e joga na variável linha 
    Buffer[out][0] = '0'; // Limpa o slot no buffer
    out = (out + 1) % N; // Atualiza o índice 
    sem_post(&mutexGeral); 
    sem_post(&slotVazio); // Sinaliza o slot vazio no buffer
}

void *Produtor(void *arg) {
    FILE *fp;
    char linha[MAX_LINHAS_TAM];

    fp = fopen((char *)arg, "r"); // Abre o arquivo  para leitura
    if (fp == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    while (fgets(linha, MAX_LINHAS_TAM, fp)) { // ler linhas do arquivo e armazena em Linha
        char *nova_linha = strdup(linha); // Aloca memória e copia a linha do arquivo
        Insere(nova_linha); // Insere a linha no buffer compartilhado
        sleep(1); 
    }

    fclose(fp); // Fecha o arquivo
    fim_arquivo = 1; // Indica que o arquivo foi lido completamente

    pthread_exit(NULL);
}

void *Consumidor(void *arg) {
    char linha[MAX_LINHAS_TAM];

    //verifica o arquivo não foi lido completamente para seguir o loop
    while (!fim_arquivo || out != in) { // se in e out são iguais sign que o buffer ta vazio
        Retira(linha); // Retira uma linha do buffer
        printf("Consumidor: %s", linha); // Imprime a linha retirada
        sleep(1); 
    }

    pthread_exit(NULL);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: %s <arquivo_leitura> <num_threads>\n", argv[0]);
        return 1;
    }

    int num_threads = atoi(argv[2]); // Obtém o número de threads a partir da linha de comando

    // Declaração das threads
    pthread_t threads[num_threads];
    pthread_t produtor;

    sem_init(&mutexGeral, 0, 1); // Inicializa o semáforo para exclusão mútua
    sem_init(&slotCheio, 0, 0); // Inicializa o semáforo indicando que o buffer está vazio
    sem_init(&slotVazio, 0, N); // Inicializa o semáforo indicando que todos os slots estão vazios

    // Cria a thread produtora
    pthread_create(&produtor, NULL, Produtor, (void *)argv[1]);

    int i;
    // Cria as threads consumidoras
    for (i = 0; i < num_threads; i++) {
        pthread_create(&threads[i], NULL, Consumidor, NULL);
    }

    // Aguarda o término das threads
    for (i = 0; i < num_threads; i++) {
        pthread_join(threads[i], NULL);
    }
    pthread_join(produtor, NULL); // Aguarda o término da thread produtora

    sem_destroy(&mutexGeral);
    sem_destroy(&slotCheio);
    sem_destroy(&slotVazio);

    pthread_exit(NULL);

}
