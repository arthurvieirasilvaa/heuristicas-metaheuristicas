#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cfloat>
#include <random>
#include <chrono>
#include <algorithm>
#include <climits>

using namespace std;
using namespace chrono;

// Constantes utilizadas
constexpr double t0 = 1000.0;
constexpr double alpha = 0.99;
constexpr int SA_MAX = 1000;

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

    for(int i = 0; i < n; i++) {
        cout << indexes[i] << " ";
    }

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

string simulated_annealing(int n, int wmax, const vector<int> &profits, const vector<int> &weights) {
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
    string s = generate_initial_solution(n, wmax, weights, gen); // solução inicial aleatória
    cout << "Solucao inicial aleatoria s0: " << s << endl;
    
    int current_profit = calculate_profit(profits, s);
    int current_weight = calculate_weight(weights, s);

    cout << "Profit da solucao inicial aleatoria s0: " << current_profit << endl;
    cout << "Peso da solucao inicial aleatoria s0: " << current_weight << endl;

    string s_best = s;
    int s_best_total_profit = current_profit;
    int i = 0;

    string neighbor;
    int neighbor_total_profit, neighbor_weight;

    while (T > 0.0) {
        while (i < SA_MAX) {
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

    cout << "t0 = " << t0 << endl;
    cout << "alfa = " << alpha << endl;
    cout << "SA_MAX = " << SA_MAX << endl;

    auto start = steady_clock::now();

    string solution = simulated_annealing(n, wmax, profits, weights);
    
    auto end = steady_clock::now();

    auto duration = duration_cast<microseconds>(end - start);

    cout << "================================================================" << endl;
    cout << "Solucao: " << solution << endl;

    int total_profit = calculate_profit(profits, solution);
    int weight = calculate_weight(weights, solution);
    cout << "Profit apos o Simulated Annealing: " << total_profit << endl;
    cout << "Peso apos o Simulated Annealing: " << weight << endl;

    cout << "Duracao: " << duration.count() << " microssegundos" << endl;

    file.close();

    return 0;
}