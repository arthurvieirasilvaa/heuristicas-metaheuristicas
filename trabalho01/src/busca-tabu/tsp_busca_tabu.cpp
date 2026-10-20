#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <climits>
#include <array>

using namespace std;
using namespace chrono;

enum StopCriterion {
    ITERATIONS_WITHOUT_IMPROVEMENT,
    TOTAL_ITERATIONS
};

struct Configuration {
    const int TABU_TENURE;
    const int MAX_TOTAL_ITERATIONS;
    const int MAX_ITERATIONS_WITHOUT_IMPROVEMENT;
    StopCriterion stop_criterion;
};

constexpr int MAX_ITERATIONS_WITHOUT_IMPROVEMENT = 100; // número máximo de iterações sem melhora
constexpr int TABU_TENURE = 5; // Duração Tabu

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

int calculate_distance(double xi, double yi, double xj, double yj) {
    double xd = xi - xj;
    double yd = yi - yj;
    double dij = (int) (sqrt(xd * xd + yd * yd) + 0.5);

    return dij;
}

vector<int> greedy_nearest_neighbor(vector<vector<int>> &matrix, int n, int initial_city) {
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

array<int, 2> select_oldest_tabu_move(vector<vector<int>> &matrix, int n, vector<int> &s,
    const vector<vector<int>> &T, vector<int> &best_neighbor, int &best_neighbor_total_distance) {
    array<int, 2> indexes = {-1, -1};
    int min_tabu = INT_MAX;
    vector<int> neighbor;   
    
    for(int i = 0; i < n; i++) {
        for(int j = i+1; j < n; j++) {
            // O movimento tabu há mais tempo na lista é realizado:
            if(T[i][j-i-1] > 0 && T[i][j-i-1] < min_tabu) {
                neighbor = s;

                // Operador de vizinhança (2-swap):
                neighbor[i] = s[j];
                neighbor[j] = s[i];

                min_tabu = T[i][j-i-1];
                indexes.at(0) = i;
                indexes.at(1) = j;

                best_neighbor = neighbor;
                best_neighbor_total_distance = calculate_total_distance(matrix, best_neighbor, n);
            }
        }
    }
    
    return indexes;
}

vector<int> tabu_search(vector<vector<int>> &matrix, int n, int initial_city, Configuration configuration) {
    // Gerando a solução gulosa (usando o vizinho mais próximo):
    vector<int> s = greedy_nearest_neighbor(matrix, n, initial_city); // solução inicial

    int current_total_distance = calculate_total_distance(matrix, s, n);

    vector<int> sbest = s;
    int iter = 0;
    int iterations_without_improvement = 0;
    vector<vector<int>> T(n);
    
    // O número de posições em cada linha da matriz diminui:
    for(int i = 0; i < n; i++) {
        T[i].resize(n-i-1, 0);
    }
  
    vector<int> neighbor, best_neighbor;

    array<int, 2> indexes;
    int new_total_distance;
    int best_total_distance = current_total_distance;
    int best_neighbor_total_distance;

    bool running = true;
    while (running) {
        iter++;
        indexes.at(0) = -1;
        indexes.at(1) = -1;
        best_neighbor.clear();
        best_neighbor_total_distance = INT_MAX;

        // Gerando a vizinhança da solução atual:
        for(int i = 0; i < n; i++) {
            for(int j = i+1; j < n; j++) {
                neighbor = s;

                // Operador de vizinhança (2-swap):
                neighbor[i] = s[j];
                neighbor[j] = s[i];    
            
                new_total_distance = calculate_total_distance(matrix, neighbor, n);
            
                /*
                    Critério de Ativação por objetivo regional e verifica se o 
                    movimento não é tabu:
                */  
                if(new_total_distance < best_total_distance || T[i][j-i-1] == 0) {
                    if(new_total_distance < best_neighbor_total_distance) {
                        best_neighbor = neighbor;
                        best_neighbor_total_distance = new_total_distance;
                        indexes.at(0) = i;
                        indexes.at(1) = j;
                    }
                }
            }
        }

        /*
            Se todos os movimentos estão proibidos, permite o movimento tabu há
            mais tempo na matriz (Critério de Aspiração por default):
        */
        if(indexes.at(0) == -1 || indexes.at(1) == -1) {
            cout << "Criterio de Aspiracao por default" << endl;
            
            indexes = select_oldest_tabu_move(matrix, n, s, T, best_neighbor, best_neighbor_total_distance);
            
            // Verifica se não há nenhum movimento viável:
            if(indexes.at(0) == -1 || indexes.at(1) == -1) {
                break;
            }
        }

        // Matriz Tabu com Prazo (Decrescente):
        for(int i = 0; i < n; i++) {
            for(int j = 0; j < T[i].size(); j++) {
                if(T[i][j] > 0) {
                    T[i][j]--;
                }
            }
        } 

        s = best_neighbor;

        // Atualiza a Matriz Tabu T:
        T[indexes.at(0)][indexes.at(1) - indexes.at(0) - 1] = configuration.TABU_TENURE;

        if(best_neighbor_total_distance < best_total_distance) {
            sbest = s;
            best_total_distance = best_neighbor_total_distance;
            iterations_without_improvement = 0;
        }

        else {
            iterations_without_improvement++;
        }

        // Verifica qual o critério de parada:
        if(configuration.stop_criterion == ITERATIONS_WITHOUT_IMPROVEMENT) {
            // Número máximo de iterações:
            if(iterations_without_improvement >= configuration.MAX_ITERATIONS_WITHOUT_IMPROVEMENT) {
                running = false;
            }
        }

        else if(configuration.stop_criterion == TOTAL_ITERATIONS) {
            // Número de iterações sem melhorias:
            if(iter >= configuration.MAX_TOTAL_ITERATIONS) {
                running = false;
            }
        }
    }

    return sbest;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./tsp_busca_tabu <nome_do_arquivo_de_entrada>" << endl;
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
    
    vector<double> x;
    vector<double> y;

    int n = 0;
    while(getline(file, line)) {
        stringstream ss(line);

        // Fazendo o parsing do stream de string:
        int city;
        double xi, yi;
        if(ss >> city >> xi >> yi) {
            x.push_back(xi);
            y.push_back(yi);
            n++;
        }
    }

    cout << "Numero de cidades: " << n << endl;

    vector<vector<int>> matrix(n, vector<int>(n));
    int distance;
    for(int i = 0; i < n; i++) {
        for(int j = i+1; j < n; j++) {
            distance = calculate_distance(x[i], y[i], x[j], y[j]);
            matrixAddEdge(matrix, i, j, distance);
        }
    }

    Configuration configuration1 = {10, 200, 0, TOTAL_ITERATIONS};
    Configuration configuration2 = {5, 0, 100, ITERATIONS_WITHOUT_IMPROVEMENT};

    cout << "Configuracao 1:" << endl;

    auto start = steady_clock::now();

    vector<int> sbest = tabu_search(matrix, n, 1, configuration1);
    cout << "================================================================" << endl;
    cout << "Solucao: ";
    print_solution(sbest);
    
    auto end = steady_clock::now();

    auto duration = duration_cast<microseconds>(end - start);
    
    cout << "================================================================" << endl;

    int total_distance = calculate_total_distance(matrix, sbest, n);
    cout << "Distancia total: " << total_distance << endl;

    cout << "Duracao: " << duration.count() << " microssegundos" << endl;

    start = steady_clock::now();

    cout << "Configuracao 2:" << endl;
    
    sbest = tabu_search(matrix, n, 1, configuration2);
    cout << "================================================================" << endl;
    cout << "Solucao: ";
    print_solution(sbest);
    
    end = steady_clock::now();

    duration = duration_cast<microseconds>(end - start);
    
    cout << "================================================================" << endl;

    total_distance = calculate_total_distance(matrix, sbest, n);
    cout << "Distancia total: " << total_distance << endl;

    cout << "Duracao: " << duration.count() << " microssegundos" << endl;

    file.close();

    return 0;
}