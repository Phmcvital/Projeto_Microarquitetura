#include "ULA.h"
#include <iostream>
using namespace std;

// Convertendo o texto do arquivo extraído para inteiro (0 - 1)
int charParaInt(char c) {
    return (c == '1') ? 1 : 0;
}

// Extrai os sinais de controle a partir da instrução (6 bits: inc inva enb ena f1 f0)
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

// Executa a ULA com os sinais de controle e entradas A e B
ResultadoULA execULA(SinaisdeControle sinais, int A, int B) {
    ResultadoULA r{0,0};

    // Normaliza entradas dependendo do enable
    int entrada_A = A;
    if (sinais.ENA == 0){
        entrada_A = 0; // disable A
    }
    
    // Selecionar valor para registrador B (igual ao primeiro código)
    int entrada_B = B;
    if (sinais.ENB == 0){
        entrada_B = 0; // disable B
    }
    
    // Aplica INVA 
    if (sinais.INVA == 1) {
        entrada_A = ~entrada_A;
    }
    
    // Configura vai_um para INC 
    int co = 0;
    if (sinais.INC == 1) {
        co= 1;
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
        /*signed long long soma = (unsigned long long)(unsigned int)entrada_A + 
                                 (unsigned long long)(unsigned int)entrada_B + 
                                 co;
        r.Saida = static_cast<int>(soma);*/
        unsigned int a_u = static_cast<unsigned int>(entrada_A);
        unsigned int b_u = static_cast<unsigned int>(entrada_B);
        unsigned int soma_u = a_u + b_u + co;
        r.Saida = static_cast<int>(soma_u);
        
        // Carry = 1 se houve overflow (soma > máximo unsigned de 32 bits)
        r.Carry = (soma_u < a_u || (soma_u == a_u && b_u + co> 0)) ? 1 : 0;
       //.Carry = (soma > 0xFFFFFFFF) ? 1 : 0;
    }

    return r;
}
