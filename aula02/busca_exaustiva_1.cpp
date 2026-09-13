#include <iostream>
#include <fstream>
#include <chrono>

using namespace std;
using namespace chrono;

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./busca_exaustiva_1 <nome_do_arquivo_de_entrada.txt>" << endl;
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
    long long total = 1LL << n;
    cout << total << endl;

    auto start = steady_clock::now();

    int bit;
    for(long long i = 0; i < total; i++) {
        for(long long j = n-1; j >= 0; j--) {
            bit = (i >> j) & 1LL;
            cout << bit;
        }
        cout << endl;
    }

    auto end = steady_clock::now();

    auto duration = duration_cast<microseconds>(end - start);
    cout << "Duracao: " << duration.count() << " microssegundos" << endl;

    file.close();

    return 0;
}
