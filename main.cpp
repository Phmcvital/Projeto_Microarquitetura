#include <iostream>
#include <vector>
#include "arquivo.h" 

using namespace std;

int main() {
    cout << "=== ETAPA 1 - ULA ===" << endl << endl;
    
    // a e b ativos como definido nos requisitos do projeto
    int A = -1;
    int B = 1;
    
    cout << "Lendo instrucoes..." << endl;
    vector<string> instrucoes = lerArquivo("programa_etapa1.txt");
    // guardar as entradas como instrucoes

    if (instrucoes.empty()) {
        cerr << "Nenhuma instrução encontrada ou arquivo inexistente." << endl;
        cerr << "Crie um arquivo 'programa_etapa1.txt' com instrucoes de 6 bits." << endl;
        return 1;
    }
    
    cout << "Encontradas " << instrucoes.size() << " instrucoes!" << endl;
    
    cout << "Executando instrucoes na ULA..." << endl;
    vector<EstadoULA> log;
    int PC = 0;
    // indicando qual instrucao vai ser executada na sequencia
    
    // percorre cada instrucao no vetor de instrucoes
    for (const auto& instrucao : instrucoes) {
        // Extrair sinais de controle da instrução
        SinaisdeControle sinais = extrairInstrucao(instrucao);
        
        // Executar operação na ULA
        ResultadoULA resultado = execULA(sinais, A, B);
        
        // Registrar estado atual para o log
        EstadoULA estado;
        estado.regIR = instrucao; // a palavra é armazenada no IR como definido no projeto
        estado.regPC = PC;
        estado.A = A;  
        estado.B = B;
        estado.S = resultado.Saida;
        estado.Carry = resultado.Carry;
        
        // adiciona log
        log.push_back(estado);
        PC++;
        // incrementa o PC passando para a próxima instrucao 
    }
    
    // 3. Salvar log de execução
    cout << "Gerando arquivo de log..." << endl;
    salvarLog(log, "saida_etapa1.txt");
    
    cout << endl << "Encerrando." << endl;
    
    return 0;
}
