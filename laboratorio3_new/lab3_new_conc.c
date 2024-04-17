#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include "timer.h"

// Definição de estruturas e variáveis globais
float *matA; // Matriz de entrada A
float *matB; // Matriz de entrada B
float *matC; // Matriz de saída C
int linhasA, colunasA, linhasB, colunasB; // Dimensões das matrizes de entrada
int nthreads; // Número de threads

// Estrutura para passar argumentos para as threads
typedef struct{
   int id; // Identificador da thread
   int dim; // Dimensão da matriz de entrada
} tArgs;

// Função que as threads executarão
void * tarefa(void *arg) {
   tArgs *args = (tArgs*) arg;
   //printf("Thread %d\n", args->id);
   for(int i=args->id; i<args->dim; i+=nthreads)
      for(int j=0; j<colunasB; j++) 
         for(int k=0; k<colunasA; k++) 
            matC[i*colunasB + j] += matA[i*colunasA + k] * matB[k*colunasB + j];
   pthread_exit(NULL);
}

// Função principal
int main(int argc, char* argv[]) {
   double inicio_inicializacao, fim_inicializacao, delta_inicializacao;
   double inicio_processamento, fim_processamento, delta_processamento;
   double inicio_finalizacao, fim_finalizacao, delta_finalizacao;
   double delta_execucao;
   int dimensoes_linhas, dimensoes_colunas; // para colocar as dimensões da matriz


   //Marcação do tempo inicial de iniciaização:
   GET_TIME(inicio_inicializacao);
 
   // Verificação dos parâmetros de entrada
   if(argc < 4) {
      printf("Digite: %s <arquivo matriz A> <arquivo matriz B> <arquivo saida> <numero de threads>\n", argv[0]);
      return 1;
   }

   // Leitura dos argumentos de entrada
   char *nomeArquivoA = argv[1];
   char *nomeArquivoB = argv[2];
   char *nomeArquivoSaida = argv[3];
   nthreads = atoi(argv[4]);

   // Leitura das matrizes de entrada A e B
   FILE *arquivoA = fopen(nomeArquivoA, "rb");
   FILE *arquivoB = fopen(nomeArquivoB, "rb");

   if(!arquivoA || !arquivoB) {
      printf("Erro ao abrir arquivo de entrada\n");
      return 2;
   }

   // Leitura das dimensões das matrizes A e B
   fread(&linhasA, sizeof(int), 1, arquivoA);
   fread(&colunasA, sizeof(int), 1, arquivoA);
   fread(&linhasB, sizeof(int), 1, arquivoB);
   fread(&colunasB, sizeof(int), 1, arquivoB);

   // colocando dimensões da matriz nas variáveis
   dimensoes_linhas = linhasA;
   dimensoes_colunas = colunasB;

   // Verificação das dimensões das matrizes
   if(colunasA != linhasB) {
      printf("Erro: Dimensões das matrizes incompatíveis para multiplicação\n");
      return 3;
   }

   // Alocação de memória para as matrizes de entrada A e B e a matriz de saída C
   matA = (float *) malloc(sizeof(float) * linhasA * colunasA);
   matB = (float *) malloc(sizeof(float) * linhasB * colunasB);
   matC = (float *) calloc(linhasA * colunasB, sizeof(float));

   if(matA == NULL || matB == NULL || matC == NULL) {
      printf("Erro ao alocar memória\n");
      return 4;
   }

   // Leitura das matrizes de entrada A e B dos arquivos
   fread(matA, sizeof(float), linhasA * colunasA, arquivoA);
   fread(matB, sizeof(float), linhasB * colunasB, arquivoB);

   // Fechamento dos arquivos de entrada
   fclose(arquivoA);
   fclose(arquivoB);

   //Marcação do tempo final de iniciaização:
    GET_TIME(fim_inicializacao); 

   // Cálculo do tempo de inicialização
   delta_inicializacao = fim_inicializacao - inicio_inicializacao;

   // Impressão do tempo total de processamento (paralelismo)
   // printf("Tempo de inicialização paralelo: %lf segundos\n", delta_inicializacao);


   // Marcação do tempo inicial de processamento (paralelismo)
   GET_TIME(inicio_processamento);

   // Criação das threads
   pthread_t *tid = (pthread_t*) malloc(sizeof(pthread_t) * nthreads);
   tArgs *args = (tArgs*) malloc(sizeof(tArgs) * nthreads);

   if(tid == NULL || args == NULL) {
      printf("Erro ao alocar memória para as threads\n");
      return 5;
   }

   // Inicialização das threads
   for(int i = 0; i < nthreads; i++) {
      (args+i)->id = i;
      (args+i)->dim = linhasA;
      if(pthread_create(tid+i, NULL, tarefa, (void*) (args+i))) {
         printf("Erro ao criar thread\n");
         return 6;
      }
   }

   // Espera pelo término das threads
   for(int i = 0; i < nthreads; i++) {
      pthread_join(*(tid+i), NULL);
   }

   // Marcação do tempo final de processamento (paralelismo)
   GET_TIME(fim_processamento);

   // Cálculo do tempo de processamento
   delta_processamento = fim_processamento - inicio_processamento;

   // Impressão do tempo total de processamento (paralelismo)
   // printf("Tempo de processamento paralelo: %lf segundos\n", delta_processamento);

   // Marcação do tempo inicial da finalização do programa 
   GET_TIME(inicio_finalizacao);

   // Escrita da matriz de saída no arquivo de saída
   FILE *arquivoSaida = fopen(nomeArquivoSaida, "wb");

   if(!arquivoSaida) {
      printf("Erro ao abrir arquivo de saída\n");
      return 7;
   }

   // Escrita das dimensões da matriz de saída no arquivo
   fwrite(&linhasA, sizeof(int), 1, arquivoSaida);
   fwrite(&colunasB, sizeof(int), 1, arquivoSaida);

   // Escrita da matriz de saída no arquivo
   fwrite(matC, sizeof(float), linhasA * colunasB, arquivoSaida);

   // Fechamento do arquivo de saída
   fclose(arquivoSaida);

   // Liberação da memória alocada
   free(matA);
   free(matB);
   free(matC);
   free(args);
   free(tid);

   // Marcação do tempo final da finalização do programa 
   GET_TIME(fim_finalizacao);

   // Cálculo do tempo de finalização
   delta_finalizacao = fim_finalizacao - inicio_finalizacao;

   // Impressão do tempo total de processamento (paralelismo)
   // printf("Tempo de finalização paralelo: %lf segundos\n", delta_finalizacao);


   // Cálculo do tempo de execução
   delta_execucao = delta_inicializacao + delta_processamento + delta_finalizacao;
   
   // Impressão do tempo total de execução
   printf("%lf, %d , %dx%d\n", delta_execucao, nthreads, dimensoes_linhas, dimensoes_colunas);


   return 0;
}


