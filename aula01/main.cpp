#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>

using namespace std;

struct Node {
    int vertex;
    int weight;
};

void matrixAddEdge(vector<vector<int>> &matrix, int v1, int v2, int weight) {
    matrix[v1][v2] = weight;
    matrix[v2][v1] = weight;
}

void printMatrix(vector<vector<int>> matrix) {
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

void listAddEdge(vector<vector<Node>> &adjacencyList, int v1, int v2, int weight) {
    Node node1;
    node1.vertex = v2 + 1;
    node1.weight = weight;
    adjacencyList[v1].push_back(node1);

    Node node2;
    node2.vertex = v1 + 1;
    node2.weight = weight;
    adjacencyList[v2].push_back(node2);
}

void printList(vector<vector<Node>> adjacencyList) {
    cout << "Lista de Adjacencia:" << endl;
    for(int i = 0; i < adjacencyList.size(); i++) {
        cout << i+1 << " : ";
        for(int j = 0; j < adjacencyList[i].size(); j++) {
            cout << "(Vertice = " << adjacencyList[i][j].vertex;
            cout << ", Peso = " << adjacencyList[i][j].weight << ") "; 
        }
        cout << endl;
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

    vector<vector<Node>> adjacencyList(nodes);
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
            listAddEdge(adjacencyList, v1-1, v2-1, weight);
        }
        
        else {
            cout << "A linha do arquivo de entrada está corrompida!" << endl;
            exit(1);
        }
    }

    printMatrix(matrix);
    cout << endl;
    printList(adjacencyList);

    file.close();

    return 0;
}