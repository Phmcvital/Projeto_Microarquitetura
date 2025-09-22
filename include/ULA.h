#include <string>

using namespace std;

struct SinaisdeControle{
    int F0, F1;
    int ENA, ENB;
    int INVA, INC;
};

struct ResultadoULA {
    int Saida, Carry;
};

struct EstadoULA {
    int A, B;
    int S, Carry;
    int regPC;
    string regIR;
};

SinaisdeControle extrairInstrucao(string inst);
ResultadoULA execULA(SinaisdeControle sinais, int A, int B);