#include "arquivo.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset> 
#include <string> 

using namespace std;

namespace FormatacaoLog {
    const string LINHA = string(120, '=');
    const string DIVISORIA = string(120, '-');
    
    void escreverCabecalho(ofstream& arquivo) {
        arquivo << LINHA << endl;
        arquivo << "LOG DE EXECUCAO - ULA - MIC-1" << endl;
        arquivo << LINHA << endl;
        arquivo << setw(6) << "Ciclo" << " " << setw(6) << "PC" << " " 
                << setw(8) << "IR" << " " << setw(34) << "A" << " "
                << setw(34) << "B" << " " << setw(34) << "S" << " "
                << setw(6) << "Carry" << endl;
        arquivo << DIVISORIA << endl;
    }

    void escreverRodape(ofstream& arquivo, size_t totalInstrucoes) {
        arquivo << LINHA << endl;
        arquivo << "Total de instrucoes executadas: " << totalInstrucoes << endl;
        arquivo << LINHA << endl;
        arquivo << "Ciclo " << totalInstrucoes + 1 << endl;
        arquivo << "PC = " << totalInstrucoes << endl;
        arquivo << "> Line is empty, EOP" << endl;
    }
}
// Função para converter decimal para string binária de 6 bits
string paraBinario32bits(int valor) {
    // Usa bitset para converter para binário e depois para string
    bitset<32> bits(valor);
    return bits.to_string();
}


vector <string> lerArquivo(const string &nomeArquivo){
    vector <string> instrucoes;
    ifstream arquivo(nomeArquivo);
    string linha;

    if (!arquivo.is_open()) {
        cerr << "Não foi possível abrir o arquivo '" << nomeArquivo << "'" << endl;
        return instrucoes;
    }

    int numerodaLinha = 1;
    while (getline(arquivo, linha)) {
        // Remove espaços do inicio e fim
        size_t inicio = linha.find_first_not_of(" \t");
        size_t fim = linha.find_last_not_of(" \t");
        
        if (inicio != string::npos && fim != string::npos) {
            linha = linha.substr(inicio, fim - inicio + 1);
        }
        
        // Garantir que a linha tenha apenas 6 caracteres e só contém 0s e 1s
        if (linha.length() == 6) {
            bool continuar = true;
            for (char c : linha) {
                if (c != '0' && c != '1') {
                    continuar = false;
                    break;
                }
            }
            
            //Se a condicao for obedecida, guardar as instrucoes
            if (continuar) {
                instrucoes.push_back(linha);
            } else { // se não, mostrar em qeu linha esta o erro
                cerr << "Linha " << numerodaLinha << " contém caracteres inválidos: " << linha << endl;
            }
        } else if (!linha.empty()) {
            cerr << "Linha " << numerodaLinha << " ignorada por (tamanho incorreto): " << linha << endl;
        }
        
        numerodaLinha++; // passar para a linha debaixo até chegar ao fim
    }
    
    arquivo.close();
    return instrucoes;
}

// Salva o log de execução em um arquivo texto
void salvarLog(const vector<EstadoULA>& log, const string& nomeArquivo) {
    ofstream arquivo(nomeArquivo);
    string linha, linha2;
    
    if (!arquivo.is_open()) {
        cerr << "Não foi possível criar o arquivo de log '" << nomeArquivo << "'" << endl;
        return;
    }

    for (int i = 0; i < 132; i++) {
        linha += '=';
        linha2 += "-";
    }

    arquivo << "Início do programa" << endl;
    // Cabeçalho do arquivo de log
    arquivo << linha << endl;
    arquivo << "Ciclo\tPC\t  IR\t\t\t\t   A\t\t\t\t\t\t\t\t   B\t\t\t\t\t\t\t\t\tS  \t\t\t\t  Carry" << endl;
    arquivo << linha << endl;
    int ciclos = 0;
    // Dados de cada instrução executada
    for (const auto& estado : log) {
        ciclos++;
        arquivo << estado.regPC + 1 << "\t\t" << estado.regPC + 1 << "\t"
                << estado.regIR << "\t"
                << paraBinario32bits(estado.A) << "\t"
                << paraBinario32bits(estado.B) << "\t"
                << paraBinario32bits(estado.S) << "\t"
                << estado.Carry << endl;
    }

    arquivo << linha << endl;
    arquivo << "Ciclo " << ciclos + 1<< endl;
    arquivo << "PC " << ciclos + 1 << endl;
    arquivo << "Line is empty, EOP" << endl; 

    arquivo.close();
    cout << "Log salvo em: " << nomeArquivo << endl;
}