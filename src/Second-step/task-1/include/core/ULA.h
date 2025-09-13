#include <string>
#include <iostream>
#include <vector>
#include "../utils/arquivo.h"

using namespace std;

// Sinais de controle da ULA
struct SinaisdeControle{
    int F0, F1;
    int ENA, ENB;
    int INVA, INC;
    int SLL8, SRA1;
};

// Estado da ULA e operandos
struct EstadoULA {
    int A, B;
    int S, Carry;
    int regPC;
    string regIR;
};

// Funções
int charParaInt(char c);
void execTask(const string input, const string output);
EstadoULA controlOperation(const SinaisdeControle control);
vector<SinaisdeControle> readSinaisdeControle(const string input);
vector<SinaisdeControle> extractInstruction(vector<string> inst);