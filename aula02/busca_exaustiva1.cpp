#include <iostream>
#include <fstream>
#include <cmath>
#include <bitset>

using namespace std;

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./busca_exaustiva1 <nome_do_arquivo_de_entrada.txt>" << endl;
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
    long total = 1L << n;
    cout << total << endl;

    int bit;
    for(long i = 0; i < total; i++) {
        for(long j = n-1; j >= 0; j--) {
            bit = (i >> j) & 1L;
            cout << bit;
        }
        cout << endl;
    }

    file.close();

    return 0;
}