#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <math.h>
#include "timer.h"

// Variáveis globais 
int limite_inferior, limite_superior, numero_intervalos = 2;
double erro, resp_seq, trapézio_maior;

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

// Calcula a integral de forma sequencial usando a resolução por método dos trapezios.
double calculaIntegralSequencial(double (*funcao_escolhida)(double))
{
    double largura_intervalo, resp = 0; // largura do intervalo representa a altura do trapezio
    int i;

    // Calcula a largura de cada intervalo.
    largura_intervalo = (double)(limite_superior - limite_inferior) / numero_intervalos;

    // Soma a área de cada trapézio formado pela função nos intervalos.
    for (i = 0; i < numero_intervalos; i++)
    {
        double x_esq = limite_inferior + i * largura_intervalo;
        double x_dir = x_esq + largura_intervalo;
        resp += (funcao_escolhida(x_esq) + funcao_escolhida(x_dir)) * largura_intervalo / 2.0;
    }
    
    // Verifica se a diferença entre o trapézio maior e a resposta é menor que o erro desejado.
    // Se sim, retorna o valor absoluto da resposta.
    if (fabs(trapézio_maior - fabs(resp)) < erro)
    {
        return fabs(resp);
    }
    // Se não, aumenta o número de intervalos, atualiza o trapézio maior e recursivamente chama a função.
    else
    {
        numero_intervalos++;
        trapézio_maior = fabs(resp);
        return calculaIntegralSequencial(funcao_escolhida);
    }
}

// Função principal do programa.
int main(int argc, char *argv[])
{
    double inicio, fim, tempo_total;
    double (*funcao_escolhida)(double);

    // Verifica a quantidade de argumentos da linha de comando.
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

    // Inicializa o trapézio maior usando a média da função nos extremos do intervalo.
    trapézio_maior = (double)(limite_superior - limite_inferior) * (funcao_escolhida(limite_inferior) + funcao_escolhida(limite_superior)) / 2.0;
    trapézio_maior = fabs(trapézio_maior);

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
