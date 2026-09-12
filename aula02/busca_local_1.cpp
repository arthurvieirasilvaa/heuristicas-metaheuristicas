#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <random>
#include <chrono>

using namespace std;

constexpr int MAX_ITERATIONS = 1000; // número máximo de iterações

string generate_initial_solution(int n, int wmax, const vector<int> &weights) {
    string initial_solution = "";

    /*
        Gerador não-determinístico para inicializar o Mersenne Twister. Se
        random_device for realmente aleatória, a seed será realmente aleatória.
        Se random_device sempre retornar o mesmo valor, utilizar o operador XOR
        adiciona uma camada de variação:
    */
    random_device rd;
    auto seed = rd() ^ chrono::system_clock::now().time_since_epoch().count();
    static mt19937 gen(seed);

    // Distribuição uniforme entre 0 e 1 (inclusive):
    uniform_int_distribution<int> dist(0, 1);

    int current_weight = 0;
    for(int i = 0; i < n; i++) {
        if(dist(gen) == 0) {
            initial_solution += '0';
        }

        else {
            /*
                Verifica se a adição do item não ultrapassará a capacidade da
                mochila:
            */
            if(current_weight + weights[i] <= wmax) {
                initial_solution += '1';
                current_weight += weights[i];
            }

            else {
                initial_solution += '0';
            }
        }
    }

    return initial_solution;
}

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

void knapsack(int n, int wmax, const vector<int> &profits, const vector<int> &weights) {
    // Gerando a solução inicial aleatória (melhor solução atual):
    string current_solution = generate_initial_solution(n, wmax, weights);
    cout << "Solucao inicial: " << current_solution << endl;

    int current_profit = calculate_profit(profits, current_solution);
    int current_weight = calculate_weight(weights, current_solution);

    cout << "Profit da solucao inicial: " << current_profit << endl;
    cout << "Peso da solucao inicial: " << current_weight << endl;

    cout << "--------------------------------" << endl;

    bool solution_improved = true;
    int current_iteration = 0;
    string neighbor;
    int new_profit, new_weight;
    while (current_iteration < MAX_ITERATIONS and solution_improved) {
        solution_improved = false;

        // Gerando a vizinhança da solução atual:
        for(int i = 0; i < n; i++) {
            neighbor = current_solution;

            // Operador de vizinhança (flip):
            if(current_solution[i] == '0') {
                neighbor[i] = '1';
            }
            
            else {
                neighbor[i] = '0';
            }

            new_profit = calculate_profit(profits, neighbor);
            new_weight = calculate_weight(weights, neighbor);

            cout << "Vizinho " << i << ": " << neighbor << endl;
            cout << "Profit do vizinho " << i << ": " << new_profit << endl;
            cout << "Peso do vizinho " << i << ": " << new_weight << endl;
            cout << "--------------------------------" << endl;
            
            // Política de melhoria (First Improvement):
            if((new_profit > current_profit) && new_weight <= wmax) {
                current_solution = neighbor;
                current_profit = new_profit;
                current_weight = new_weight;
                solution_improved = true;
                break;
            }
        }

        current_iteration++;
    }
    
    cout << "Solucao: " << current_solution << endl;
    cout << "Profit: " << current_profit << endl;
    cout << "--------------------------------" << endl;
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./busca_local_1 <nome_do_arquivo_de_entrada.txt>" << endl;
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
        cout << "A linha do arquivo de entrada está corrompida!" << endl;
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

    knapsack(n, wmax, profits, weights);
    
    return 0;
}