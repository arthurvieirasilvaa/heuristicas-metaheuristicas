#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cfloat>
#include <climits>

using namespace std;

constexpr int MAX_ITERATIONS = 1000; // número máximo de iterações

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

string greedy_lowest_weight_items(int n, int wmax, const vector<int> &weights) {
    string solution(n, '0'); // nenhum item na mochila
    vector<bool> backpack_items(n, false);

    int current_weight = 0;
    int lowest_weight, item_index;
    while (current_weight < wmax) {
        lowest_weight = INT_MAX;
        item_index = -1;

        for(int i = 0; i < n; i++) {
            if(weights[i] < lowest_weight && current_weight + weights[i] <= wmax && backpack_items[i] == false) {
                lowest_weight = weights[i];
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

void knapsack(int n, int wmax, const vector<int> &profits, const vector<int> &weights, string &solution) {
    int current_profit = calculate_profit(profits, solution);
    int current_weight = calculate_weight(weights, solution);

    bool solution_improved = true;
    int current_iteration = 0;
    string neighbor;
    int new_profit, new_weight;
    while (current_iteration < MAX_ITERATIONS && solution_improved) {
        solution_improved = false;

        // Gerando a vizinhança da solução atual:
        for(int i = 0; i < n; i++) {
            neighbor = solution;

            // Operador de vizinhança (flip):
            if(solution[i] == '0') {
                neighbor[i] = '1';
            }
            
            else {
                neighbor[i] = '0';
            }

            new_profit = calculate_profit(profits, neighbor);
            new_weight = calculate_weight(weights, neighbor);
            
            // Política de melhoria (First Improvement):
            if((new_profit > current_profit) && new_weight <= wmax) {
                solution = neighbor;
                current_profit = new_profit;
                current_weight = new_weight;
                solution_improved = true;
                break;
            }
        }

        current_iteration++;
    }
}

int main(int argc, char *argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./mochila01_guloso <nome_do_arquivo_de_entrada.txt>" << endl;
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

    // Gerando a solução gulosa (usando o melhor custo benefício - peso/valor):
    string solution = greedy_by_weight_to_value_ratio(n, wmax, weights, profits);
    cout << "Solucao gulosa (usando o melhor custo beneficio - peso/valor): " << solution << endl;

    int greedy_profit = calculate_profit(profits, solution);
    int greedy_weight = calculate_weight(weights, solution);

    cout << "Profit da solucao gulosa (usando o melhor custo beneficio - peso/valor): " << greedy_profit << endl;
    cout << "Peso da solucao gulosa (usando o melhor custo beneficio - peso/valor): " << greedy_weight << endl;
    cout << "--------------------------------" << endl;

    /*
        Aplicando a Busca Local na solução gulosa (usando o melhor custo
        benefício - peso/valor):
    */
    knapsack(n, wmax, profits, weights, solution);
    int total_profit = calculate_profit(profits, solution);

    cout << "Solucao apos a Busca Local: " << solution << endl;
    cout << "Profit apos a Busca Local: " << total_profit << endl;
    cout << "--------------------------------" << endl;

    // Gerando a solução gulosa (usando os itens mais leves):
    solution = greedy_lowest_weight_items(n, wmax, weights);
    cout << "Solucao gulosa (usando os itens mais leves): " << solution << endl;

    greedy_profit = calculate_profit(profits, solution);
    greedy_weight = calculate_weight(weights, solution);

    cout << "Profit da solucao gulosa (usando os itens mais leves): " << greedy_profit << endl;
    cout << "Peso da solucao gulosa (usando os itens mais leves): " << greedy_weight << endl;
    cout << "--------------------------------" << endl;

    
    // Aplicando a Busca Local na solução gulosa (usando os itens mais leves):
    knapsack(n, wmax, profits, weights, solution);
    total_profit = calculate_profit(profits, solution);

    cout << "Solucao apos a Busca Local: " << solution << endl;
    cout << "Profit apos a Busca Local: " << total_profit << endl;
    cout << "--------------------------------" << endl;
    
    return 0;
}
