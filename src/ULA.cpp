#include "../include/ULA.h"
#include <iostream>
using namespace std;

// Convertendo o texto do arquivo extraído para inteiro (0 - 1)
int charParaInt(char c) {
    return (c == '1') ? 1 : 0;
}

// Extrai os sinais de controle a partir da instrução (6 bits: f0 f1 ena enb inva inc)
SinaisdeControle extrairInstrucao(string inst) {
    SinaisdeControle sinais;
    sinais.INC = charParaInt(inst[5]);
    sinais.INVA = charParaInt(inst[4]);
    sinais.ENB = charParaInt(inst[3]);
    sinais.ENA = charParaInt(inst[2]);
    sinais.F1 = charParaInt(inst[1]);
    sinais.F0 = charParaInt(inst[0]);
    return sinais;
}

ResultadoULA execULA(SinaisdeControle sinais, int A, int B) {
    ResultadoULA r{0,0};

    int entrada_A = A;
    if (sinais.ENA == 0){
        entrada_A = 0; 
    }
    
    int entrada_B = B;
    if (sinais.ENB == 0){
        entrada_B = 0; 
    }
    
    // Aplica INVA 
    if (sinais.INVA == 1) {
        entrada_A = ~entrada_A;
    }
    
    // Configura vai_um para INC 
    int co = 0;
    if (sinais.INC == 1) {
        co = 1;
    }
    // Resultado base dependendo da combinação f0 f1
    if (sinais.F0 == 0 && sinais.F1 == 0) {
        r.Saida = entrada_A & entrada_B;
        r.Carry = 0;
    } else if (sinais.F0 == 0 && sinais.F1 == 1) {
        r.Saida = entrada_A | entrada_B;
        r.Carry = 0;
    } else if (sinais.F0 == 1 && sinais.F1 == 0) {
        r.Saida = entrada_A ^ entrada_B;
        r.Carry = 0;
    } else if (sinais.F0 == 1 && sinais.F1 == 1) {
        // soma a, b e o incremento como unsigned para detectar carry
        unsigned int soma_temp = static_cast<unsigned int>(entrada_A)
                    + static_cast<unsigned int>(entrada_B) + co;

        r.Saida = static_cast<int>(soma_temp);

        // a soma ultrapassou o limite de 32 bits, o resultado fica menor que 1 dos operandos
        // carry é setado
        r.Carry = (soma_temp < static_cast<unsigned int>(entrada_A));
    }

    return r;
}
