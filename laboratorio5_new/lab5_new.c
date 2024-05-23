#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <stdbool.h>
#include <semaphore.h>

// Estrutura do buffer circular
typedef struct {
    int *buffer; // Array para colocar os itens
    int tamanho; // Tamanho máximo do buffer
    int in;// Índice de inserção
    int out;// Índice de retirada
    int contagem; // Contagem atual de itens dentro d buffer
    sem_t mutexGeral;// Semáforo para exclusão mútua
    sem_t slotCheio;// Semáforo para slots cheios
    sem_t slotVazio;// Semáforo para slots vazios
} BufferCircular;

// Estrutura para os argumentos do consumidor
typedef struct {
    int id;// identificador da thread consumidora
    int contagem_primos;// Contagem de números primos encontrados
    BufferCircular *buffer;// Referência ao buffer
    bool *finalizado;// Marcador para indicar finalização
} ArgumentosConsumidor;

// Estrutura para os argumentos do produtor
typedef struct {
    FILE *arquivo;// Arquivo de entrada para consumir pela produtora
    BufferCircular *buffer;// Referência ao buffer
    int num_consumidores; // Número de consumidores
} ArgumentosProdutor;


bool eh_primo(long long int n) {
    if (n <= 1) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    for (long long int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

// Função de inserir item no buffer
void buffer_coloca(BufferCircular *cb, int item) {
    sem_wait(&cb->slotCheio); 
    sem_wait(&cb->mutexGeral);
    cb->buffer[cb->in] = item;// Insere o item no buffer
    cb->in = (cb->in + 1) % cb->tamanho;// Atualiza o índice de inserção
    cb->contagem++;// Incremento da contagem de items
    sem_post(&cb->mutexGeral);
    sem_post(&cb->slotVazio); 
}

// Função de retirar item do buffer
int buffer_retira(BufferCircular *cb) {
    sem_wait(&cb->slotVazio); 
    sem_wait(&cb->mutexGeral);
    int item = cb->buffer[cb->out]; // Retira o item do buffer
    cb->out = (cb->out + 1) % cb->tamanho; // Atualiza o índice de retirada
    cb->contagem--; // Decrementa a contagem de itens
    sem_post(&cb->mutexGeral); 
    sem_post(&cb->slotCheio);     
    return item;
}

// Função da thread produtora
void *produtor(void *arg) {
    ArgumentosProdutor *args = (ArgumentosProdutor *)arg;
    FILE *arquivo = args->arquivo;
    BufferCircular *buffer = args->buffer;
    int numero;

    // Lê os números do arquivo e os insere no buffer
    while (fread(&numero, sizeof(int), 1, arquivo) == 1) {
        buffer_coloca(buffer, numero); //os semáforos estão dentro dela
    }

    fclose(arquivo);

    // Sinaliza os consumidores para parar
    for (int i = 0; i < args->num_consumidores; ++i) {
        buffer_coloca(buffer, -1); 
    }

    return NULL;
}

// Função das threads consumidoras
void *consumidor(void *arg) {
    ArgumentosConsumidor *args = (ArgumentosConsumidor *)arg;
    int item;

    while (1) {
        item = buffer_retira(args->buffer);
        if (item == -1) break; // Se encontrar o marcador de fim, sai do loop
        if (eh_primo(item)) { // Verificação do numero se é priomo 
            args->contagem_primos++; // incrementa a contagem do primos
        }
    }

    args->finalizado[args->id] = true; // Marca o consumidor como finalizado
    return NULL;
}

int main(int argc, char *argv[]) {
    // Pra verificar os argumentos passados 
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <arquivo de entrada> <tamanho do buffer> <número de consumidores>\n", argv[0]);
        return EXIT_FAILURE;
    }

    //coleta dos valores passados
    const char *nome_arquivo = argv[1];
    int tamanho_buffer = atoi(argv[2]);
    int num_consumidores = atoi(argv[3]);

    // Inicialização do buffer circular
    BufferCircular buffer;
    buffer.buffer = malloc(tamanho_buffer * sizeof(int));
    buffer.tamanho = tamanho_buffer;
    buffer.in = 0;
    buffer.out = 0;
    buffer.contagem = 0;
    sem_init(&buffer.mutexGeral, 0, 1);
    sem_init(&buffer.slotCheio, 0, tamanho_buffer);
    sem_init(&buffer.slotVazio, 0, 0);

    FILE *arquivo = fopen(nome_arquivo, "rb");
    if (!arquivo) {
        perror("Não foi possível abrir o arquivo");
        return EXIT_FAILURE;
    }

    pthread_t thread_produtor;
    pthread_t threads_consumidoras[num_consumidores];
    ArgumentosConsumidor args_consumidor[num_consumidores];
    bool finalizado[num_consumidores];

    // Inicializa argumentos dos consumidores
    for (int i = 0; i < num_consumidores; ++i) {
        args_consumidor[i].id = i;
        args_consumidor[i].contagem_primos = 0;
        args_consumidor[i].buffer = &buffer;
        args_consumidor[i].finalizado = finalizado;
        finalizado[i] = false;
    }

    // Inicializa argumentos do produtor e cria a thread produtora
    ArgumentosProdutor args_produtor = {arquivo, &buffer, num_consumidores};
    pthread_create(&thread_produtor, NULL, produtor, &args_produtor);

    // Cria as threads consumidoras
    for (int i = 0; i < num_consumidores; ++i) {
        pthread_create(&threads_consumidoras[i], NULL, consumidor, &args_consumidor[i]);
    }

    // Aguarda produtora terminar
    pthread_join(thread_produtor, NULL);

    // Aguarda todas consumidoras terminarem
    for (int i = 0; i < num_consumidores; ++i) {
        pthread_join(threads_consumidoras[i], NULL);
    }

    int total_primos = 0;
    int id_vencedor = -1;
    int max_primos = 0;

    // Calcula o total de números primos encontrados e a thread consumidora que pegou mais primos
    for (int i = 0; i < num_consumidores; ++i) {
        total_primos += args_consumidor[i].contagem_primos; // pega a contagem de cada consumidora e soma
        if (args_consumidor[i].contagem_primos > max_primos) { 
            max_primos = args_consumidor[i].contagem_primos; //max_primos representa sempre a quantidade da maior  consumidora 
            id_vencedor = args_consumidor[i].id; // coleta o id da consumidora que mais encontrou primos
        }
    }

    printf("Total de primos encontrados: %d\n", total_primos);
    printf("Thread consumidora %d encontrou a maior quantidade de primos (%d)\n", id_vencedor, max_primos);

    // Liberação buffer
    free(buffer.buffer);
    sem_destroy(&buffer.mutexGeral);
    sem_destroy(&buffer.slotCheio);
    sem_destroy(&buffer.slotVazio);

    return EXIT_SUCCESS;
}
