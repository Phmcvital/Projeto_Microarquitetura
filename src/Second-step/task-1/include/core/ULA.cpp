#include "ULA.h"

int charParaInt(char c) {
    return (c == '1') ? 1 : 0;
}

vector<SinaisdeControle> extractInstruction(vector<string> inst) {
    vector<SinaisdeControle>  inputData;

    for(auto var : inst)
    {
        SinaisdeControle data;
        data.INC = charParaInt(var[7]);
        data.INVA = charParaInt(var[6]);
        data.ENB = charParaInt(var[5]);
        data.ENA = charParaInt(var[4]);
        data.F1 = charParaInt(var[3]);
        data.F0 = charParaInt(var[2]);
        data.SRA1 = charParaInt(var[1]);
        data.SLL8 = charParaInt(var[0]);

        inputData.push_back(data);
    }

    return inputData;
}

vector<SinaisdeControle> readSinaisdeControle(const string input, const string output){
    vector<string> inputString;
    inputString = lerArquivo(input, output);

    vector<SinaisdeControle>  inputData;
    inputData = extractInstruction(inputString);

    return inputData;
}

EstadoULA controlOperation(const SinaisdeControle control, EstadoULA& ULAState){
    EstadoULA result;

    if (control.F0 == 0 && control.F1 == 0)
    {
        result.S = ULAState.A & ULAState.B;
        result.Carry = 0;

    }else if (control.F0 == 1 && control.F1 == 0)
    {
        result.S = ULAState.A | ULAState.B;
        result.Carry = 0;

    }else if (control.F0 == 0 && control.F1 == 1)
    {
        result.S = ULAState.A ^ ULAState.B;
        result.Carry = 0;

    }else if (control.F0 == 1 && control.F1 == 1)
    {
        result.S = ULAState.A + ULAState.B + ULAState.Carry;
        result.Carry = ((unsigned long long)ULAState.A + (unsigned long long)ULAState.B // * unsigned long long para previnir overflow na soma antes da comparação
                        + (unsigned long long)ULAState.Carry > 0xFFFFFFFF) ? 1 : 0; 
        
    }

    return result;
}

void execTask(const string input, const string output){

    vector<SinaisdeControle> inputData = readSinaisdeControle(input, output);
    ofstream outputFile(output);

    if (inputData.empty()) {
        cerr << "Nenhuma instrução válida encontrada no arquivo de entrada." << endl;
        return;
    }



    outputFile.close();
}
