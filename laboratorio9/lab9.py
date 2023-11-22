# Disciplina: Programacao Concorrente 
# Prof.: Silvana Rossetto 
# aluno: Rafael Ribeiro 
# Laboratório: 9
# Objetivo: Usar os recursos de pool de processos ou de futuros em Python para resolver o mesmo problema do Lab8, de encontrar numeros primos em uma lista de entrada.. 


import concurrent.futures
import math
import time

def ehPrimo(n):
    if n <= 1:
        return False
    if n == 2:
        return True
    if n % 2 == 0:
        return False
    for i in range(3, int(math.sqrt(n)) + 1, 2):
        if n % i == 0:
            return False
    return True

def VerificaPrimo(n):
    return ehPrimo(n)


if __name__ == "__main__":
    start = time.time()
    N = 1000
    NTHREADS = 6
    #cria um pool de threads OU de processos com no maximo 5 intancias 
    #with concurrent.futures.ThreadPoolExecutor(max_workers=NTHREADS) as executor:
    with concurrent.futures.ProcessPoolExecutor(max_workers=NTHREADS) as executor:
        futures = []

        #submete a tarefa para execucao assincrona
        for i in range(1, N + 1):
            futures.append(executor.submit(VerificaPrimo, i))

        #recebe os resultados
        count = 0
        for future in futures:
            result = future.result()
            if result:
                count += 1

    end = time.time()
    print("intervalo analisado para saber a quantidade de primos: 1 até", N)
    print("Total de números primos encontrados:", count)
    print('work took {} seconds'.format(end - start))

