#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <chrono>

using namespace std;
using namespace std::chrono;

void matrixAddEdge(vector<vector<int>> &matrix, int v1, int v2, int weight) {
    matrix[v1][v2] = weight;
    matrix[v2][v1] = weight;
}

void printMatrix(const vector<vector<int>> &matrix) {
    cout << "Matrix de Adjacencia:" << endl;
    int size = matrix.size();

    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
                cout << setw(3) << matrix[i][j] << " ";
        }
        cout << endl;
    }
}

void setMatrixDiagonalToZero(vector<vector<int>> &matrix) {
    int size = matrix.size();

    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(i == j) {
                matrix[i][j] = 0;
            }
        }
    }
}

void print_permutation(const vector<int> &permutation) {
    for(int i = 0; i < permutation.size(); i++) {
        cout << permutation[i] << " ";
    }
    cout << endl;
}

void generate_permutation(vector<vector<int>> &matrix, vector<int> &permutation, int begin, int nodes) {
    if(begin == nodes) {
        print_permutation(permutation);
        return;
    }
    
    int n1, n2;
    for(int i = begin; i < nodes; i++) { 
        // Trocando duas posições:
        n1 = permutation[i];
        n2 = permutation[begin];
        permutation[begin] = n1;
        permutation[i] = n2;

        // Verifica se a cidade adicionada foi a primeira:
        if(begin == 0) {
            generate_permutation(matrix, permutation, begin+1, nodes);
        }

        else {
            int v1 = permutation[begin-1];
            int v2 = permutation[begin];
            
            /*
                Verifica se existe aresta entre a cidada mais recentemente
                adicionada e sua antecessora antes de continuar a construção da 
                permutação (Backtracking):
            */
            if(matrix[v1-1][v2-1] != -1) {
                generate_permutation(matrix, permutation, begin+1, nodes);
            }
        }

        // Trocando duas posições:
        n1 = permutation[i];
        n2 = permutation[begin];
        permutation[begin] = n1;
        permutation[i] = n2;
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./busca_exaustiva_2b <nome_do_arquivo_de_entrada.txt>" << endl;
        exit(1);
    }
    
    ifstream file;
    file.open(argv[1]);

    // Verifica se o arquivo foi aberto:
    if(!file.is_open()) {
        cout << "Ocorreu um erro ao abrir o arquivo de entrada!" << endl;
        exit(1);
    }

    string line;
    getline(file, line); // lendo o número de vértices e de arestas do grafo
    
    stringstream ss(line);
    int nodes, edges;

    // Fazendo o parsing do stream de string:
    if(ss >> nodes >> edges) {
        cout << "Nos: " << nodes << endl;
        cout << "Arestas: " << edges << endl;
    }
    
    else {
        cout << "A linha do arquivo de entrada está corrompida!" << endl;
        exit(1);
    }

    // Inicializa a Matriz de Adjacência com pesos -1:
    vector<vector<int>> matrix(nodes, vector<int>(nodes, -1));
    setMatrixDiagonalToZero(matrix);

    while(getline(file, line)) {
        // Ignora linha em branco:
        if(line.empty()) {
            continue;
        }

        stringstream ss(line);

        // Fazendo o parsing do stream de string:
        int v1, v2, weight;
        if(ss >> v1 >> v2 >> weight) {
            matrixAddEdge(matrix, v1-1, v2-1, weight);
        }
        
        else {
            cout << "A linha do arquivo de entrada está corrompida!" << endl;
            exit(1);
        }
    }

    printMatrix(matrix);

    vector<int> permutation;
    for(int i = 1; i <= nodes; i++) {
        permutation.push_back(i);
    }

    auto start = steady_clock::now();
    generate_permutation(matrix, permutation, 0, nodes);
    auto end = steady_clock::now();

    auto duration = duration_cast<microseconds>(end - start);

    cout << "Duracao: " << duration.count() << " microssegundos" << endl;

    file.close();

    return 0;
}
