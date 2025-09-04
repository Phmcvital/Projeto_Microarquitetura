#include "arquivo.h"
#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset>  

using namespace std;

// Função para converter decimal para string binária de 8 bits
string paraBinario8bits(int valor) {
    // Usa bitset para converter para binário e depois para string
    bitset<8> bits(valor);
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
    
    if (!arquivo.is_open()) {
        cerr << "Não foi possível criar o arquivo de log '" << nomeArquivo << "'" << endl;
        return;
    }
    
    // Cabeçalho do arquivo de log
    arquivo << "==================================================" << endl;
    arquivo << "LOG DE EXECUCAO - ULA - MIC-1" << endl;
    arquivo << "==================================================" << endl;
    arquivo << "PC\t\tIR\t\t\tA\t\tB\t\tS\t\t\tCarry" << endl;
    arquivo << "--------------------------------------------------" << endl;
    
    // Dados de cada instrução executada
    for (const auto& estado : log) {
        arquivo << estado.regPC << "\t"
                << estado.regIR << "\t"
                << paraBinario8bits(estado.A) << "\t"
                << paraBinario8bits(estado.B) << "\t"
                << paraBinario8bits(estado.S) << "\t"
                << estado.Carry << endl;
    }
    
    arquivo << "==================================================" << endl;
    arquivo << "Total de instrucoes executadas: " << log.size() << endl;
    
    arquivo.close();
    cout << "Log salvo em: " << nomeArquivo << endl;
}