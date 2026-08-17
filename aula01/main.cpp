#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

int main(int argc, char* argv[]) {
    if(argc < 2) {
        cout << "Argumentos invalidos!" << endl;
        cout << "Formato correto: ./main <nome_do_arquivo_de_entrada.txt>" << endl;
        exit(1);
    }
    
    ifstream file;

    file.open(argv[1]);

    // Verifica se o arquivo foi aberto:
    if(!file.is_open()) {
        cout << "Ocorreu um erro ao abrir o arquivo de entrada!" << endl;
        exit(1);
    }

    else {
        string line;
        getline(file, line); // lendo o número de vértices e de arestas do grafo

        stringstream ss(line);
        int nodes, edges;

        // Fazendo o parsing do stream de string:
        if(ss >> nodes >> edges) {
            cout << "Nos: " << nodes << endl;
            cout << "Arestas: " << edges << endl;
        }
        
        else {
            cout << "A linha do arquivo de entrada está corrompida!" << endl;
            exit(1);
        }
    }

    file.close();
    
    return 0;
}