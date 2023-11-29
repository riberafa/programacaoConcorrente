/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Implementação Trabalho Final de Disciplina - Código 2 */
/* Objetivo: implementação concorrente para a resolução de integrais por meio do método de aproximação retangular com ponto médio */


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <string.h>
#include "timer.h"

int limite_inferior, limite_superior, numero_intervalos = 2, nthreads, n_threads_executadas = 0, threads_bloqueadas = 0, resp_encontrada = 0;
double erro, resp_conc, retangulo_maior;
pthread_mutex_t mutex;
pthread_cond_t cond;

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

// Calcula a integral de forma concorrente usando a resolução por método de aproximação retangular com ponto médio.
void *calculaIntegralConcorrente(void *arg)
{
    t_args *args = (t_args *) arg;

    int i, thread_id = args->id;
    double largura_intervalo, resp_thread;

    while (!resp_encontrada)
    {
        // Calcula a largura do intervalo que cada thread irá processar
        largura_intervalo  = (double)(limite_superior - limite_inferior) / numero_intervalos;
        // Variável para armazenar a contribuição da thread para a integral
        resp_thread = 0;

        // Loop para calcular a área dos intervalos atribuídos a cada thread
        for (i = thread_id; i <= numero_intervalos; i += nthreads)
        {   
            // Calcula a área do retângulo para o ponto médio do intervalo atribuído à thread
            resp_thread += largura_intervalo  * args->funcao_escolhida((limite_inferior + (largura_intervalo  * i)) - (largura_intervalo  / 2));
        }

        // Bloqueia a região crítica para atualizar as variáveis compartilhadas
        pthread_mutex_lock(&mutex);
        resp_conc += resp_thread;
        n_threads_executadas++;

        // Se ainda nem todas as threads terminaram, espera
        if (n_threads_executadas < nthreads)
        {
            threads_bloqueadas++;
            pthread_cond_wait(&cond, &mutex);
            threads_bloqueadas--;
        }
        // Se todas as threads terminaram, verifica se o erro é atendido para saber se encontrou a resposta
        else
        {
            if (fabs(retangulo_maior - fabs(resp_conc)) < erro)
            {   
                // Sinaliza que o cálculo foi concluído
                resp_encontrada = 1;
                pthread_cond_broadcast(&cond);
            }
            else
            {                
                // Espera que todas as threads estejam bloqueadas antes de atualizar as variáveis e continuar o cálculo
                while (threads_bloqueadas != nthreads - 1)
                {
                    // Aguarda as outras threads
                }                
                // Atualiza as variáveis para a próxima iteração
                numero_intervalos++;
                retangulo_maior = fabs(resp_conc);
                resp_conc = 0;
                n_threads_executadas = 0;
                // Libera as threads bloqueadas para continuar a próxima iteração
                pthread_cond_broadcast(&cond);
            }
        }
        // Libera a região crítica
        pthread_mutex_unlock(&mutex);
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    pthread_t *tid_sistema;
    t_args *args;
    int i, *id;
    double inicio, fim, tempo_total;
    double (*funcao_escolhida)(double);
    pthread_mutex_init(&mutex, NULL);

    // Verifica a quantgidade de argumentos da linha de comando.
    if (argc < 6){
        fprintf(stderr, "Digite: %s <erro> <limite_inferior> <limite_superior> <numero_threads>\n", argv[0]);
        exit(-1);
    }
    
    // Mapeia parâmetros recebidos da linha de comando.
    funcao_escolhida = mapeiaFuncao(argv[1]);
    erro = atof(argv[2]);
    limite_inferior = atoi(argv[3]);
    limite_superior = atoi(argv[4]);
    nthreads = atoi(argv[5]);


    //identificadores das threads no sistema
    tid_sistema = (pthread_t *) malloc(sizeof(pthread_t) * nthreads);
    if (tid_sistema == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }
     //separa espaço vazio na memória para os argumentos
    args = (t_args *) malloc(sizeof(t_args) * nthreads);
    if (args == NULL) {
        printf("--ERRO: malloc()\n");
        exit(-1);
    }

    // Inicializa o retângulo maior usando a média da função no ponto médio do intervalo.
    retangulo_maior = (double)(limite_superior - limite_inferior) * funcao_escolhida((limite_inferior + limite_superior) / 2);
    retangulo_maior = fabs(retangulo_maior);

    GET_TIME(inicio);

    //cria as threads 
    for (i = 0; i < nthreads; i++) {
        id = malloc(sizeof(int));
        if (id == NULL){
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
    //--espera todas as threads terminarem
    for (i = 0; i < nthreads; i++) {
        pthread_join(tid_sistema[i], NULL);
    }

    GET_TIME(fim);

    tempo_total = fim - inicio;

    //output
    printf("Resposta da Integral da função %s: %lf\n", argv[1],  fabs(resp_conc));
    printf("Tempo total de execução do cálculo da função %s com %d threads: %.8lf\n", argv[1], nthreads, tempo_total);

    free(tid_sistema);
    free(args);
    pthread_mutex_destroy(&mutex);

    return 0;
}

// Conjunto de funções escolhidas para o cálculo da integral.
double funcaoAlpha(double x) { return sin(pow(x, 2)); }
double funcaoBeta(double x) { return cos(pow(M_E, -x)) * (0.005 * pow(x, 3) + 1); }

