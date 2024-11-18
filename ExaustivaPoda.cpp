#include <iostream>
#include <vector>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <numeric> // Para std::iota

// Função para ler o grafo a partir de um arquivo
std::vector<std::vector<int>> LerGrafo(const std::string& nomeArquivo, int& numVertices) {
    std::ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open()) {
        std::cerr << "Erro ao abrir o arquivo " << nomeArquivo << std::endl;
        return {};
    }

    int numArestas;
    arquivo >> numVertices >> numArestas;

    std::vector<std::vector<int>> grafo(numVertices, std::vector<int>(numVertices, 0));

    for (int i = 0; i < numArestas; ++i) {
        int u, v;
        arquivo >> u >> v;
        grafo[u - 1][v - 1] = 1;
        grafo[v - 1][u - 1] = 1;
    }

    return grafo;
}

// Função recursiva com poda para encontrar a clique máxima
void BuscarCliqueComPoda(const std::vector<std::vector<int>>& grafo, std::vector<int>& atualClique, 
                         std::vector<int>& cliqueMaxima, std::vector<int>& candidatos, int& maxTamanho) {
    if (atualClique.size() + candidatos.size() <= maxTamanho) return;

    while (!candidatos.empty()) {
        int v = candidatos.back();
        candidatos.pop_back();

        std::vector<int> novosCandidatos;
        for (int u : candidatos) {
            if (grafo[v][u] == 1) {
                novosCandidatos.push_back(u);
            }
        }

        atualClique.push_back(v);
        if (atualClique.size() > cliqueMaxima.size()) {
            cliqueMaxima = atualClique;
            maxTamanho = cliqueMaxima.size();
        }

        BuscarCliqueComPoda(grafo, atualClique, cliqueMaxima, novosCandidatos, maxTamanho);
        atualClique.pop_back();
    }
}

// Função principal para encontrar clique máxima com poda
std::vector<int> EncontrarCliqueMaximaHeuristica(const std::vector<std::vector<int>>& grafo) {
    int numVertices = grafo.size();
    std::vector<int> candidatos(numVertices);
    std::iota(candidatos.begin(), candidatos.end(), 0); // Inicializa com 0, 1, ..., numVertices-1

    std::vector<int> cliqueMaxima;
    std::vector<int> atualClique;
    int maxTamanho = 0;

    BuscarCliqueComPoda(grafo, atualClique, cliqueMaxima, candidatos, maxTamanho);
    return cliqueMaxima;
}

int main() {
    int numVertices;
    std::string nomeArquivo = "grafo180.txt";

    std::vector<std::vector<int>> grafo = LerGrafo(nomeArquivo, numVertices);
    if (grafo.empty()) return 1;

    auto inicio = std::chrono::high_resolution_clock::now();
    std::vector<int> cliqueMaxima = EncontrarCliqueMaximaHeuristica(grafo);
    auto fim = std::chrono::high_resolution_clock::now();

    double duracao = std::chrono::duration_cast<std::chrono::duration<double>>(fim - inicio).count();

    std::cout << "Clique máxima encontrada: ";
    for (int v : cliqueMaxima) {
        std::cout << v + 1 << " ";
    }
    std::cout << "\nTamanho da clique máxima: " << cliqueMaxima.size() << std::endl;
    std::cout << std::fixed << std::setprecision(6) << "Tempo total de execução: " << duracao << " segundos\n";

    return 0;
}




