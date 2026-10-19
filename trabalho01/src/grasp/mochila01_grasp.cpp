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

constexpr int MAX_ITERATIONS = 1000;
constexpr int GRASPMax = 100;

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

string local_search(int n, int wmax, const vector<int> &profits, const vector<int> &weights, string s) {
    int current_profit = calculate_profit(profits, s);
    int current_weight = calculate_weight(weights, s);

    bool solution_improved = true;
    int current_iteration = 0;
    string neighbor;
    int new_profit, new_weight;
    while (current_iteration < MAX_ITERATIONS && solution_improved) {
        solution_improved = false;

        // Gerando a vizinhança da solução atual:
        for(int i = 0; i < n; i++) {
            neighbor = s;

            // Operador de vizinhança (flip):
            if(s[i] == '0') {
                neighbor[i] = '1';
            }
            
            else {
                neighbor[i] = '0';
            }

            new_profit = calculate_profit(profits, neighbor);
            new_weight = calculate_weight(weights, neighbor);
            
            // Política de melhoria (First Improvement):
            if((new_profit > current_profit) && new_weight <= wmax) {
                s = neighbor;
                current_profit = new_profit;
                current_weight = new_weight;
                solution_improved = true;
                break;
            }
        }

        current_iteration++;
    }

    return s;
}

void create_gc(const vector<int> &profits, vector<int> LC, vector<int> &gc) {
    gc.clear();

    // Profit do candidato c da LC se for adicionado na mochila: 
    for(int i = 0; i < LC.size(); i++) {
        gc.push_back(profits[LC[i]]);
    }
}

void create_LRC(const vector<int> &profits, vector<int> LC, vector<int> &LRC, double alpha) {
    vector<int> gc;
    create_gc(profits, LC, gc);

    int cmin = *min_element(gc.begin(), gc.end());
    int cmax = *max_element(gc.begin(), gc.end());

    LRC.clear();    

    double g = cmax - (alpha * (cmax - cmin));

    for(int i = 0; i < gc.size(); i++) {
        if(gc[i] >= g) {
            LRC.push_back(LC[i]);
        }
    }
}

string random_greedy_construction(int n, int wmax, const vector<int> &profits, const vector<int> &weights, const double alpha, mt19937 &gen) {
    string s(n, '0'); // solução inicial vazia
    
    // Cria LC:
    vector<int> LC, feasible_LC, LRC;
    for(int i = 0; i < n; i++) {
        LC.push_back(i);
    }

    int x;
    int current_weight = 0;
    while (LC.size() > 0) {
        feasible_LC.clear();

        // Cria LC viável apenas com os itens que cabem na mochila:
        for(int i = 0; i < LC.size(); i++) {
            if(current_weight + weights[LC[i]] <= wmax) {
                feasible_LC.push_back(LC[i]);
            }
        }

        // Verifica se mais nenhum item cabe na mochila:
        if(feasible_LC.size() == 0) {
            break;
        }

        // Cria LRC (Lista Restrita de Candidatos):
        create_LRC(profits, feasible_LC, LRC, alpha);
        uniform_int_distribution<size_t> dis(0, LRC.size()-1);
        size_t random_index = dis(gen);

        x = LRC[random_index]; // um elemento aleatório da LRC
        s[x] = '1'; // adiciona o item na mochila
        current_weight += weights[x]; // atualiza o peso atual

        auto iterator = find(LC.begin(), LC.end(), x);
        LC.erase(iterator); // atualiza LC (remove x, atualiza valores)
    }
    
    return s;
}
 
string grasp(int n, int wmax, const vector<int> &profits, const vector<int> &weights, mt19937 &gen) {
    string s, sbest;
    const double alpha = 0.7;
    int current_profit;
    int best_total_profit = INT_MIN;
    int iter = 0;

    while (iter < GRASPMax) {
        s = random_greedy_construction(n, wmax, profits, weights, alpha, gen);
        s = local_search(n, wmax, profits, weights, s);

        current_profit = calculate_profit(profits, s);

        if(current_profit > best_total_profit) {
            sbest = s;
            best_total_profit = current_profit;
        }

        iter++;
    }
    
    return sbest;
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./mochila01_grasp <nome_do_arquivo_de_entrada>" << endl;
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

    /*
        Gerador não-determinístico para inicializar o Mersenne Twister. Se
        random_device for realmente aleatória, a seed será realmente aleatória.
        Se random_device sempre retornar o mesmo valor, utilizar o operador XOR
        adiciona uma camada de variação:
    */
    random_device rd;
    auto seed = rd() ^ system_clock::now().time_since_epoch().count();
    mt19937 gen(seed);

    auto start = steady_clock::now();

    string s = grasp(n, wmax, profits, weights, gen);

    auto end = steady_clock::now();

    auto duration = duration_cast<microseconds>(end - start);

    int total_profit = calculate_profit(profits, s);

    cout << "Solucao apos o GRASP: " << s << endl;
    cout << "Profit apos o GRASP: " << total_profit << endl;

    cout << "Duracao: " << duration.count() << " microssegundos" << endl;

    file.close();

    return 0;
}