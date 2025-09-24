#include "Memoria.h"
// Construtor que inicializa a memória a partir de um arquivo
Memoria::Memoria(const std::string& arquivo_dados) {
    std::ifstream file(arquivo_dados);

    std::string linha_binaria;
    while (getline(file, linha_binaria)) {
        if (linha_binaria.length() == 32) {
            uint32_t valor = std::stoul(linha_binaria, nullptr, 2);
            this->dados.push_back(valor);
        }
    }
    file.close();
}


// Função de leitura

uint32_t Memoria::ler(uint32_t endereco) {
    if (endereco < this->dados.size()) {
        return this->dados[endereco];
    }
    throw std::out_of_range("Endereço de memória inválido");
    return 0;
}

// Função de escrita

void Memoria::escrever(uint32_t endereco, uint32_t valor) {
    if (endereco < this->dados.size()) {
        this->dados[endereco] = valor;
    } else {
        throw std::out_of_range("Endereço de memória inválido");
    }
}

//Função de log

void Memoria::log(std::ofstream& log_file) {
    log_file << "--- Estado da Memória de Dados ---\n";
    for (size_t i = 0; i < this->dados.size(); ++i) {
        log_file << "Endereço " << i << ": " << this->dados[i] << "\n";
    }
    log_file << "----------------------------------\n\n";
}