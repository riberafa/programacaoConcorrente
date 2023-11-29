/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Implementação Trabalho Final de Disciplina - Código 1 */
/* Objetivo: implementação sequencial para a resolução de integrais por meio do método de aproximação retangular com ponto médio */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include "timer.h"

// Variáveis globais 
int limite_inferior, limite_superior, numero_intervalos = 2;
double erro, resp_seq, retangulo_maior;

// Declarações das funções possíveis para o cálculo da integral.
double funcaoAlpha(double x);
double funcaoBeta(double x);

// Mapeia uma string para a função desejada para o cálculo da integral (informada pelo usuário) e retorna o ponteiro para essa função.
double (*mapeiaFuncao(char *nome_funcao))(double) {
    if (strcmp(nome_funcao, "alpha") == 0) return funcaoAlpha;
    else if (strcmp(nome_funcao, "beta") == 0) return funcaoBeta;
    else {
        fprintf(stderr, "Função desconhecida: %s\n", nome_funcao);
        exit(-1);
    }
}

// Calcula a integral de forma sequencial usando a resolução por método de aproximação retangular com ponto médio.
double calculaIntegralSequencial(double (*funcao_escolhida)(double))
{
    double largura_intervalo, resp = 0;
    int i;

    // Calcula a largura de cada intervalo.
    largura_intervalo = (double)(limite_superior - limite_inferior) / numero_intervalos;

    // Soma a área de cada retângulo formado pela função nos intervalos, usando o ponto médio.
    for (i = 1; i <= numero_intervalos; i++)
    {
        resp += largura_intervalo * funcao_escolhida((limite_inferior + (largura_intervalo * i)) - (largura_intervalo / 2));
    }
    // Verifica se a diferença entre o retângulo maior e a resposta é menor que o erro desejado.
    // Se sim, retorna o valor absoluto da resposta.
    if (fabs(retangulo_maior - fabs(resp)) < erro)
    {
        return fabs(resp);
    }
    // Se não, aumenta o número de intervalos, atualiza o retângulo maior e recursivamente chama a função.
    else
    {
        numero_intervalos++;
        retangulo_maior = fabs(resp);
        return calculaIntegralSequencial(funcao_escolhida);
    }
}

// Função principal do programa.
int main(int argc, char *argv[])
{
    double inicio, fim, tempo_total;
    double (*funcao_escolhida)(double);

    // Verifica a quantgidade de argumentos da linha de comando.
    if (argc < 5)
    {
        fprintf(stderr, "Digite: %s <funcao> <erro> <limite_inferior> <limite_superior>\n", argv[0]);
        exit(-1);
    }
    
    // Mapeia parâmetros recebidos da linha de comando.
    funcao_escolhida = mapeiaFuncao(argv[1]);
    erro = atof(argv[2]);
    limite_inferior = atoi(argv[3]);
    limite_superior = atoi(argv[4]);

    GET_TIME(inicio);

    // Inicializa o retângulo maior usando a média da função no ponto médio do intervalo.
    retangulo_maior = (double)(limite_superior - limite_inferior) * funcao_escolhida((limite_inferior + limite_superior) / 2);
    retangulo_maior = fabs(retangulo_maior);

    // Calcula a integral sequencialmente.
    resp_seq = calculaIntegralSequencial(funcao_escolhida);

    GET_TIME(fim);

    tempo_total = fim - inicio;
    
    //output
    printf("Resposta da Integral da função %s: %lf\n", argv[1], resp_seq);
    printf("Tempo total de execução do cálculo da função %s: %.8lf\n", argv[1], tempo_total);

    return 0;
}

// Conjunto de funções escolhidas para o cálculo da integral.
double funcaoAlpha(double x) { return sin(pow(x, 2)); }
double funcaoBeta(double x) { return cos(pow(M_E, -x)) * (0.005 * pow(x, 3) + 1); }
