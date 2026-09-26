#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <iomanip>
#include <cmath>
#include <random>
#include <chrono>
#include <algorithm>
#include <limits>
#include <cmath>

using namespace std;
using namespace chrono;

// Constantes utilizadas
constexpr double t0 = 1000.0;
constexpr double alpha = 0.95;
constexpr int SA_MAX = 100;

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

vector<int> generate_initial_solution(int n, mt19937 &gen) {
    vector<int> initial_solution;

    for(int i = 1; i <= n; i++) {
        initial_solution.push_back(i);
    }

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

vector<int> generate_neighbor(vector<int> solution, int n, mt19937 &gen) {
    uniform_int_distribution<> dist(0, n-1);

    int i = dist(gen);
    int j = dist(gen);

    // Verifica se a cidades que serão trocadas são iguais:
    while (i == j) {
        j = dist(gen);
    }
    
    vector<int> neighbor = solution;

    // Operador de vizinhança (2-swap):
    neighbor[i] = solution[j];
    neighbor[j] = solution[i];

    return neighbor;
}

vector<int> simulated_annealing(vector<vector<int>> &matrix, int n) {
    /*
        Gerador não-determinístico para inicializar o Mersenne Twister. Se
        random_device for realmente aleatória, a seed será realmente aleatória.
        Se random_device sempre retornar o mesmo valor, utilizar o operador XOR
        adiciona uma camada de variação:
    */
    random_device rd;
    auto seed = rd() ^ system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);
    uniform_real_distribution<> dist(0.0, 1.0);

    double T = t0; // temperatura inicial
    vector<int> s = generate_initial_solution(n, gen); // solução inicial aleatória
    cout << "Solucao inicial aleatoria s0: ";
    print_solution(s);

    vector<int> s_best = s;
    int i = 0;

    vector<int> neighbor;
    cout << "================================================================" << endl;
    cout << "Temperatura (T)\tDistancia total atual\tMelhor distancia total" << endl;
    cout << "================================================================" << endl;

    while (T > 0.0) {
        while (i < SA_MAX) {
            i++;
            neighbor = generate_neighbor(s, n, gen);

            int neighbor_total_distance = calculate_total_distance(matrix, neighbor, n);
            int s_total_distance = calculate_total_distance(matrix, s, n);
            if(neighbor_total_distance < s_total_distance) {
                s = neighbor;
                
                int s_best_total_distance = calculate_total_distance(matrix, s_best, n);
                if(neighbor_total_distance < s_best_total_distance) {
                    s_best = neighbor; // melhor solução até agora
                    s_best_total_distance = neighbor_total_distance;
                }
            }

            else if(dist(gen) < exp((s_total_distance - neighbor_total_distance) / T)) {
                s = neighbor;
            }
        }

        cout << fixed << T << "\t\t" << calculate_total_distance(matrix, s, n) << "\t\t" << calculate_total_distance(matrix, s_best, n) << endl;

        T *= alpha; // atualiza temperatura

        /*
            Verifica se T é suficientemente pequeno, uma vez que o seu tipo é
            double, ele nunca será 0. Forçamos que T tenha o valor 0:
        */
        if(T < 0.0000000001) {
            T = 0;
        }

        i = 0;
    }

    return s_best;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./tsp_simulated_annealing <nome_do_arquivo_de_entrada.txt>" << endl;
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
    cout << "t0 = " << t0 << endl;
    cout << "alfa = " << alpha << endl;
    cout << "SA_MAX = " << SA_MAX << endl;

    vector<int> solution = simulated_annealing(matrix, n);
    
    cout << "================================================================" << endl;
    cout << "Solucao: ";
    print_solution(solution);

    int total_distance = calculate_total_distance(matrix, solution, n);
    cout << "Distancia total: " << total_distance << endl;

    file.close();

    return 0;
}