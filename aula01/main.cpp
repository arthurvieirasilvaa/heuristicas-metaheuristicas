#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>

using namespace std;

void matrixAddEdge(vector<vector<int>> &matrix, int v1, int v2, int weight) {
    matrix[v1][v2] = weight;
    matrix[v2][v1] = weight;
}

void printMatrix(vector<vector<int>> matrix) {
    cout << "Matrix de Adjacencia:" << endl;
    int size = matrix.size();

    for(int i = 0; i < size; i++) {
        for(int j = 0; j < size; j++) {
            if(matrix[i][j] != -1) {
                cout << i+1 << " -> " << j+1 << " : " << matrix[i][j] << endl;
            } 
        }
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./main <nome_do_arquivo_de_entrada.txt>" << endl;
        exit(1);
    }
    
    ifstream file;

    file.open(argv[1]);

    // Verifica se o arquivo foi aberto:
    if(!file.is_open()) {
        cout << "Ocorreu um erro ao abrir o arquivo de entrada!" << endl;
        exit(1);
    }

    else {
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
        while(getline(file, line)) {
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
    }

    file.close();

    return 0;
}