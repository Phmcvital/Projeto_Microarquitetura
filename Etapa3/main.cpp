#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>

#include "core/ULA.h" 
#include "core/Memoria.h"

using namespace std;

Registradores carregarRegistradores(const string& nomeArquivo);

void processarArquivoIJVM(const string& arq_instrucoes, const string& arq_saida, Registradores& regs, Memoria& mem);

// --- Implementação de carregarRegistradores ---
Registradores carregarRegistradores(const string& nomeArquivo) {
    Registradores regs = {0}; // Inicializa todos com 0
    ifstream arquivo(nomeArquivo);
    string linha;

    if (!arquivo.is_open()) {
        cerr << "Erro ao abrir o arquivo de registradores: " << nomeArquivo << endl;
        return regs;
    }

    while (getline(arquivo, linha)) {
        stringstream ss(linha);
        string nomeReg, separador, valorBin;

        if (ss >> nomeReg >> separador >> valorBin) {
            
            if (nomeReg == "mar") {
                regs.MAR = stoul(valorBin, nullptr, 2);
            } else if (nomeReg == "mdr") {
                regs.MDR = stoul(valorBin, nullptr, 2);
            } else if (nomeReg == "pc") {
                regs.PC = stoul(valorBin, nullptr, 2);
            } else if (nomeReg == "mbr") {
                // MBR é de 8 bits
                regs.MBR = static_cast<uint8_t>(stoul(valorBin, nullptr, 2));
            } else if (nomeReg == "sp") {
                regs.SP = stoul(valorBin, nullptr, 2);
            } else if (nomeReg == "lv") {
                regs.LV = stoul(valorBin, nullptr, 2);
            } else if (nomeReg == "cpp") {
                regs.CPP = stoul(valorBin, nullptr, 2);
            } else if (nomeReg == "tos") {
                regs.TOS = stoul(valorBin, nullptr, 2);
            } else if (nomeReg == "opc") {
                regs.OPC = stoul(valorBin, nullptr, 2);
            } else if (nomeReg == "h") {
                regs.H = stoul(valorBin, nullptr, 2);
            }
        }
    }
    
    arquivo.close();
    return regs;
}


int main() {

    string arquivo_instrucoes    = "instrucoes.txt"; 
    string arquivo_regs_inicial  = "registradores_etapa3.txt";
    string arquivo_mem_inicial   = "dados_etapa3.txt";
    string arquivo_log_saida     = "saida_final.txt";
    
    cout << "Iniciando simulador da Mic-1..." << endl;


    Registradores regs = carregarRegistradores(arquivo_regs_inicial);
    Memoria mem(arquivo_mem_inicial);


    processarArquivoIJVM(arquivo_instrucoes, arquivo_log_saida, regs, mem);
    
    return 0;
}