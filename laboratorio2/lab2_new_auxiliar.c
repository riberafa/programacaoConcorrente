/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 2 */
/* Objetivo: PProjetar, implementar e avaliar uma versao concorrente para o problema de
somar todos os elementos de um vetor de numeros reais.  ́*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX 1000 // Valor máximo dos elementos do vetor

int main(int argc, char *argv[]) {
    long int n; // Quantidade de elementos do vetor que serão gerados
    float elem; // Valor gerado para incluir no vetor
    float soma = 0.0; // Soma dos valores gerados
    int fator = 1; // Fator multiplicador para gerar números negativos

    // Valida e recebe os valores de entrada
    if (argc < 2) {
       printf("Use: %s <numero de elementos (maior que 0)> \n", argv[0]);
       return 1;
    }
    n = atol(argv[1]);
    if (n < 1) {
       printf("ERRO: o numero de elementos deve ser maior que 0 \n");
       return 2;
    }	    

    // Semente para gerar números randomicos
    srand((unsigned)time(NULL));

    // Imprime a quantidade de elementos do vetor (inteiro)
    printf("%ld\n", n);
    
    // Gera e imprime os elementos do vetor
    for (int i = 0; i < n; i++) {
        elem = (rand() % MAX) / 3.0 * fator;
        printf("%f ", elem);
        soma += elem; // Adiciona o elemento atual à soma total
        fator *= -1;
    }
    // Imprime a soma total dos valores do vetor
        printf("\n%f\n", soma);

    return 0;
}
