/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 2 */
/* Objetivo: PProjetar, implementar e avaliar uma versao concorrente para o problema de
somar todos os elementos de um vetor de numeros reais.  ́*/
  
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>

#define TESTE

//escopo global (par ser compartilhado com as threads)
float *vetor; //vetor de entrada

//define o tipo de dado de entrada das threads
typedef struct {
   int id; //id da thread
   long int tamBloco; //tamanho do bloco (cada thread processa um bloco)
} tArgs;

//define o tipo de retorno das threads
typedef struct {
   float soma;
} tSoma;


//fluxo das threads
void *tarefa(void *arg) {
    tArgs *args = (tArgs *)arg;

    tSoma *somaLocal; // Variável local com os resultados da thread
    somaLocal = (tSoma *)malloc(sizeof(tSoma));
    if (somaLocal == NULL) {
        pthread_exit(NULL);
    }

    long int ini = args->id * args->tamBloco; // Elemento inicial do bloco da thread
    long int fim = ini + args->tamBloco; // Elemento final (não processado) do bloco da thread

    // Soma os elementos do bloco da thread
    somaLocal->soma = 0;
    for (long int i = ini; i < fim; i++) {
        somaLocal->soma += vetor[i];
    }

    // Retorna o resultado da soma local
    pthread_exit((void *)somaLocal);
}

//fluxo principal
int main(int argc, char *argv[]) {
   float somaTotal; //valores encontrados pela solucao concorrente
   long int dim; //dimensao do vetor de entrada
   long int tamBloco; //tamanho do bloco de cada thread 
   long int restante; //tamanho do bloco de cada thread 
   int nthreads; //numero de threads que serao criadas
   pthread_t *tid; //vetor de identificadores das threads no sistema
   tSoma *retorno; //valor de retorno das threads

#ifdef TESTE
   float somaEsperada; //valores de referencia para os testes
#endif

   //recebe e valida os parametros de entrada (dimensao do vetor, numero de threads)
   if(argc < 2) {
       fprintf(stderr, "Digite: %s <numero threads>\n", argv[0]);
       return 1; 
   }
   nthreads = atoi(argv[1]);
   printf("nthreads=%d\n", nthreads); 

   //carrega o vetor de entrada
   scanf("%ld", &dim); //primeiro pergunta a dimensao do vetor
   printf("dim=%ld\n", dim); 

   //aloca o vetor de entrada
   vetor = (float*) malloc(sizeof(float)*dim);
   if(vetor == NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 2;
   }
   //preenche o vetor de entrada
   for(long int i=0; i<dim; i++)
      scanf("%f", &vetor[i]);
#ifdef TESTE

   //le os valores esperados 
   scanf("\n%f\n", &somaEsperada);
   for(long int i=0; i<dim; i++)
      printf("%f ", vetor[i]); // escreve no vetor na posição i 
#endif   

   //cria as threads
   tid = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
   if(tid==NULL) {
      fprintf(stderr, "ERRO--malloc\n");
      return 3;
   }
   tamBloco = dim/nthreads;  //tamanho do bloco de cada thread 
   restante = dim % nthreads; //pro restante dos blocos de divisões
   if (restante != 0) { // for pro caso de resto para as threads processarem
      int threadsComExtra = restante; 
      for (int i = 0; i < threadsComExtra; i++) {
         tArgs *args = (tArgs*) malloc(sizeof(tArgs));
         if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            pthread_exit(NULL);
         }
         args->id = i; 
         args->tamBloco = tamBloco + 1; // Cada uma das threads extras recebe um elemento extra
         if(pthread_create(tid+i, NULL, tarefa, (void*) args)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 5;
         }
      }
      for (int i = threadsComExtra; i < nthreads; i++) {
         tArgs *args = (tArgs*) malloc(sizeof(tArgs));
         if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            pthread_exit(NULL);
         }
         args->id = i; 
         args->tamBloco = tamBloco;
         if(pthread_create(tid+i, NULL, tarefa, (void*) args)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 5;
         }
      }
   } else { //for normal ideal, sem problemas com resto
      for(int i=0; i<nthreads; i++) {
         // Aloca e preenche argumentos para thread
         tArgs *args = (tArgs*) malloc(sizeof(tArgs));
         if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            pthread_exit(NULL);
         }
         args->id = i; 
         args->tamBloco = tamBloco;
         if(pthread_create(tid+i, NULL, tarefa, (void*) args)){
            fprintf(stderr, "ERRO--pthread_create\n");
            return 5;
         }
      }
   }   
  
   //aguarda o termino das threads
   for(int i=0; i<nthreads; i++) {
      if(pthread_join(*(tid+i), (void**) &retorno)){
         fprintf(stderr, "ERRO--pthread_create\n");
         return 6;
      }
      somaTotal += retorno->soma;
      free(retorno); // Libera a memória alocada para o retorno da thread
   }
   

   //exibir os resultados
   printf("\n\nSoma concorrente:%f \n", somaTotal);

#ifdef TESTE
   printf("\n\nSoma correta Esperada:%f \n", somaEsperada);
#endif

   //libera as areas de memoria alocadas
   free(vetor);
   free(tid);

   return 0;
}
