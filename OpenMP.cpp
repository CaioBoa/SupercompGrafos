#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <string>
#include <fstream>
#include <iomanip>
#include <omp.h>
#include <numeric> // Para std::iota e std::accumulate

// Restante do código permanece o mesmo...


// Função para carregar o grafo a partir de um arquivo
std::vector<std::vector<int>> CarregarGrafo(const std::string& caminhoArquivo, int& totalVertices) {
    std::ifstream entrada(caminhoArquivo);
    if (!entrada.is_open()) {
        std::cerr << "Falha ao abrir o arquivo " << caminhoArquivo << std::endl;
        return {};
    }

    int totalArestas;
    entrada >> totalVertices >> totalArestas;

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

// Função recursiva com melhorias e poda
void ExplorarCliqueComPoda(const std::vector<std::vector<int>>& matrizAdjacencia, 
                           std::vector<int>& cliqueAtual, std::vector<int>& melhorCliqueLocal, 
                           std::vector<int>& candidatos, int& maxCliqueSize) {
    if (cliqueAtual.size() + candidatos.size() <= maxCliqueSize) return;

    for (int i = 0; i < candidatos.size(); ++i) {
        int vertice = candidatos[i];

        std::vector<int> novosCandidatos;
        for (int j = i + 1; j < candidatos.size(); ++j) {
            if (matrizAdjacencia[vertice][candidatos[j]] == 1) {
                novosCandidatos.push_back(candidatos[j]);
            }
        }

        cliqueAtual.push_back(vertice);
        if (cliqueAtual.size() > melhorCliqueLocal.size()) {
            melhorCliqueLocal = cliqueAtual;
            #pragma omp critical
            maxCliqueSize = std::max(maxCliqueSize, (int)melhorCliqueLocal.size());
        }

        ExplorarCliqueComPoda(matrizAdjacencia, cliqueAtual, melhorCliqueLocal, novosCandidatos, maxCliqueSize);
        cliqueAtual.pop_back();
    }
}

// Função principal para encontrar a maior clique com heurísticas e OpenMP
std::vector<int> ObterMaiorClique(const std::vector<std::vector<int>>& matrizAdjacencia, int totalVertices) {
    std::vector<int> melhorClique;
    int maxCliqueSize = 0;

    // Pré-processamento: Ordenar vértices pelo grau (maior primeiro)
    std::vector<int> ordemVertices(totalVertices);
    std::iota(ordemVertices.begin(), ordemVertices.end(), 0);
    std::sort(ordemVertices.begin(), ordemVertices.end(), [&](int u, int v) {
        return std::accumulate(matrizAdjacencia[u].begin(), matrizAdjacencia[u].end(), 0) >
               std::accumulate(matrizAdjacencia[v].begin(), matrizAdjacencia[v].end(), 0);
    });

    #pragma omp parallel
    {
        std::vector<int> melhorCliqueLocal;
        std::vector<int> cliqueAtual;

        #pragma omp for schedule(dynamic) nowait
        for (int i = 0; i < totalVertices; ++i) {
            std::vector<int> candidatos;
            for (int j = i + 1; j < totalVertices; ++j) {
                if (matrizAdjacencia[ordemVertices[i]][ordemVertices[j]] == 1) {
                    candidatos.push_back(ordemVertices[j]);
                }
            }

            cliqueAtual.push_back(ordemVertices[i]);
            ExplorarCliqueComPoda(matrizAdjacencia, cliqueAtual, melhorCliqueLocal, candidatos, maxCliqueSize);
            cliqueAtual.pop_back();
        }

        #pragma omp critical
        {
            if (melhorCliqueLocal.size() > melhorClique.size()) {
                melhorClique = melhorCliqueLocal;
            }
        }
    }

    return melhorClique;
}

int main() {
    int totalVertices;
    std::string caminhoArquivo = "grafo180.txt";

    auto matrizAdjacencia = CarregarGrafo(caminhoArquivo, totalVertices);

    auto inicioExecucao = std::chrono::high_resolution_clock::now();

    std::vector<int> melhorClique = ObterMaiorClique(matrizAdjacencia, totalVertices);

    auto tempoExecucao = std::chrono::duration_cast<std::chrono::duration<double>>(
                            std::chrono::high_resolution_clock::now() - inicioExecucao).count();

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

