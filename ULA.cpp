#include "ULA.h"
#include <iostream>
using namespace std;

// Convertendo o texto do arquivo extraído para inteiro (0 - 1)
int charParaInt(char c) {
    return (c == '1') ? 1 : 0;
}

// Extrai os sinais de controle a partir da instrução (6 bits: na ordem -> inc inva enb ena f1 f0)
SinaisdeControle extrairInstrucao(string inst){
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
ResultadoULA execULA(SinaisdeControle sinais, int A, int B){
    ResultadoULA r{0,0};

    // Normaliza entradas (0..255) dependendo do enable, mantendo os 8 bits significativos
    int entrada_A = sinais.ENA ? (A & 0xFF) : 0;
    int entrada_B = sinais.ENB ? (B & 0xFF) : 0;

    // Aplica INVA caso esteja ativo
    if (sinais.INVA) entrada_A = (~entrada_A) & 0xFF;

    // Resultado base dependendo da combinação f0 f1
    int resultado_base = 0;
    if (sinais.F0 == 0 && sinais.F1 == 0) {
        resultado_base = entrada_A & entrada_B;
    } else if (sinais.F0 == 0 && sinais.F1 == 1) {
        resultado_base = entrada_A | entrada_B;
    } else if (sinais.F0 == 1 && sinais.F1 == 0) {
        resultado_base = entrada_A ^ entrada_B;
    } else if (sinais.F0 == 1 && sinais.F1 == 1) {
        resultado_base = entrada_A + entrada_B;
    }

    // Aplica incremento no resultado caso esteja ativo
    int resultado_final = resultado_base + (sinais.INC ? 1 : 0);

    // Se o resultado final estourar 255 bits, carry ativou
    r.Carry = (resultado_final > 255) ? 1 : 0;

    // Saída 8 bits ( & 0xff mantem apenas os 8 bits signicativos do resultado final)
    r.Saida = resultado_final & 0xFF;

    return r;
}
