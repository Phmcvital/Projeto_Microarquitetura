#include "Memoria.h"
#include <stdexcept>

Memoria::Memoria(const std::string& arquivo_dados) {
    std::ifstream file(arquivo_dados);
    if (!file.is_open()) {
        throw std::runtime_error("Não foi possível abrir arquivo de memória");
    }
    
    std::string linha;
    while (file >> linha) {
        if (linha.length() == 32) {
            this->dados.push_back(std::stoll(linha, nullptr, 2));
        }
    }
    file.close();
}

uint32_t Memoria::ler(uint32_t endereco) {
    if (endereco < this->dados.size()) {
        return this->dados[endereco];
    }
    throw std::out_of_range("Endereço de memória inválido");
}

void Memoria::escrever(uint32_t endereco, uint32_t valor) {
    if (endereco < this->dados.size()) {
        this->dados[endereco] = valor;
    } else {
        throw std::out_of_range("Endereço de memória inválido");
    }
}

void Memoria::log(std::ofstream& log_file) {
    log_file << "--- Estado da Memória de Dados ---\n";
    for (size_t i = 0; i < this->dados.size(); ++i) {
        log_file << "Endereço " << i << ": " << this->dados[i] << "\n";
    }
    log_file << "----------------------------------\n\n";
}