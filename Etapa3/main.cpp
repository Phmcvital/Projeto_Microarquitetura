#include "core/ULA.h"

using namespace std;

int main() {
    string instrucoes = "instrucoes.txt";  
    string output = "saida.txt";
    string memoria = "dados_etapa3_tarefa1.txt";         
    string registradores = "registradores_etapa3_tarefa1.txt";
    
    execTask(instrucoes, output, memoria, registradores);
    return 0;
}