#include <iostream>
#include <fstream>

using namespace std;

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./busca_exaustiva2 <nome_do_arquivo_de_entrada.txt>" << endl;
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

    file.close();

    return 0;
}
