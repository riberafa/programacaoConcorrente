/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 3 */
/* Objetivo: Projetar e implementar um programa concorrente onde a ordem de execução das threads é 
  controlada no programa, usando variáveis de condição e o padrão barreira. */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
  long long int indice;
  int nthreads;
} t_args;

/* Variaveis globais */
int bloqueadas = 0;
pthread_mutex_t x_mutex;
pthread_cond_t x_cond;

/* Função Barreira */
void Barreira (int id, int nthreads) {
  pthread_mutex_lock(&x_mutex);
  if(bloqueadas == (nthreads - 1)){
    pthread_cond_broadcast(&x_cond);
    bloqueadas = 0;
  }
  else{
    bloqueadas++;
    pthread_cond_wait(&x_cond, &x_mutex);
  }
  pthread_mutex_unlock(&x_mutex);
}

/* Thread Principal */
void *thread_imprime_controlado (void *arg) {
  t_args *args = (t_args *) arg;    

  printf("Olá da Thread %lld\n", args->indice); 
  Barreira(args->indice, args->nthreads); 

  printf("Que dia Bonito %lld\n", args->indice);
  Barreira(args->indice, args->nthreads); 

  printf("Até breve da Thread %lld\n", args->indice);
  pthread_exit(NULL);
}

/* Funcao principal */
int main(int argc, char *argv[]) {

  //printa pra lembrar ao usuario que precisa dos inputs
  if (argc != 2) { 
    printf("Uso: %s <N> <T>\n", argv[0]);
    printf("T: número de threads\n");
    exit(-1);
  }

  //salva os input do usuário com o número de threads T
  long long int T = atoll(argv[1]);

  int t; 
  pthread_t *threads;
  t_args *args;

  //separa espaço vazio na memória para os argumentos
  args = (t_args *) malloc(sizeof(t_args) * T);
  if (args == NULL) {
    printf("--ERRO: malloc()\n");
    exit(-1);
  }

  //identificadores das threads no sistema
  threads = (pthread_t *) malloc(sizeof(pthread_t) * T);
  if (threads == NULL) {
    printf("--ERRO: malloc()\n");
    exit(-1);
  }

  /* Inicilaiza o mutex (lock de exclusao mutua) e a variavel de condicao */
  pthread_mutex_init(&x_mutex, NULL);
  pthread_cond_init (&x_cond, NULL);

  /* Cria as threads */
  for (t = 0; t < T; t++) {
    args[t].indice = t + 1;
    args[t].nthreads = T;
    if (pthread_create(&threads[t], NULL, thread_imprime_controlado, (void *) &args[t])) {
      printf("--ERRO: pthread_create()\n");
      exit(-1);
    }
  }

  /* Espera todas as threads completarem */
  for (t = 0; t < T; t++) {
    pthread_join(threads[t], NULL);
  }

  free(threads);
  free(args);

  /* Desaloca variaveis e termina */
  pthread_mutex_destroy(&x_mutex);
  pthread_cond_destroy(&x_cond);

  pthread_exit(NULL);

}