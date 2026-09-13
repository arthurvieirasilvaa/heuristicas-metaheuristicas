#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>

using namespace std;
using namespace chrono;

constexpr int MAX_ITERATIONS = 1000;

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
    int dij = (int) (sqrt(xd * xd + yd * yd) + 0.5);

    return dij;
}

vector<int> generate_initial_solution(int n) {
    vector<int> initial_solution;

    for(int i = 1; i <= n; i++) {
        initial_solution.push_back(i);
    }

    /*
        Gerador não-determinístico para inicializar o Mersenne Twister. Se
        random_device for realmente aleatória, a seed será realmente aleatória.
        Se random_device sempre retornar o mesmo valor, utilizar o operador XOR
        adiciona uma camada de variação:
    */
    random_device rd;
    auto seed = rd() ^ system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);

    // Reorganiza aleatoriamente os elementos do vetor:
    shuffle(initial_solution.begin(), initial_solution.end(), gen);

    return initial_solution;
}

int calculate_total_distance(vector<vector<int>> &matrix, vector<int> &solution, int n) {
    int distance = 0;
    for(int i = 0; i < n-1; i++) {
        distance += matrix[solution[i]-1][solution[i+1]-1];
    }

    // Distância da última cidade para a primeira:
    distance += matrix[solution[n-1]-1][solution[0]-1];

    return distance;
}

void print_solution(vector<int> &solution) {
    for(int i = 0; i < solution.size(); i++) {
        cout << solution[i] << " ";
    }
    cout << endl;
}

void tsp(vector<vector<int>> &matrix, int n) {
    // Gerando a solução inicial aleatória (melhor solução atual):
    vector<int> current_solution = generate_initial_solution(n);

    cout << "Solucao inicial: ";
    print_solution(current_solution);

    int current_total_distance = calculate_total_distance(matrix, current_solution, n);
    cout << "Distancia total da solucao inicial: " << current_total_distance << endl;
    cout << "--------------------------------" << endl;

    bool solution_improved = true;
    int current_iteration = 0;
    vector<int> neighbor;
    int new_total_distance;
    while (current_iteration < MAX_ITERATIONS && solution_improved) {
        solution_improved = false;

        // Gerando a vizinhança da solução atual:
        for(int i = 0; i < n; i++) {
            for(int j = i+1; j < n; j++) {
                neighbor = current_solution;

                // Operador de vizinhança (2-swap):
                neighbor[i] = current_solution[j];
                neighbor[j] = current_solution[i];
                
                new_total_distance = calculate_total_distance(matrix, neighbor, n);

                // Política de melhoria (First Improvement):
                if(new_total_distance < current_total_distance) {
                    current_solution = neighbor;
                    current_total_distance = new_total_distance;
                    solution_improved = true;
                    break;
                }
            }

            // First Improvement:
            if(solution_improved) {
                break;
            }
        }

        current_iteration++;
    }

    cout << "Solucao: ";
    print_solution(current_solution);
    cout << "Distancia total da solucao: " << current_total_distance << endl;
    cout << "--------------------------------" << endl;
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

    tsp(matrix, n);

    file.close();

    return 0;
}