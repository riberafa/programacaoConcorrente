import matplotlib
matplotlib.use('TkAgg')  # Ou 'Qt5Agg'

import subprocess
import numpy as np
import matplotlib.pyplot as plt
import re
import os

# Função para executar um comando no shell e extrair o tempo de execução
def executar_comando(comando, pattern):
    try:
        resultado = subprocess.run(comando, shell=True, check=True, capture_output=True, text=True)
        match = re.search(pattern, resultado.stdout)
        if match:
            tempo_execucao_str = match.group(1)
            tempo_execucao = float(tempo_execucao_str)
            return tempo_execucao
        else:
            print(f"Erro: Não foi possível encontrar o tempo de execução no output do comando: {comando}")
            return None
    except subprocess.CalledProcessError as e:
        print(f"Erro ao executar comando: {comando}")
        print(f"Código de retorno: {e.returncode}")
        print(f"Saída de erro: {e.stderr}")
        return None
    except Exception as e:
        print(f"Erro inesperado ao executar comando: {comando}")
        print(e)
        return None

# Função para coletar tempos de execução para um comando específico várias vezes
def coletar_tempos(comando_base, num_execucoes, pattern):
    tempos = []
    for i in range(num_execucoes):
        tempo = executar_comando(comando_base, pattern)
        if tempo is not None:
            tempos.append(tempo)
            if i == 0:
                print(f"Primeira execução - Tempo: {tempo:.6f} s")
            else:
                desvio_padrao = np.std(tempos)
                print(f"Amostra {i+1} - Tempo: {tempo:.6f} s, Desvio Padrão: {desvio_padrao:.6f}")
        else:
            print(f"Erro ao coletar tempo para a amostra {i+1}")
    return tempos

# Função para calcular a aceleração comparando o tempo sequencial com o tempo concorrente
def calcular_aceleracao(tempo_seq, tempos_conc):
    aceleracao = []
    for tempo_conc in tempos_conc:
        aceleracao.append(tempo_seq / tempo_conc)
    return aceleracao

# Função para calcular a eficiência comparando a aceleração com o número de threads
def calcular_eficiencia(aceleracao, num_threads):
    eficiencia = []
    for acc in aceleracao:
        eficiencia.append(acc / num_threads)
    return eficiencia

def plotar_grafico(x, y, xlabel, ylabel, title, filename):
    plt.figure(figsize=(8, 6))
    plt.plot(x, y, marker='o')
    plt.xlabel(xlabel)
    plt.ylabel(ylabel)
    plt.title(title)
    plt.grid(True)
    plt.savefig(filename)
    plt.show()

def plotar_tempo_medio_por_thread(tempos_conc, num_threads, filename):
    media_tempos = [np.mean(tc) for tc in tempos_conc]

    plt.figure(figsize=(10, 6))
    plt.plot(num_threads, media_tempos, marker='o', linestyle='-')
    plt.xlabel('Número de Threads')
    plt.ylabel('Tempo Médio de Execução (s)')
    plt.title('Tempo Médio de Execução por Número de Threads')
    plt.grid(True)
    plt.tight_layout()
    plt.savefig(filename)
    plt.show()

    # Output dos tempos médios de execução
    for i, n_threads in enumerate(num_threads):
        print(f"Tempo médio de execução com {n_threads} threads: {media_tempos[i]:.6f} s")

# Configurações de execução
num_execucoes = 5
comandos_conc = [ # Lista de comandos para execução concorrente com diferentes números de threads
    './new_integral_conc beta 0.001 0 1000 1',
    './new_integral_conc beta 0.001 0 1000 2',
    './new_integral_conc beta 0.001 0 1000 3',
    './new_integral_conc beta 0.001 0 1000 4',
    './new_integral_conc beta 0.001 0 1000 6',
    './new_integral_conc beta 0.001 0 1000 8'
]

# Padrão de expressão regular para captura dos tempos
pattern_conc = r'Tempo total de execução do cálculo da função [a-z]+ com [0-9]+ threads:? (\d+\.\d+)'

# Coleta de tempos de execução para o concorrente
tempos_conc = []
for comando in comandos_conc:
    print(f"\nColetando tempos de execução para o código concorrente com o comando: {comando}")
    tempos_conc.append(coletar_tempos(comando, num_execucoes, pattern_conc))

# Plotar gráfico de tempo médio de execução por número de threads
plotar_tempo_medio_por_thread(tempos_conc, [1, 2, 3, 4, 6, 8], 'tempo_medio_por_thread.png')

# Cálculo da média dos tempos para cada cenário
media_conc = [np.mean(tc) for tc in tempos_conc]

print(f"\nTempo Concorrente com 1 thread - Média: {media_conc[0]:.6f}")

for i, num_threads in enumerate([2, 3, 4, 6, 8]):
    print(f"Tempo Concorrente com {num_threads} threads - Média: {media_conc[i+1]:.6f}")
    print(f"Amostras: {tempos_conc[i+1]}")

# Cálculo da aceleração para cada cenário de concorrência
aceleracoes = [media_conc[0] / np.mean(tc) for tc in tempos_conc]

# Imprimir informações sobre a aceleração
print("\nInformações sobre a aceleração:")
for i, num_threads in enumerate([1, 2, 3, 4, 6, 8]):
    print(f"Aceleração com {num_threads} threads: {aceleracoes[i]:.6f}")

# Gráfico de aceleração x número de threads
x_threads = [1, 2, 3, 4, 6, 8]
plotar_grafico(x_threads, aceleracoes, 'Número de Threads', 'Aceleração', 'Aceleração x Número de Threads', 'aceleracao_threads.png')

# Cálculo da eficiência em relação aos núcleos (fixo em 4)
eficiencias_nucleos = [aceleracoes[i] / 4 for i in range(len(aceleracoes))]

# Imprimir informações sobre a eficiência em relação aos núcleos
print("\nInformações sobre a eficiência em relação aos núcleos (fixo em 4):")
for i, num_threads in enumerate([1, 2, 3, 4, 6, 8]):
    print(f"Eficiência com {num_threads} threads: {eficiencias_nucleos[i]:.6f}")

# Gráfico de eficiência x número de threads
plotar_grafico(x_threads, [np.mean(acc) for acc in aceleracoes], 'Número de Threads', 'Eficiência', 'Eficiência x Número de Threads', 'eficiencia_threads.png')

# Gráfico de eficiência x número de núcleos (fixo em 4)
plotar_grafico(x_threads, eficiencias_nucleos, 'Número de Threads', 'Eficiência', 'Eficiência x Núcleos (Fixo em 4)', 'eficiencia_nucleos.png')
