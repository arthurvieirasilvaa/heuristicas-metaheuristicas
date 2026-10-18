#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cfloat>
#include <chrono>
#include <algorithm>
#include <climits>

using namespace std;
using namespace chrono;

constexpr int MAX_ITERATIONS_WITHOUT_IMPROVEMENT = 100; // número máximo de iterações sem melhora
constexpr int TABU_TENURE = 5; // Duração Tabu

string greedy_by_weight_to_value_ratio(int n, int wmax, const vector<int> &weights, const vector<int> &profits) {
    string solution(n, '0'); // nenhum item na mochila

    // Custo-benefício (peso/valor) de cada item da mochila:
    vector<double> weight_to_value;

    for(int i = 0; i < n; i++) {
        // Verifica se o profit atual é 0:
        if(profits[i] == 0) {
            weight_to_value.push_back(DBL_MAX);
        }

        else {
            weight_to_value.push_back((double) weights[i]/profits[i]);
        }
    }

    vector<bool> backpack_items(n, false);

    int current_weight = 0;
    double best_weight_to_value;
    int item_index;
    while (current_weight < wmax) {
        best_weight_to_value = DBL_MAX;
        item_index = -1;

        for(int i = 0; i < n; i++) {
            if(weight_to_value[i] < best_weight_to_value && current_weight + weights[i] <= wmax && backpack_items[i] == false) {
                best_weight_to_value = weight_to_value[i];
                item_index = i;
            }
        }

        // Não encontrou nenhum item que caiba:
        if(item_index == -1) {
            break;
        }

        solution[item_index] = '1';
        backpack_items[item_index] = true; // marca o item como adicionado
        current_weight += weights[item_index];
    }

    return solution;
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

long long calculate_objective(int n, int wmax, const vector<int> &profits, const vector<int> &weights, string current_solution, long long p, int weight) {
    long long profit = calculate_profit(profits, current_solution);

    // Função objetivo com penalidade:
    return profit - p * max(0LL, (long long) weight - wmax);
}

string generate_neighbor(string s, int index) {
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

int select_oldest_tabu_move(int n, int wmax, string &s, const vector<int> &profits, const vector<int> &weights, 
    const vector<int> &T, string &best_neighbor, long long &best_neighbor_score, long long p) {
    int index_new_s = -1;
    int min_tabu = INT_MAX;
    string neighbor;
    int new_weight;
    for(int i = 0; i < n; i++) {
        neighbor = generate_neighbor(s, i);
        new_weight = calculate_weight(weights, neighbor);

        // O movimento tabu há mais tempo na lista é realizado:
        if(T[i] > 0 && T[i] < min_tabu) {
            min_tabu = T[i];
            index_new_s = i;

            best_neighbor = neighbor;
            best_neighbor_score = calculate_objective(n, wmax, profits, weights, best_neighbor, p, new_weight);
        }
    }
    
    return index_new_s;
}

string tabu_search(int n, int wmax, const vector<int> &profits, const vector<int> &weights, long long p) {
    // Gerando a solução gulosa (usando o melhor custo benefício - peso/valor):
    string s = greedy_by_weight_to_value_ratio(n, wmax, weights, profits); // solução inicial
    cout << "Solucao gulosa (usando o melhor custo beneficio - peso/valor): " << s << endl;

    int current_profit = calculate_profit(profits, s);
    cout << "Profit da solucao gulosa (usando o melhor custo beneficio - peso/valor): " << current_profit << endl;
    cout << "Peso da solucao gulosa (usando o melhor custo beneficio - peso/valor): " << calculate_weight(weights, s) << endl;
    cout << "--------------------------------" << endl;

    string sbest = s;
    int iter = 0;
    int iterations_without_improvement = 0;
    vector<int> T(n, 0);
  
    string neighbor, best_neighbor;

    int index_new_s;
    long long new_score;
    int new_weight;
    long long best_score = current_profit;
    long long best_neighbor_score;
    while (iterations_without_improvement < MAX_ITERATIONS_WITHOUT_IMPROVEMENT) {
        iter++;
        index_new_s = -1;
        best_neighbor = "";
        best_neighbor_score = LLONG_MIN;

        // Gerando a vizinhança da solução atual:
        for(int i = 0; i < n; i++) {
            neighbor = generate_neighbor(s, i);
            new_weight = calculate_weight(weights, neighbor);

            new_score = calculate_objective(n, wmax, profits, weights, neighbor, p, new_weight);
        
            /*
                Critério de Ativação por objetivo regional e verifica se o 
                movimento não é tabu:
            */  
            if(new_score > best_score || T[i] == 0) {
                if(new_score > best_neighbor_score) {
                    best_neighbor = neighbor;
                    best_neighbor_score = new_score;
                    index_new_s = i;
                }
            }
        }

        /*
            Se todos os movimentos estão proibidos, permite o movimento tabu há
            mais tempo na lista (Critério de Aspiração por default):
        */
        if(index_new_s == -1) {
            cout << "Criterio de Aspiracao por default" << endl;
            
            index_new_s = select_oldest_tabu_move(n, wmax, s, profits, weights, T, best_neighbor, best_neighbor_score, p);
            
            // Verifica se não há nenhum movimento viável:
            if(index_new_s == -1) {
                break;
            }
        }

        // Lista Tabu com Prazo (Decrescente):
        for(int i = 0; i < n; i++) {
            if(T[i] > 0) {
                T[i]--;
            }
        }

        s = best_neighbor;
        T[index_new_s] = TABU_TENURE; // Atualiza a lista tabu T

        if(best_neighbor_score > best_score) {
            sbest = s;
            best_score = best_neighbor_score;
            iterations_without_improvement = 0;
        }

        else {
            iterations_without_improvement++;
        }

        cout << "Iteracao: " << iter << endl <<
        "Movimento: " << index_new_s << endl <<
        "Profit atual: " << best_neighbor_score << endl <<
        "Melhor profit: " << best_score << endl <<
        "--------------------------------" <<
        endl;
    }

    return sbest;
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./mochila01_busca_tabu <nome_do_arquivo_de_entrada>" << endl;
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

    // Constante de penalidade:
    long long p = 0;
    for(int i = 0; i < profits.size(); i++) {
        p += profits[i];
    }

    auto start = steady_clock::now();

    string sbest = tabu_search(n, wmax, profits, weights, p);

    auto end = steady_clock::now();

    auto duration = duration_cast<microseconds>(end - start);

    int total_profit = calculate_profit(profits, sbest);

    cout << "Solucao apos a Busca Tabu: " << sbest << endl;
    cout << "Profit apos a Busca Tabu: " << total_profit << endl;

    cout << "Duracao: " << duration.count() << " microssegundos" << endl;

    file.close();

    return 0;
}