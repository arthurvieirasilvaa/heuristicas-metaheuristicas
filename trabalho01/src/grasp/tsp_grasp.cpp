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
#include <numeric>

using namespace std;
using namespace chrono;

struct Configuration {
    double alpha;
    const int MAX_ITERATIONS;
    const int GRASPMax;
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

vector<int> local_search(vector<vector<int>> &matrix, int n, vector<int> &s, int MAX_ITERATIONS) {
    int current_total_distance = calculate_total_distance(matrix, s, n);

    bool solution_improved = true;
    int current_iteration = 0;
    vector<int> neighbor;
    int new_total_distance;
    while (current_iteration < MAX_ITERATIONS && solution_improved) {
        solution_improved = false;

        // Gerando a vizinhança da solução atual:
        for(int i = 0; i < n; i++) {
            for(int j = i+1; j < n; j++) {
                neighbor = s;

                // Operador de vizinhança (2-swap):
                neighbor[i] = s[j];
                neighbor[j] = s[i];
                
                new_total_distance = calculate_total_distance(matrix, neighbor, n);

                // Política de melhoria (First Improvement):
                if(new_total_distance < current_total_distance) {
                    s = neighbor;
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

    return s;
}

void create_gc(vector<vector<int>> &matrix, vector<int> s, vector<int> LC, vector<int> &gc) {
    gc.clear();

    int last_city = s[s.size()-1]; // última cidade inserida

    int c_distance_to_last_city; // distância do candidato c da LC à última cidade inserida
    for(int i = 0; i < LC.size(); i++) {
        c_distance_to_last_city = matrix[LC[i]-1][last_city-1];
        gc.push_back(c_distance_to_last_city);
    }
}

void create_LRC(vector<vector<int>> &matrix, vector<int> s, vector<int> LC, vector<int> &LRC, double alpha) {
    vector<int> gc;
    create_gc(matrix, s, LC, gc);

    int cmin = *min_element(gc.begin(), gc.end());
    int cmax = *max_element(gc.begin(), gc.end());

    LRC.clear();

    double g = cmin + (alpha * (cmax - cmin));

    for(int i = 0; i < gc.size(); i++) {
        if(gc[i] <= g) {
            LRC.push_back(LC[i]);
        }
    }
}

vector<int> random_greedy_construction(vector<vector<int>> &matrix, int n, const double alpha, mt19937 &gen) {
    vector<int> s;
    s.push_back(1); // insere a cidade inicial

    // Cria LC:
    vector<int> LC, LRC;
    for(int i = 2; i <= n; i++) {
        LC.push_back(i);
    }

    int x;
    while (LC.size() > 0) {
        // Cria LRC (Lista Restrita de Candidatos):
        create_LRC(matrix, s, LC, LRC, alpha);
        uniform_int_distribution<size_t> dis(0, LRC.size()-1);
        size_t random_index = dis(gen);

        x = LRC[random_index]; // um elemento aleatório da LRC
        s.push_back(x);

        auto iterator = find(LC.begin(), LC.end(), x);
        LC.erase(iterator); // atualiza LC (remove x, atualiza valores)
    }
    
    return s;
}
 
vector<int> grasp(vector<vector<int>> &matrix, int n, Configuration configuration, mt19937 &gen) {
    vector<int> s, sbest;
    const double alpha = configuration.alpha;
    int current_total_distance;
    int best_total_distance = INT_MAX;
    int iter = 0;

    while (iter < configuration.GRASPMax) {
        s = random_greedy_construction(matrix, n, alpha, gen);
        s = local_search(matrix, n, s, configuration.MAX_ITERATIONS);

        current_total_distance = calculate_total_distance(matrix, s, n);

        if(current_total_distance < best_total_distance) {
            sbest = s;
            best_total_distance = current_total_distance;
        }

        iter++;
    }
    
    return sbest;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./tsp_grasp <nome_do_arquivo_de_entrada>" << endl;
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

    /*
        Gerador não-determinístico para inicializar o Mersenne Twister. Se
        random_device for realmente aleatória, a seed será realmente aleatória.
        Se random_device sempre retornar o mesmo valor, utilizar o operador XOR
        adiciona uma camada de variação:
    */
    random_device rd;
    auto seed = rd() ^ system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);

    Configuration configuration1 = {0.3, 100, 10};
    Configuration configuration2 = {0.1, 100, 50};

    vector<int> total_distances_results(10);
    vector<double> durations(10);
    int total_distance;

    cout << "Configuracao 1:" << endl;
    cout << "alfa = " << configuration1.alpha << endl;
    cout << "GRASPMax = " << configuration1.GRASPMax << endl;

    for(int i = 0; i < 10; i++) {
        auto start = steady_clock::now();

        vector<int> s = grasp(matrix, n, configuration1, gen);

        auto end = steady_clock::now();

        auto duration = duration_cast<microseconds>(end - start);

        cout << "================================================================" << endl;
        cout << "Solucao: ";
        print_solution(s);

        int total_distance = calculate_total_distance(matrix, s, n);
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
    cout << "alfa = " << configuration2.alpha << endl;
    cout << "GRASPMax = " << configuration2.GRASPMax << endl;

    for(int i = 0; i < 10; i++) {
        auto start = steady_clock::now();

        vector<int> s = grasp(matrix, n, configuration2, gen);

        auto end = steady_clock::now();

        auto duration = duration_cast<microseconds>(end - start);

        cout << "================================================================" << endl;
        cout << "Solucao: ";
        print_solution(s);

        int total_distance = calculate_total_distance(matrix, s, n);
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
