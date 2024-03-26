/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Objetivo: Implementar o seu primeiro programa concorrente! Escreva um programa
com M threads, para elevar ao quadrado cada elemento de um vetor de N elementos do
tipo inteiro. (Para cada elemento ai do vetor, calcular o novo valor e escrever o resultado
na mesma posição do elemento.) */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// cria a estrutura de dados para armazenar os argumentos da thread
typedef struct
{
    int inicio_array, fim_array; //inicio e fim que cada thread irá atuar
} t_Args;

// declarando os arrays que iremos usar
int *array_;
int *array_verificacao;

// funcao executada pelas threads
void *eleva_ao_quadrado_array(void *arg)
{
    t_Args *args = (t_Args *)arg;
    int inicio_array = args->inicio_array;
    int fim_array = args->fim_array;
    free(arg); // libera a memoria que foi alocada na main

    for (int i = inicio_array; i < fim_array; i += 1)
    {
        array_[i] *= array_[i]; // elevando ao quadrado
    }
    pthread_exit(NULL);
}

void funcao_de_verificacao(int array[], int tamanho)
{
    // Aloca memória para o array_verificacao
    array_verificacao = calloc(tamanho, sizeof(tamanho)); 


    // Verifica se a alocação de memória do array_verificacao deu certo.
    if (array_verificacao == NULL)
    {
        printf("Erro ao alocar memória para array_verificacao\n");
        return;
    }

    // Preenche o array de verificacao 
    for (int i = 0; i < tamanho; i++) {
        array_verificacao[i] = i + 1; // Preenche o array_verificacao
    }

    // Função de verificação
    for (int i = 0; i < tamanho; i++)
    {
        array_verificacao[i] *= array_verificacao[i]; // Elevando o array de comparação, também ao quadrado
        if (array_verificacao[i] != array[i])
        { // compara os vetores em cada posição
            printf("O array na posição %d possui valor diferente.\n", i);
        }
    }
    printf("--O quadrado de cada elemento de um vetor de %d posições foi criado e verificado com sucesso! :)\n", tamanho);
    // Libera a memória alocada para array_verificacao
    free(array_verificacao);
}

// funcao principal do programa
int main(int argc, char **argv)
{
    // printa pra lembrar ao usuario que precisa dos inputs
    if (argc != 3)
    {
        printf("Uso: %s <TAM_ARRAY> <NTHREADS>\n", argv[0]);
        printf("TAM_ARRAY: tamanho do array \n");
        printf("NTHREADS: número de threads a serem criadas\n");
        exit(-1);
    }

    // salva os inputs do usuário: tamanho do array e quantidade de threads
    long long int TAM_ARRAY = atoll(argv[1]); // convertendo string grande para inteiro
    int NTHREADS = atoi(argv[2]);             // convertendo string para inteiro
    pthread_t tid_sistema[NTHREADS];               // identificadores das threads no sistema
    t_Args *args;                                  // receberá os argumentos para a thread
    array_ = calloc(TAM_ARRAY, sizeof(TAM_ARRAY)); // separa espaço vazio na memória para o array

    if (array_ == NULL) // Verifica se a alocação de memória do array_ deu certo.
    {
        printf("Erro ao alocar memória para array_\n");
        exit(-1);

    }

    // Preenche o array com valores específicos
    for (int i = 0; i < TAM_ARRAY; i++) {
        array_[i] = i + 1; // Preenche o array_
    }

    for (int i = 0; i < NTHREADS; i++)
    {
        printf("--Aloca e preenche argumentos para thread %d\n", i);
        args = malloc(sizeof(t_Args)); // separa espaço vazio na memória para os argumentos
        if (args == NULL)
        {
            printf("--ERRO: malloc()\n");
            exit(-1);
        }
        args->inicio_array = TAM_ARRAY / NTHREADS * i;    // pega o início de atuação no array de cada thread
        args->fim_array = TAM_ARRAY / NTHREADS * (i + 1); // pega o fim de atuação no array de cada thread

        printf("--Cria a thread %d\n", i);
        if (pthread_create(&tid_sistema[i], NULL, eleva_ao_quadrado_array, (void *)args))
        {
            printf("--ERRO: pthread_create()\n");
            exit(-1);
        }
    }

    //--espera todas as threads terminarem
    for (int i = 0; i < NTHREADS; i++)
    {
        if (pthread_join(tid_sistema[i], NULL))
        {
            printf("--ERRO: pthread_join() \n");
            exit(-1);
        }
    }

    // funcao de verificacao do vetor de inteiros gerado
    funcao_de_verificacao(array_, TAM_ARRAY);

    printf("--Thread principal terminou\n");
    pthread_exit(NULL);
}
