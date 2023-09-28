/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 4 */
/* Objetivo: Praticar a implementação de padrões clássicos de programação concorrente usando locks e variáveis de condição. Nesta semana, vamos trabalhar com o problema dos leitores/escritores. */
  
  #include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "list_int.h" // Suponho que você tenha uma biblioteca personalizada para manipular a lista
#include "timer.h"    // Suponho que você tenha uma biblioteca personalizada para medir o tempo

#define QTDE_OPS 10000000 //quantidade de operacoes sobre a lista (leitura e escrita)
#define QTDE_INI 100
#define MAX_VALUE 100

struct list_node_s* head_p = NULL;
int nthreads;
int num_leit = 0;           // Contador de leitores atualmente lendo
int num_escr = 0;           // Contador de escritores atualmente escrevendo
pthread_mutex_t mutex;      // Mutex para proteger as variáveis compartilhadas
pthread_cond_t cond_leit;   // Variável de condição para leitores
pthread_cond_t cond_escr;   // Variável de condição para escritores
int leitores_ativos = 0;    // Contador de leitores ativos no momento

void leitor(int id) {
    pthread_mutex_lock(&mutex);
    while (num_escr > 0) {
        pthread_cond_wait(&cond_leit, &mutex); // Aguarda se houver escritores ativos
    }
    num_leit++;
    leitores_ativos++;  //nem printei mas era só pra armazenar a quantidade de threads lendo
    pthread_mutex_unlock(&mutex);

    Member(id % MAX_VALUE, head_p); // Operação de leitura

    pthread_mutex_lock(&mutex);
    num_leit--;
    if (num_leit == 0) {
        pthread_cond_signal(&cond_escr); // Sinaliza para escritores que não há leitores 
    }
    leitores_ativos--;
    pthread_mutex_unlock(&mutex);

}

void escritor(int id) {
    pthread_mutex_lock(&mutex);
    while (num_leit > 0 || num_escr > 0) {
        pthread_cond_wait(&cond_escr, &mutex); // Aguarda se houver leitores ou escritores ativos
    }
    num_escr++;

    pthread_mutex_unlock(&mutex);

    Insert(id % MAX_VALUE, &head_p); // Operação de escrita

    pthread_mutex_lock(&mutex);
    num_escr--;
    pthread_cond_broadcast(&cond_leit); // Libera leitores bloqueados
    pthread_cond_signal(&cond_escr);    // Libera outros escritores bloqueados
    pthread_mutex_unlock(&mutex);
}

//tarefa das threads
void* tarefa(void* arg) {
   long int id = (long int) arg;
   int op;
   int in, read; 
   in=read = 0; 
   //realiza operacoes de consulta (98%), insercao (1%) e remocao (1%)
    for(long int i = id; i < QTDE_OPS; i += nthreads) {
      op = rand() % 100;
      if(op < 98) {
        leitor(i); 
        read++;
      } else {
        escritor(i);   
        in++;
      }
   }
   //registra a qtde de operacoes realizadas por tipo
   printf("Thread %ld: in=%d read=%d\n", id, in, read);
   pthread_exit(NULL);
}

int main(int argc, char* argv[]) {
    pthread_t* tid;
    double ini, fim, delta;

    //verificação pro usuário:
    if (argc < 2) {
        printf("Digite: %s <numero de threads>\n", argv[0]);
        return 1;
    }
    nthreads = atoi(argv[1]);

    for (int i = 0; i < QTDE_INI; i++) {
        Insert(i % MAX_VALUE, &head_p); // Inicializa a lista com valores
    }

    GET_TIME(ini);

    tid = malloc(sizeof(pthread_t) * nthreads);
    if (tid == NULL) {
        printf("--ERRO: malloc()\n");  // alocação das threads
        return 2;
    }

    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond_leit, NULL);
    pthread_cond_init(&cond_escr, NULL);

    //cria as threads
    for(long int i=0; i<nthreads; i++) {
        if(pthread_create(tid+i, NULL, tarefa, (void*) i)) {
            printf("--ERRO: pthread_create()\n"); return 3;
        }
    }
   

    for (int i = 0; i < nthreads; i++) {
        if (pthread_join(*(tid + i), NULL)) {
            printf("--ERRO: pthread_join()\n");
            return 4;
        }
    }

    GET_TIME(fim);
    delta = fim - ini;
    printf("Tempo: %lf\n", delta);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond_leit);
    pthread_cond_destroy(&cond_escr);
    free(tid);
    Free_list(&head_p);
    pthread_exit(NULL);
}