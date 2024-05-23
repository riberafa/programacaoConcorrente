#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

int ehPrimo(long long int n) {
    int i;
    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2)
        if (n % i == 0) return 0;
    return 1;
}

void gerar_arquivo_binario(const char *nome_arquivo, int N) {
    FILE *arquivo = fopen(nome_arquivo, "wb");
    if (!arquivo) {
        perror("Não foi possível abrir o arquivo");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    int primos_encontrados = 0;
    for (int i = 0; i < N; ++i) {
        int number = rand();
        if (ehPrimo(number)) {
            primos_encontrados++;
        }
        fwrite(&number, sizeof(int), 1, arquivo);
    }

    fclose(arquivo);

    printf("Quantidade de números primos encontrados: %d\n", primos_encontrados);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo de saída> <número de inteiros>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *nome_arquivo = argv[1];
    int N = atoi(argv[2]);

    gerar_arquivo_binario(nome_arquivo, N);

    return EXIT_SUCCESS;
}
