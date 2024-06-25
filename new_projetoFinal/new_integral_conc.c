#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include "timer.h"

// Variáveis globais 
int limite_inferior, limite_superior, numero_intervalos = 2, nthreads, n_threads_executadas = 0, threads_bloqueadas = 0, resp_encontrada = 0;
double erro, resp_conc, trapezio_maior;
pthread_mutex_t mutex;
pthread_cond_t cond;

// Declarações das funções possíveis para o cálculo da integral.
double funcaoAlpha(double x);
double funcaoBeta(double x);

typedef struct {
    long long int id;
    double (*funcao_escolhida)(double);
} t_args;

// Mapeia uma string para a função desejada para o cálculo da integral (informada pelo usuário) e retorna o ponteiro para essa função.
double (*mapeiaFuncao(char *nome_funcao))(double) {
    if (strcmp(nome_funcao, "alpha") == 0) return funcaoAlpha;
    else if (strcmp(nome_funcao, "beta") == 0) return funcaoBeta;
    else {
        fprintf(stderr, "Função desconhecida: %s\n", nome_funcao);
        exit(-1);
    }
}

// Calcula a integral de forma concorrente usando a resolução por método dos trapezios.
void *calculaIntegralConcorrente(void *arg) {
    t_args *args = (t_args *) arg;

    int i, thread_id = args->id;
    double largura_intervalo, resp_thread; // largura do intervalo representa a altura do trapezio

    while (!resp_encontrada) {
        // Calcula a largura de cada intervalo.
        largura_intervalo = (double) (limite_superior - limite_inferior) / numero_intervalos;
        resp_thread = 0;

        // Soma a área de cada trapézio formado pela função nos intervalos.
        for (i = thread_id; i <= numero_intervalos; i += nthreads) {
            double x_esq = limite_inferior + largura_intervalo * (i - 1);
            double x_dir = limite_inferior + largura_intervalo * i;
            resp_thread += largura_intervalo * (args->funcao_escolhida(x_esq) + args->funcao_escolhida(x_dir)) / 2.0;
        }

        pthread_mutex_lock(&mutex);
        resp_conc += resp_thread;
        n_threads_executadas++;

        if (n_threads_executadas < nthreads) {
            threads_bloqueadas++;
            pthread_cond_wait(&cond, &mutex);
            threads_bloqueadas--;
        } else {
            // Verifica se a diferença entre o trapézio maior e a resposta é menor que o erro desejado.
            // Se sim, retorna o valor absoluto da resposta.
            if (fabs(trapezio_maior - fabs(resp_conc)) < erro) {
                resp_encontrada = 1;
                pthread_cond_broadcast(&cond);
            } else { // Se não, aumenta o número de intervalos, atualiza o trapézio maior e itera novamente  com a nova quantidade de intervalos
                numero_intervalos++;
                trapezio_maior = fabs(resp_conc);
                resp_conc = 0;
                n_threads_executadas = 0;

                pthread_cond_broadcast(&cond);
            }
        }
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    pthread_t *tid_sistema;
    t_args *args;
    int i, *id;
    double inicio, fim, tempo_total;
    double (*funcao_escolhida)(double);
    pthread_mutex_init(&mutex, NULL);

    // Verifica a quantidade de argumentos da linha de comando.
    if (argc < 6) {
        fprintf(stderr, "Digite: %s <funcao> <erro> <limite_inferior> <limite_superior> <numero_threads>\n", argv[0]);
        exit(-1);
    }

    // Mapeia parâmetros recebidos da linha de comando.
    funcao_escolhida = mapeiaFuncao(argv[1]);
    erro = atof(argv[2]);
    limite_inferior = atoi(argv[3]);
    limite_superior = atoi(argv[4]);
    nthreads = atoi(argv[5]);

    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if (tid_sistema == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }

    args = (t_args *) malloc(sizeof(t_args) * nthreads);
    if (args == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }

    // Inicializa o trapézio maior usando a média da função nos extremos do intervalo.
    trapezio_maior = (double) (limite_superior - limite_inferior) * funcao_escolhida((limite_inferior + limite_superior) / 2);
    trapezio_maior = fabs(trapezio_maior);

    GET_TIME(inicio);

    for (i = 0; i < nthreads; i++) {
        id = malloc(sizeof(int));
        if (id == NULL) {
            printf("--ERRO: malloc do id\n");
            exit(-1);
        }
        args[i].id = i + 1;
        args[i].funcao_escolhida = funcao_escolhida;
        if (pthread_create(&tid_sistema[i], NULL, calculaIntegralConcorrente, (void *) &args[i])) {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    for (i = 0; i < nthreads; i++) {
        pthread_join(tid_sistema[i], NULL);
    }

    GET_TIME(fim);

    tempo_total = fim - inicio;

    printf("Resposta da Integral da função %s: %lf\n", argv[1], fabs(resp_conc));
    printf("Tempo total de execução do cálculo da função %s com %d threads: %.8lf\n", argv[1], nthreads, tempo_total);

    free(tid_sistema);
    free(args);
    pthread_mutex_destroy(&mutex);

    return 0;
}
// Conjunto de funções escolhidas para o cálculo da integral.
double funcaoAlpha(double x) { return sin(pow(x, 2)); }
double funcaoBeta(double x) { return cos(pow(M_E, -x)) * (0.005 * pow(x, 3) + 1); }
