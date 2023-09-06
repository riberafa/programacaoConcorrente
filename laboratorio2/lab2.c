#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"

pthread_mutex_t mutex; // variável de lock para exclusão mútua


int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

typedef struct {
    long long int inicio, fim;
} t_args;

int qtd_primos = 0;
int qtd_primos_verificacao = 0;

void *conta_primos(void *arg) {
    t_args *args = (t_args *) arg;
    long long int i;
    for (i = args->inicio; i <= args->fim; i++) {
        if (ehPrimo(i)) {
            
            pthread_mutex_lock(&mutex); // Bloqueia para atualizar a soma global
            qtd_primos++; //atualiza variável global
            pthread_mutex_unlock(&mutex); // Libera o bloqueio
        }
    }
    pthread_exit(NULL);
}

int verifica_primos(long long int N, int T) {
    pthread_t *tid_sistema;
    t_args *args;
    int t;
    //identificadores das threads no sistema
    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * T);
    if (tid_sistema == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
    //separa espaço vazio na memória para os argumentos
    args = (t_args *) malloc(sizeof(t_args) * T);
    if (args == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
    pthread_mutex_init(&mutex, NULL); //inicializa mute

    //cria as threads 
    for (t = 0; t < T; t++) {
        args[t].inicio = t * N / T + 1;
        args[t].fim = (t + 1) * N / T;
        if (pthread_create(&tid_sistema[t], NULL, conta_primos, (void *) &args[t])) {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }
    //--espera todas as threads terminarem
    for (t = 0; t < T; t++) {
        pthread_join(tid_sistema[t], NULL);
    }

    free(tid_sistema);
    free(args);

    return qtd_primos;
}

int main(int argc, char **argv) {
    //printa pra lembrar ao usuario que precisa dos inputs
    if (argc != 3) { 
        printf("Uso: %s <N> <T>\n", argv[0]);
        printf("N: número de elementos\n");
        printf("T: número de threads\n");
        exit(-1);
    }

    //salva os inputs do usuários sequência e threads
    long long int N = atoll(argv[1]);
    int T = atoi(argv[2]);

    double start_time, end_time; // variáveis double para controle do get_time

    GET_TIME(start_time); //pega time inicio

    int qtd_primos_encontrados = verifica_primos(N, T);
    
    GET_TIME(end_time); //pega time fim

    double tempo_decorrido = end_time - start_time;

    printf("Quantidade de números primos encontrados: %d\n", qtd_primos_encontrados);
    
	printf("Tempo decorrido: %lf segundos.\n", tempo_decorrido);

	printf("--Thread principal terminou\n");
    pthread_exit(NULL);
}

