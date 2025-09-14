#include "include/core/ULA.h"

using namespace std;

int main() {
    int opcao;
    cout << "Escolha a tarefa a executar:" << endl;
    cout << "1 - Etapa 1" << endl;
    cout << "2 - Etapa 2 Tarefa 1" << endl;
    cout << "3 - Etapa 2 Tarefa 2" << endl;
    cout << "Opção: ";
    cin >> opcao;
    
    switch(opcao) {
        case 1: {
            string input = "inputs/etapa1.txt";
            string output = "outputs/output-etapa1.txt";
            execTask(input, output);
            break;
        }
        case 2: {
            string input = "inputs/step2-task1.txt";
            string output = "outputs/output-step2-task1.txt";
            execTask(input, output);
            break;
        }
        case 3: {
            string input = "step2-task2.txt";
            string output = "output-step2-task2.txt";
            execTaskEtapa2Tarefa2(input, output);
            break;
        }
        default:
            cout << "Opção inválida!" << endl;
            break;
    }
    
    return 0;
}
