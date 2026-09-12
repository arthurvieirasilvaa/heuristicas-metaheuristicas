#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>

using namespace std;

void matrixAddEdge(vector<vector<int>> &matrix, int v1, int v2, int distance) {
    matrix[v1][v2] = distance;
    matrix[v2][v1] = distance;
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

int calculate_distance(int xi, int yi, int xj, int yj) {
    int xd = xi - xj;
    int yd = yi - yj;
    int dij = (int) sqrt(((xd * xd) + (yd * yd)) + 0.5);

    return dij;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./busca_local_2 <nome_do_arquivo_de_entrada.txt>" << endl;
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
    
    vector<int> x;
    vector<int> y;

    int n = 0;
    while(getline(file, line)) {
        stringstream ss(line);

        // Fazendo o parsing do stream de string:
        int city, xi, yi;
        if(ss >> city >> xi >> yi) {
            x.push_back(xi);
            y.push_back(yi);
            n++;
        }
    }

    vector<vector<int>> matrix(n, vector<int>(n));
    int distance;
    for(int i = 0; i < n; i++) {
        for(int j = i+1; j < n; j++) {
            distance = calculate_distance(x[i], y[i], x[j], y[j]);
            matrixAddEdge(matrix, i, j, distance);
        }
    }

    printMatrix(matrix);

    cout << "Numero de cidades: " << n << endl;

    file.close();

    return 0;
}