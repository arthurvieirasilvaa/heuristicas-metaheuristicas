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
#include <numeric>

using namespace std;
using namespace chrono;

// Função de resfriamento:
enum CoolingRate {
    LINEAR,
    GEOMETRIC
};

// Valores utilizados:
struct Configuration {
    double t0;
    double alpha;
    double beta;
    const int SA_MAX;
    CoolingRate cooling_rate;
};

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

vector<int> simulated_annealing(vector<vector<int>> &matrix, int n, Configuration configuration) {
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

    double T = configuration.t0; // temperatura inicial
    vector<int> s = generate_initial_solution(n, gen); // solução inicial aleatória

    vector<int> s_best = s;
    int i = 0;

    vector<int> neighbor;

    while (T > 0.0) {
        while (i < configuration.SA_MAX) {
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

        // Verifica qual é a função de resfriamento utilizada:
        if(configuration.cooling_rate == GEOMETRIC) {
            T *= configuration.alpha; // atualiza temperatura
        }

        else if(configuration.cooling_rate == LINEAR) {
            T -= configuration.beta; // atualiza temperatura
        }

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
        cout << "Formato correto: ./tsp_simulated_annealing <nome_do_arquivo_de_entrada>" << endl;
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

    vector<vector<int>> matrix(n, vector<int>(n));
    int distance;
    for(int i = 0; i < n; i++) {
        for(int j = i+1; j < n; j++) {
            distance = calculate_distance(x[i], y[i], x[j], y[j]);
            matrixAddEdge(matrix, i, j, distance);
        }
    }

    cout << "Numero de cidades: " << n << endl;

    Configuration configuration1 = {1000.0, 0.99, 0.0, 1000, GEOMETRIC};
    Configuration configuration2 = {5000.0, 0.0, 2.0, 1000, LINEAR};

    vector<int> total_distances_results(10);
    vector<double> durations(10);
    int total_distance;

    cout << "Configuracao 1:" << endl;
    cout << "t0 = " << configuration1.t0 << endl;
    cout << "alfa = " << configuration1.alpha << endl;
    cout << "SA_max = " << configuration1.SA_MAX << endl;

    for(int i = 0; i < 10; i++) {
        auto start = steady_clock::now();

        vector<int> solution = simulated_annealing(matrix, n, configuration1);

        auto end = steady_clock::now();

        auto duration = duration_cast<microseconds>(end - start);

        cout << "================================================================" << endl;
        cout << "Solucao: ";
        print_solution(solution);

        int total_distance = calculate_total_distance(matrix, solution, n);
        cout << "Distancia total: " << total_distance << endl;

        cout << "Duracao: " << duration.count() << " microssegundos" << endl;
        cout << "================================================================" << endl;

        total_distances_results[i] = total_distance;
        durations[i] = duration.count();
    }

    int best_total_distance_result = *min_element(total_distances_results.begin(), total_distances_results.end());
    int worst_total_distance_result = *max_element(total_distances_results.begin(), total_distances_results.end());
    double mean_total_distances_results = accumulate(total_distances_results.begin(), total_distances_results.end(), 0.0) / 10;

    cout << "Melhor distancia total: " << best_total_distance_result << endl;
    cout << "Pior distancia total: " << worst_total_distance_result << endl;
    cout << "Media das distancias totais: " << mean_total_distances_results << endl;

    cout << "Configuracao 2:" << endl;
    cout << "t0 = " << configuration2.t0 << endl;
    cout << "beta = " << configuration2.beta << endl;
    cout << "SA_max = " << configuration2.SA_MAX << endl;
    
    for(int i = 0; i < 10; i++) {
        auto start = steady_clock::now();

        vector<int> solution = simulated_annealing(matrix, n, configuration2);

        auto end = steady_clock::now();

        auto duration = duration_cast<microseconds>(end - start);

        cout << "================================================================" << endl;
        cout << "Solucao: ";
        print_solution(solution);

        int total_distance = calculate_total_distance(matrix, solution, n);
        cout << "Distancia total: " << total_distance << endl;

        cout << "Duracao: " << duration.count() << " microssegundos" << endl;
        cout << "================================================================" << endl;

        total_distances_results[i] = total_distance;
        durations[i] = duration.count();
    }

    best_total_distance_result = *min_element(total_distances_results.begin(), total_distances_results.end());
    worst_total_distance_result = *max_element(total_distances_results.begin(), total_distances_results.end());
    mean_total_distances_results = accumulate(total_distances_results.begin(), total_distances_results.end(), 0.0) / 10;

    cout << "Melhor distancia total: " << best_total_distance_result << endl;
    cout << "Pior distancia total: " << worst_total_distance_result << endl;
    cout << "Media das distancias totais: " << mean_total_distances_results << endl;

    file.close();

    return 0;
}
