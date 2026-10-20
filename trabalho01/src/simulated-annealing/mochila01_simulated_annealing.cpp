#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cfloat>
#include <random>
#include <chrono>
#include <algorithm>
#include <climits>
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

int calculate_profit(const vector<int> &profits, string current_solution) {
    int profit = 0;

    for(int i = 0; i < profits.size(); i++) {
        if(current_solution[i] == '1') {
            profit += profits[i];
        }
    }

    return profit;
}

int calculate_weight(const vector<int> &weights, string current_solution) {
    int weight = 0;

    for(int i = 0; i < weights.size(); i++) {
        if(current_solution[i] == '1') {
            weight += weights[i];
        }
    }

    return weight;
}

string generate_initial_solution(int n, int wmax, const vector<int> &weights, mt19937 &gen) {
    string initial_solution(n, '0'); // nenhum item na mochila

    // Distribuição uniforme entre 0 e 1 (inclusive):
    uniform_int_distribution<int> dist(0, 1);

    /*
        Criando um vetor com os índices de 0 até n-1 e reorganizando
        aleatoriamente a ordem de inserção:
    */
    vector<int> indexes;
    for(int i = 0; i < n; i++) {
        indexes.push_back(i);
    }
    shuffle(indexes.begin(), indexes.end(), gen);

    int current_weight = 0;
    for(int i = 0; i < n; i++) {
        int index = indexes[i];

        /*
            Verifica se a adição do item não ultrapassará a capacidade da
            mochila:
        */
        if(current_weight + weights[index] <= wmax) {
            initial_solution[index] = '1';
            current_weight += weights[index];
        }   
    }

    return initial_solution;
}

string generate_neighbor(string s, int n, mt19937 &gen) {
    uniform_int_distribution<> dist(0, n-1);

    int index = dist(gen);
    
    string neighbor = s;

    // Operador de vizinhança (flip):
    if(s[index] == '0') {
        neighbor[index] = '1';
    }
        
    else {
        neighbor[index] = '0';
    }

    return neighbor;
}

string simulated_annealing(int n, int wmax, const vector<int> &profits, const vector<int> &weights, Configuration configuration) {
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
    string s = generate_initial_solution(n, wmax, weights, gen); // solução inicial aleatória
    
    int current_profit = calculate_profit(profits, s);
    int current_weight = calculate_weight(weights, s);

    string s_best = s;
    int s_best_total_profit = current_profit;
    int i = 0;

    string neighbor;
    int neighbor_total_profit, neighbor_weight;

    while (T > 0.0) {
        while (i < configuration.SA_MAX) {
            i++;
            neighbor = generate_neighbor(s, n, gen);
            neighbor_weight = calculate_weight(weights, neighbor);

            // Ignora soluções vizinhas inviáveis:
            if(neighbor_weight > wmax) {
                continue;
            }

            neighbor_total_profit = calculate_profit(profits, neighbor);
            if(neighbor_total_profit > current_profit) {
                s = neighbor;
                current_profit = neighbor_total_profit;
                current_weight = neighbor_weight;

                if(current_profit > s_best_total_profit) {
                    s_best = neighbor; // melhor solução até agora
                    s_best_total_profit = current_profit;
                }
            }

            else if(dist(gen) < exp((neighbor_total_profit - current_profit) / T)) {
                s = neighbor;
                current_profit = neighbor_total_profit;
                current_weight = neighbor_weight;
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

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./mochila01_simulated_annealing <nome_do_arquivo_de_entrada>" << endl;
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
    getline(file, line); // lendo o número de itens e a capacidade da mochila

    stringstream ss(line);
    int n, wmax;
    // Fazendo o parsing do stream de string:
    if(ss >> n >> wmax) {
        cout << "Numero de itens: " << n << endl;
        cout << "Capacidade da mochila: " << wmax << endl;
    }
    
    else {
        cout << "A linha do arquivo de entrada esta corrompida!" << endl;
        exit(1);
    }

    vector<int> profits;
    vector<int> weights;
    while (getline(file, line)) {
        // Ignora linha em branco:
        if(line.empty()) {
            continue;
        }

        stringstream ss(line);

        // Fazendo o parsing do stream de string:
        int vi, wi;
        if(ss >> vi >> wi) {
            profits.push_back(vi);
            weights.push_back(wi);
        }
        
        else {
            cout << "A linha do arquivo de entrada está corrompida!" << endl;
            exit(1);
        }
    }

    Configuration configuration1 = {1000.0, 0.99, 0.0, 1000, GEOMETRIC};
    Configuration configuration2 = {5000.0, 0.0, 2.0, 1000, LINEAR};

    vector<int> profits_results(10);
    vector<double> durations(10);
    int total_profit, weight;

    cout << "Configuracao 1:" << endl;
    cout << "t0 = " << configuration1.t0 << endl;
    cout << "alfa = " << configuration1.alpha << endl;
    cout << "SA_max = " << configuration1.SA_MAX << endl;

    for(int i = 0; i < 10; i++) {
        auto start = steady_clock::now();

        string solution = simulated_annealing(n, wmax, profits, weights, configuration1);
        cout << "================================================================" << endl;
        cout << "Solucao: " << solution << endl;

        auto end = steady_clock::now();

        auto duration = duration_cast<microseconds>(end - start);

        cout << "================================================================" << endl;

        int total_profit = calculate_profit(profits, solution);
        int weight = calculate_weight(weights, solution);
        cout << "Profit apos o Simulated Annealing: " << total_profit << endl;
        cout << "Peso apos o Simulated Annealing: " << weight << endl;

        cout << "Duracao: " << duration.count() << " microssegundos" << endl;
        cout << "================================================================" << endl;

        profits_results[i] = total_profit;
        durations[i] = duration.count();
    }

    int best_profit_result = *max_element(profits_results.begin(), profits_results.end());
    int worst_profit_result = *min_element(profits_results.begin(), profits_results.end());
    double mean_profits_results = accumulate(profits_results.begin(), profits_results.end(), 0.0) / 10;

    cout << "Melhor profit: " << best_profit_result << endl;
    cout << "Pior profit: " << worst_profit_result << endl;
    cout << "Media dos profits: " << mean_profits_results << endl;

    cout << "Configuracao 2:" << endl;
    cout << "t0 = " << configuration2.t0 << endl;
    cout << "beta = " << configuration2.beta << endl;
    cout << "SA_max = " << configuration2.SA_MAX << endl;

    for(int i = 0; i < 10; i++) {
        auto start = steady_clock::now();

        string solution = simulated_annealing(n, wmax, profits, weights, configuration2);
        cout << "================================================================" << endl;
        cout << "Solucao: " << solution << endl;

        auto end = steady_clock::now();

        auto duration = duration_cast<microseconds>(end - start);

        cout << "================================================================" << endl;

        int total_profit = calculate_profit(profits, solution);
        int weight = calculate_weight(weights, solution);
        cout << "Profit apos o Simulated Annealing: " << total_profit << endl;
        cout << "Peso apos o Simulated Annealing: " << weight << endl;

        cout << "Duracao: " << duration.count() << " microssegundos" << endl;
        cout << "================================================================" << endl;

        profits_results[i] = total_profit;
        durations[i] = duration.count();
    }

    best_profit_result = *max_element(profits_results.begin(), profits_results.end());
    worst_profit_result = *min_element(profits_results.begin(), profits_results.end());
    mean_profits_results = accumulate(profits_results.begin(), profits_results.end(), 0.0) / 10;

    cout << "Melhor profit: " << best_profit_result << endl;
    cout << "Pior profit: " << worst_profit_result << endl;
    cout << "Media dos profits: " << mean_profits_results << endl;

    file.close();

    return 0;
}
