#include <iostream>
#include <fstream>
#include <vector>
#include <bitset>
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
    unsigned H, OPC, TOS, CPP, LV, SP, PC, MDR, MAR;  // 32 bits
    unsigned int MBR;  // 8 bits
};

// Sinais de controle completos (21 bits)
struct SinaisCompletos {
    SinaisdeControle ULA;        // 8 bits
    int barramentoC;             // 9 bits 
    int barramentoB;             // 4 bits
    bool read; // 1 bit
    bool write; // 1 bit
};

// Estado da ULA e operandos
struct EstadoULA {
    int A, B;
    int S, Carry;
    int N, Z;  // Novos sinais N e Z
    int regPC;
    string regIR;
    Registradores regs;
    string registradorB;
    string registradoresC;
};

// Funções existentes
int charParaInt(char c);
void execTask(const string input, const string output);
void saveLog(vector<EstadoULA> log, string nomeArquivo);
EstadoULA controlOperation(const SinaisdeControle control, EstadoULA& ULAState);

// Novas funções para Etapa 2 Tarefa 2
vector<SinaisCompletos> readSinaisCompletos(const string input, const string output);
vector<SinaisCompletos> extractInstructionCompleta(vector<string> inst);
int decodificadorBarramentoB(int codigo, const Registradores& regs);
string getRegistradorBNome(int codigo);
vector<int> seletorBarramentoC(int codigo);
vector<string> getRegistradoresCNomes(int codigo);
void atualizarRegistradores(Registradores& regs, int saida, const vector<int>& habilitados);
int signExtend8to32(int valor8bits);