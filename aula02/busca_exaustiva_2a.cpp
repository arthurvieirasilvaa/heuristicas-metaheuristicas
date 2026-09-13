#include <iostream>
#include <fstream>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

void print_permutation(const vector<int> &permutation, int n) {
    for(int i = 0; i < n; i++) {
        cout << permutation[i] << " ";
    }
    cout << endl;
}

void generate_permutation(vector<int> &permutation, int begin, int n) {
    if(begin == n) {
        print_permutation(permutation, n);
        return;
    }
    
    int n1, n2;
    for(int i = begin; i < n; i++) { 
        // Trocando duas posições:
        n1 = permutation[i];
        n2 = permutation[begin];
        permutation[begin] = n1;
        permutation[i] = n2;
        
        generate_permutation(permutation, begin+1, n);

        // Trocando duas posições:
        n1 = permutation[i];
        n2 = permutation[begin];
        permutation[begin] = n1;
        permutation[i] = n2;
    }
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./busca_exaustiva_2a <nome_do_arquivo_de_entrada.txt>" << endl;
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
    getline(file, line);
    int n = stoi(line);
    cout << n << endl;

    vector<int> permutation;
    for(int i = 1; i <= n; i++) {
        permutation.push_back(i);
    }

    auto start = steady_clock::now();
    generate_permutation(permutation, 0, n);
    auto end = steady_clock::now();

    auto duration = duration_cast<microseconds>(end - start);
    cout << "Duracao: " << duration.count() << " microssegundos" << endl;

    file.close();

    return 0;
}