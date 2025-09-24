#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
#include <string>
#include <array>
#include <map>
#include <bits/stdc++.h>
#include "../utils/arquivo.h"

using namespace std;

// Sinais de controle da ULA (8 bits)
struct SinaisdeControle{
    int F0, F1;
    int ENA, ENB;
    int INVA, INC;
    int SLL8, SRA1;
};

// Registradores da Mic-1
struct Registradores {
    int32_t H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR;
    uint8_t MBR;
};

// Sinais de controle completos (23 bits)
struct SinaisCompletos23 {
    array<int, 23> bits;
    
    // Getters para os sinais
    int SLL8() const { return bits[0]; }
    int SRA1() const { return bits[1]; }
    int F0() const { return bits[2]; }
    int F1() const { return bits[3]; }
    int ENA() const { return bits[4]; }
    int ENB() const { return bits[5]; }
    int INVA() const { return bits[6]; }
    int INC() const { return bits[7]; }
    int READ() const { return bits[18]; }
    int WRITE() const { return bits[17]; }
    
    uint8_t barramentoB() const {
        return (bits[19] << 3) | (bits[20] << 2) | (bits[21] << 1) | bits[22];
    }
    
    int barramentoC() const {
        int codigo = 0;
        for (int i = 0; i < 9; ++i) {
            codigo |= (bits[8 + i] << (8 - i));
        }
        return codigo;
    }
};

// Estado da ULA e operandos
struct EstadoULA {
    int32_t A, B;
    int32_t S;
    int Carry, N, Z;
    int regPC;
    string regIR;
    Registradores regs;
    string registradorB;
    string registradoresC;
};

// Funções principais
void execTask(const string input, const string output, 
              const string arquivoMemoria, const string arquivoRegistradores);
void saveLog(vector<EstadoULA> log, string nomeArquivo);
EstadoULA controlOperation(const SinaisdeControle control, EstadoULA& ULAState);

// Funções para microinstruções de 23 bits
vector<SinaisCompletos23> readSinais23Bits(const string input);
array<int, 23> stringParaMicroinstrucao(const string& s);
int32_t decodificadorBarramentoB(uint8_t codigo, const Registradores& regs);
string getRegistradorBNome(uint8_t codigo);
vector<int> seletorBarramentoC(int codigo);
string getRegistradoresCNomes(int codigo);
void atualizarRegistradores(Registradores& regs, int32_t saida, const vector<int>& habilitados);
void carregarRegistradores(const string& arquivo, Registradores& regs);
bool lerMemoria32Bin(const string& path, vector<int32_t>& MEM);
string conversor_binario(int32_t valor);

// Funções para execução completa
void executarMicroinstrucoes(vector<SinaisCompletos23>& microinstrucoes, 
                           Registradores& regs, vector<int32_t>& MEM, 
                           ofstream& saida, int& ciclo_global);