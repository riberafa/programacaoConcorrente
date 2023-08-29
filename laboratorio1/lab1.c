/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Objetivo: Implementar o seu primeiro programa concorrente! Escreva um programa
com 8 threads, para elevar ao quadrado cada elemento de um vetor de 10000 elementos.
(Para cada elemento ai do vetor, calcular o novo valor e escrever o resultado na mesma
posic ̧ao do elemento.)  ̃
Roteiro: */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

#define TAM_ARRAY 10000 // tamanho do array
#define NTHREADS  8 // total de threads a serem criadas

//cria a estrutura de dados para armazenar os argumentos da thread
typedef struct {
  int inicio_array, fim_array;
} t_Args;

// declarando o array que iremos usar
int* array_;
int* array_verificacao;

//funcao executada pelas threads
void* eleva_ao_quadrado_array (void* arg){
  t_Args *args = (t_Args*) arg;
  int inicio_array = args->inicio_array;
  int fim_array = args->fim_array;
  free(arg); //libera a memoria que foi alocada na main

  for(int i = inicio_array; i < fim_array; i += 1) {
    array_[i] = i*i; //elevando ao quadrado
    
  }
  pthread_exit(NULL);
}

//funcao principal do programa
int main() {
  array_ = calloc(TAM_ARRAY , sizeof(TAM_ARRAY)); //separa espaço vazio na memória para o array  
  pthread_t tid_sistema[NTHREADS]; //identificadores das threads no sistema
  t_Args *args; //receberá os argumentos para a thread

  for(int i=0; i<NTHREADS; i++) {
    printf("--Aloca e preenche argumentos para thread %d\n", i);
    args = malloc(sizeof(t_Args)); //separa espaço vazio na memória para os argumentos
    if (args == NULL) {
      printf("--ERRO: malloc()\n"); exit(-1);
    } 
    args ->inicio_array = TAM_ARRAY / NTHREADS * i; //pega o início de varreadura do array em cada Thread
    args ->fim_array = TAM_ARRAY / NTHREADS * (i+1); //pega o fim de varredura do array em cada Thread

    printf("--Cria a thread %d\n", i);
    if(pthread_create(&tid_sistema[i], NULL, eleva_ao_quadrado_array, (void*) args)){
      printf("--ERRO: pthread_create()\n"); exit(-1);
    }
  }

  //--espera todas as threads terminarem
  for (int i=0; i<NTHREADS; i++) {
    if (pthread_join(tid_sistema[i], NULL)) {
         printf("--ERRO: pthread_join() \n"); exit(-1); 
    } 
  }
  //função de verificação 
  for (int i=0; i<TAM_ARRAY; i++) {
    array_verificacao = calloc(TAM_ARRAY , sizeof(TAM_ARRAY)); //separa espaço vazio na memória para o array  
    array_verificacao[i] = i*i; //elevando ao quadrado

    if(array_verificacao[i] != array_[i]){
      printf("O array na posição %d. possui valor diferente.\n", i);
      return -1;
    }
  }
  printf("--O quadrado de cada elemento de um vetor de 10000 posições foi criado e verificado com sucesso! :)\n "); 

  printf("--Thread principal terminou\n");
  pthread_exit(NULL);
}
