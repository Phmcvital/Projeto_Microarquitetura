#include <string>

using namespace std;

// Sinais de controle da ULA
struct SinaisdeControle{
    int F0, F1;
    int ENA, ENB;
    int INVA, INC;
};

// Resultado da ULA
struct ResultadoULA {
    int Saida, Carry;
};

// Estado da ULA e operandos
struct EstadoULA {
    int A, B;
    int S, Carry;
    int regPC;
    string regIR;
};

// Funções
SinaisdeControle extrairInstrucao(string inst);
ResultadoULA execULA(SinaisdeControle sinais, int A, int B);