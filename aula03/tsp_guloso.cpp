#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <climits>

using namespace std;

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

vector<int> nearest_neighbor(vector<vector<int>> &matrix, int n, int initial_city) {
    vector<int> solution;
    vector<bool> visited_cities(n, false);

    // Verifica se a cidade inicial é válida:
    if(initial_city < 1 || initial_city > n) {
        cout << "A cidade inicial " << initial_city << " fornecida está inválida!";
        exit(1);
    }

    solution.push_back(initial_city);

    int city = initial_city-1;
    visited_cities[city] = true; // marca a cidade como visitada
    int shortest_distance, nearest_city;
    while (solution.size() < n) {
        shortest_distance = INT_MAX;
        nearest_city = -1;

        for(int i = 0; i < n; i++) {
            if(matrix[city][i] < shortest_distance && visited_cities[i] == false) {
                shortest_distance = matrix[city][i];
                nearest_city = i;
            }
        }


        // Não encontrou nenhuma cidade disponível:
        if(nearest_city == -1) {
            break;
        }

        solution.push_back(nearest_city+1); 
        city = nearest_city;
        visited_cities[city] = true; // marca a cidade como visitada
    }
    
    return solution;
}

vector<int> farthest_neighbor(vector<vector<int>> &matrix, int n, int initial_city) {
    vector<int> solution;
    vector<bool> visited_cities(n, false);

    // Verifica se a cidade inicial é válida:
    if(initial_city < 1 || initial_city > n) {
        cout << "A cidade inicial " << initial_city << " fornecida está inválida!";
        exit(1);
    }

    solution.push_back(initial_city);

    int city = initial_city-1;
    visited_cities[city] = true; // marca a cidade como visitada
    int greatest_distance, farthest_city;
    while (solution.size() < n) {
        greatest_distance = 0;
        farthest_city = -1;

        for(int i = 0; i < n; i++) {
            if(matrix[city][i] > greatest_distance && visited_cities[i] == false) {
                greatest_distance = matrix[city][i];
                farthest_city = i;
            }
        }


        // Não encontrou nenhuma cidade disponível:
        if(farthest_city == -1) {
            break;
        }

        solution.push_back(farthest_city+1); 
        city = farthest_city;
        visited_cities[city] = true; // marca a cidade como visitada
    }
    
    return solution;
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

void tsp(vector<vector<int>> &matrix, vector<int> &solution, int n) {
    int current_total_distance = calculate_total_distance(matrix, solution, n);

    bool solution_improved = true;
    int current_iteration = 0;
    vector<int> neighbor;
    int new_total_distance;
    while (current_iteration < MAX_ITERATIONS && solution_improved) {
        solution_improved = false;

        // Gerando a vizinhança da solução atual:
        for(int i = 0; i < n; i++) {
            for(int j = i+1; j < n; j++) {
                neighbor = solution;

                // Operador de vizinhança (2-swap):
                neighbor[i] = solution[j];
                neighbor[j] = solution[i];
                
                new_total_distance = calculate_total_distance(matrix, neighbor, n);

                // Política de melhoria (First Improvement):
                if(new_total_distance < current_total_distance) {
                    solution = neighbor;
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
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./tsp_guloso <nome_do_arquivo_de_entrada.txt>" << endl;
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

    // Gerando a solução gulosa (usando o vizinho mais próximo):
    vector<int> solution = nearest_neighbor(matrix, n, 1);

    cout << "Solucao gulosa (usando o vizinho mais proximo): ";
    print_solution(solution);

    int greedy_total_distance = calculate_total_distance(matrix, solution, n);
    cout << "Distancia total da solucao gulosa (usando o vizinho mais proximo): " << greedy_total_distance << endl;
    cout << "--------------------------------" << endl;

    // Aplicando a Busca Local na solução gulosa (usando o vizinho mais próximo):
    tsp(matrix, solution, n);
    int local_total_distance = calculate_total_distance(matrix, solution, n);
    
    cout << "Solucao apos a Busca Local: ";
    print_solution(solution);
    cout << "Distancia total da solucao apos a Busca Local: " << local_total_distance << endl;
    cout << "--------------------------------" << endl;

    // Gerando a solução gulosa (usando o vizinho mais distante):
    solution = farthest_neighbor(matrix, n, 1);

    cout << "Solucao gulosa (usando o vizinho mais distante): ";
    print_solution(solution);

    greedy_total_distance = calculate_total_distance(matrix, solution, n);
    cout << "Distancia total da solucao gulosa (usando o vizinho mais distante): " << greedy_total_distance << endl;
    cout << "--------------------------------" << endl;

    // Aplicando a Busca Local na solução gulosa (usando o vizinho mais distante):
    tsp(matrix, solution, n);
    local_total_distance = calculate_total_distance(matrix, solution, n);
    
    cout << "Solucao apos a Busca Local: ";
    print_solution(solution);
    cout << "Distancia total da solucao apos a Busca Local: " << local_total_distance << endl;
    cout << "--------------------------------" << endl;

    file.close();

    return 0;
}