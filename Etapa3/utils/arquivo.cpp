#include "arquivo.h"

string paraBinario32bits(int valor) {
    // Usa bitset para converter para binário e depois para string
    bitset<32> bits(valor);
    return bits.to_string();
}

vector <string> lerArquivo(const string &nomeArquivo, const string &nomeArquivoSaida){
    vector <string> instrucoes;
    ifstream arquivo(nomeArquivo);
    ofstream arquivoSaida(nomeArquivoSaida);
    string linha;

    if (!arquivo.is_open()) {
        cerr << "Não foi possível abrir o arquivo '" << nomeArquivo << "'" << endl;
        return instrucoes;
    }

    int numerodaLinha = 1;
    while (getline(arquivo, linha)) {
        size_t inicio = linha.find_first_not_of(" \t");
        size_t fim = linha.find_last_not_of(" \t");
        
        if (inicio != string::npos && fim != string::npos) {
            linha = linha.substr(inicio, fim - inicio + 1);
        }

        if (!linha.empty() && linha.back() == '\r') {
            linha.pop_back();
        }        

        if (linha.length() == 21) {  // Mudança de 8 para 21 bits
            bool continuar = true;
            for (char c : linha) {
                if (c != '0' && c != '1') {
                    continuar = false;
                    break;
                }
            }

            if (continuar) {
                instrucoes.push_back(linha);
            } else {
                cerr << "Linha " << numerodaLinha << " contém caracteres inválidos: " << linha << endl;
                arquivoSaida << "Linha " << numerodaLinha << " contém caracteres inválidos: " << linha << endl;
            }
        } else if (!linha.empty()) {
            cerr << "Linha " << numerodaLinha << " ignorada por (tamanho incorreto: " << linha.length() << "): " << linha << endl;
            arquivoSaida << "Linha " << numerodaLinha << " ignorada por (tamanho incorreto: " << linha.length() << "): " << linha << endl;
        }
        
        numerodaLinha++;
    }
    
    arquivo.close();
    arquivoSaida.close();
    return instrucoes;
}

void writeLineInFile(string nomeArquivo, string linha) {
    ofstream arquivo(nomeArquivo, ios::app); // Abre para adicionar no final
    if (!arquivo.is_open()) {
        cerr << "Não foi possível abrir o arquivo '" << nomeArquivo << "'" << endl;
        return;
    }
    arquivo << linha << endl;
    arquivo.close();
}

vector<string> lerLinhasDeArquivo(const string& nomeArquivo) {
    vector<string> linhas;
    ifstream arquivo(nomeArquivo);
    string linha;

    if (!arquivo.is_open()) {
        cerr << "Não foi possível abrir o arquivo '" << nomeArquivo << "'" << endl;
        return linhas; // Retorna um vetor vazio
    }

    // Lê cada linha do arquivo
    while (getline(arquivo, linha)) {
        // Opcional: Remove espaços em branco no início e no fim
        size_t inicio = linha.find_first_not_of(" \t\r\n");
        size_t fim = linha.find_last_not_of(" \t\r\n");
        if (inicio != string::npos) {
            linhas.push_back(linha.substr(inicio, fim - inicio + 1));
        }
    }
    
    arquivo.close();
    return linhas;
}