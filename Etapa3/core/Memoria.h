#ifndef MEMORIA_H
#define MEMORIA_H

#include <vector>
#include <string>
#include <fstream>
#include <cstdint>

class Memoria {

    private:
        std::vector<uint32_t> dados; // Memória de dados simulada por um vetor de inteiros

    public:
        Memoria(const std::string& arquivo_dados);

        uint32_t ler(uint32_t endereco);
        void escrever(uint32_t endereco, uint32_t valor);

        void log(std::ofstream& log_file);

};

#endif