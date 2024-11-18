#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <string>
#include <fstream>
#include <iomanip>

// Função para carregar o grafo a partir de um arquivo
std::vector<std::vector<int>> CarregarGrafo(const std::string& caminhoArquivo, int& totalVertices) {
    std::ifstream entrada(caminhoArquivo);
    if (!entrada.is_open()) {
        std::cerr << "Falha ao abrir o arquivo " << caminhoArquivo << std::endl;
        return {};
    }

    int totalArestas;
    entrada >> totalVertices >> totalArestas;

    // Cria matriz de adjacência preenchida com zeros
    std::vector<std::vector<int>> matrizAdjacencia(totalVertices, std::vector<int>(totalVertices, 0));

    for (int i = 0; i < totalArestas; ++i) {
        int origem, destino;
        entrada >> origem >> destino;
        matrizAdjacencia[origem - 1][destino - 1] = 1;
        matrizAdjacencia[destino - 1][origem - 1] = 1;
    }

    entrada.close();
    return matrizAdjacencia;
}

// Função recursiva para explorar todas as possibilidades e identificar a maior clique
void ExplorarClique(const std::vector<std::vector<int>>& matrizAdjacencia, std::vector<int>& cliqueAtual, 
                    std::vector<int>& melhorClique, int totalVertices, int verticeInicial) {
    if (cliqueAtual.size() > melhorClique.size()) {
        melhorClique = cliqueAtual;
    }

    for (int vertice = verticeInicial; vertice < totalVertices; ++vertice) {
        bool todosConectados = true;

        for (int vizinho : cliqueAtual) {
            if (matrizAdjacencia[vizinho][vertice] == 0) {
                todosConectados = false;
                break;
            }
        }

        if (todosConectados) {
            cliqueAtual.push_back(vertice);
            ExplorarClique(matrizAdjacencia, cliqueAtual, melhorClique, totalVertices, vertice + 1);
            cliqueAtual.pop_back();
        }
    }
}

// Função principal para encontrar a maior clique no grafo
std::vector<int> ObterMaiorClique(const std::vector<std::vector<int>>& matrizAdjacencia, int totalVertices) {
    std::vector<int> melhorClique;
    std::vector<int> cliqueAtual;
    ExplorarClique(matrizAdjacencia, cliqueAtual, melhorClique, totalVertices, 0);
    return melhorClique;
}

int main() {
    int totalVertices;
    std::string caminhoArquivo = "grafo170.txt";

    // Carrega o grafo do arquivo especificado
    std::vector<std::vector<int>> matrizAdjacencia = CarregarGrafo(caminhoArquivo, totalVertices);

    auto inicioExecucao = std::chrono::high_resolution_clock::now();

    // Obtém a maior clique utilizando busca exaustiva
    std::vector<int> melhorClique = ObterMaiorClique(matrizAdjacencia, totalVertices);

    auto tempoExecucao = std::chrono::duration_cast<std::chrono::duration<double>>(
                            std::chrono::high_resolution_clock::now() - inicioExecucao).count();

    // Exibe os resultados
    std::cout << "Maior clique -> ";
    for (int vertice : melhorClique) {
        std::cout << vertice + 1 << " ";
    }
    std::cout << "\n";

    std::cout << "Tamanho da maior clique -> " << melhorClique.size() << "\n";

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "Tempo de execução -> " << tempoExecucao << " segundos\n";

    return 0;
}






