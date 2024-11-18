import matplotlib.pyplot as plt
import numpy as np

# Função para gerar o gráfico de comparação de tempos
def gerar_grafico_tempos(entradas, tempos_exaustivo, tempos_poda, tempos_openmp, tempos_mpi):
    largura_barra = 0.2  # Largura das barras
    posicoes = np.arange(len(entradas))

    plt.figure(figsize=(10, 6))

    # Adiciona as barras para cada tipo de execução
    plt.bar(posicoes - 1.5 * largura_barra, tempos_exaustivo, width=largura_barra, label='Exaustivo')
    plt.bar(posicoes - 0.5 * largura_barra, tempos_poda, width=largura_barra, label='Poda')
    plt.bar(posicoes + 0.5 * largura_barra, tempos_openmp, width=largura_barra, label='OpenMP')
    plt.bar(posicoes + 1.5 * largura_barra, tempos_mpi, width=largura_barra, label='MPI')

    # Configuração dos eixos e legendas
    plt.xticks(posicoes, entradas)
    plt.xlabel('Valores de Entrada (número de vértices)')
    plt.ylabel('Tempo de Execução (s)')
    plt.title('Comparação de Tempo de Execução por Tipo de Execução')
    plt.legend()

    # Exibir o gráfico
    plt.show()

# Dados de entrada
entradas = [100, 150, 160, 170, 180]

# Tempos fictícios para cada tipo de execução (substituir pelos valores reais)
tempos_exaustivo = [9.9, 404.3, 526.8, 0, 0]  # Exaustivo
tempos_poda = [0.6, 11, 16.3, 38.2, 48.6]        # Poda
tempos_openmp = [0.9, 16.5, 23.7, 55.4, 66]      # OpenMP
tempos_mpi = [1, 17.4, 24.8, 58, 69.2]         # MPI

# Geração do gráfico
gerar_grafico_tempos(entradas, tempos_exaustivo, tempos_poda, tempos_openmp, tempos_mpi)

