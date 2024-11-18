#include <mpi.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <numeric>

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
            maxCliqueSize = std::max(maxCliqueSize, (int)melhorCliqueLocal.size());
        }

        ExplorarCliqueComPoda(matrizAdjacencia, cliqueAtual, melhorCliqueLocal, novosCandidatos, maxCliqueSize);
        cliqueAtual.pop_back();
    }
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int totalVertices;
    std::string caminhoArquivo = "grafo180.txt";

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    std::vector<std::vector<int>> matrizAdjacencia;
    if (rank == 0) {
        matrizAdjacencia = CarregarGrafo(caminhoArquivo, totalVertices);
    }

    MPI_Bcast(&totalVertices, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (rank != 0) {
        matrizAdjacencia.resize(totalVertices, std::vector<int>(totalVertices, 0));
    }

    for (int i = 0; i < totalVertices; ++i) {
        MPI_Bcast(matrizAdjacencia[i].data(), totalVertices, MPI_INT, 0, MPI_COMM_WORLD);
    }

    std::vector<int> ordemVertices(totalVertices);
    std::iota(ordemVertices.begin(), ordemVertices.end(), 0);
    std::sort(ordemVertices.begin(), ordemVertices.end(), [&](int u, int v) {
        return std::accumulate(matrizAdjacencia[u].begin(), matrizAdjacencia[u].end(), 0) >
               std::accumulate(matrizAdjacencia[v].begin(), matrizAdjacencia[v].end(), 0);
    });

    int verticesPorProcessador = totalVertices / size;
    int inicio = rank * verticesPorProcessador;
    int fim = (rank == size - 1) ? totalVertices : inicio + verticesPorProcessador;

    std::vector<int> melhorCliqueLocal;
    double tempoInicio = MPI_Wtime();

    for (int i = inicio; i < fim; ++i) {
        std::vector<int> cliqueAtual = {ordemVertices[i]};
        std::vector<int> candidatos;

        for (int j = i + 1; j < totalVertices; ++j) {
            if (matrizAdjacencia[ordemVertices[i]][ordemVertices[j]] == 1) {
                candidatos.push_back(ordemVertices[j]);
            }
        }

        int maxCliqueSize = melhorCliqueLocal.size();
        ExplorarCliqueComPoda(matrizAdjacencia, cliqueAtual, melhorCliqueLocal, candidatos, maxCliqueSize);
    }

    double tempoExecucaoLocal = MPI_Wtime() - tempoInicio;

    if (rank != 0) {
        int tamanhoCliqueLocal = melhorCliqueLocal.size();
        MPI_Send(&tamanhoCliqueLocal, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
        MPI_Send(melhorCliqueLocal.data(), tamanhoCliqueLocal, MPI_INT, 0, 1, MPI_COMM_WORLD);
    } else {
        std::vector<int> melhorCliqueGlobal = melhorCliqueLocal;
        for (int i = 1; i < size; ++i) {
            int tamanhoCliqueRecebida;
            MPI_Recv(&tamanhoCliqueRecebida, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            std::vector<int> cliqueRecebida(tamanhoCliqueRecebida);
            MPI_Recv(cliqueRecebida.data(), tamanhoCliqueRecebida, MPI_INT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

            if (cliqueRecebida.size() > melhorCliqueGlobal.size()) {
                melhorCliqueGlobal = cliqueRecebida;
            }
        }

        double tempoExecucaoGlobal;
        MPI_Reduce(&tempoExecucaoLocal, &tempoExecucaoGlobal, 1, MPI_DOUBLE, MPI_MAX, 0, MPI_COMM_WORLD);

        std::cout << "Maior clique -> ";
        for (int vertice : melhorCliqueGlobal) {
            std::cout << vertice + 1 << " "; // Para ajustar caso a indexação comece de 1
        }
        std::cout << "\nTamanho da maior clique -> " << melhorCliqueGlobal.size() << "\n";
        std::cout << std::fixed << std::setprecision(6);
        std::cout << "Tempo total de execução -> " << tempoExecucaoGlobal << " segundos\n";
    }

    MPI_Finalize();
    return 0;
}






