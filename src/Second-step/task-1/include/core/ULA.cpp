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
    int A = ULAState.A;
    int B = ULAState.B;
    
    if (control.ENA == 0)
        A = 0;
    if (control.ENB == 0)
        B = 0;
    if (control.INVA == 1)
        A = ~A;
    

    if (control.F0 == 0 && control.F1 == 0)
    {
        result.S = A & B;
        result.Carry = 0;

    }else if (control.F0 == 0 && control.F1 == 1)
    {
        result.S = A | B;
        result.Carry = 0;

    }else if (control.F0 == 1 && control.F1 == 0)
    {
        result.S = A ^ B;
        result.Carry = 0;

    }else if (control.F0 == 1 && control.F1 == 1)
    {
        result.S = A + B + ULAState.Carry;
        result.Carry = ((unsigned long long)A + (unsigned long long)B // * unsigned long long para previnir overflow na soma antes da comparação
            + (unsigned long long)ULAState.Carry > 0xFFFFFFFF) ? 1 : 0;
    }

    //Operações de shift
    if (control.SLL8 == 1)
        result.S = (result.S << 8);
    else if (control.SRA1 == 1)
        result.S = (result.S >> 1);

    return result;
}

void execTask(const string input, const string output){

    vector<SinaisdeControle> inputData = readSinaisdeControle(input, output);
    ifstream inputFile(input);
    ofstream outputFile(output);
    vector<EstadoULA> log;
    int PC = 0;

    if (inputData.empty()) {
        cerr << "Nenhuma instrução válida encontrada no arquivo de entrada." << endl;
        return;
    }

    if (!outputFile.is_open())
    {
        cerr << "Não foi possível abrir o arquivo de saída: " << output << endl;
        return;
    }

    if (!inputFile.is_open()) {
        cerr << "Não foi possível abrir o arquivo de entrada: " << input << endl;
        return;
    }

    EstadoULA ULAState;
    EstadoULA result;

    ULAState.A = 1;
    ULAState.B = 1;
    ULAState.Carry = 0;

    for(auto var : inputData)
    {
        ULAState.regPC = PC;
        ULAState.regIR = to_string(var.SLL8) + to_string(var.SRA1) + to_string(var.F1) 
            + to_string(var.F0) + to_string(var.ENA) + to_string(var.ENB) 
            + to_string(var.INVA) + to_string(var.INC);

        result = controlOperation(var, ULAState);
        ULAState.S = result.S;
        ULAState.Carry = result.Carry;
        PC++;

        log.push_back(ULAState);
    }

    saveLog(log, output);
    inputFile.close();
    outputFile.close();
}

void saveLog(vector<EstadoULA> log, string output) {
    string linha, linha2;
    for (int i = 0; i < 132; i++) {
        linha += '=';
        linha2 += "-";
    }

    writeLineInFile(output, "Início do programa");
    writeLineInFile(output, linha);
    writeLineInFile(output, "Ciclo\tPC\t  IR\t\t\t\t   A\t\t\t\t\t\t\t   B\t\t\t\t\t\t\t\tS  \t\t\t\t  Carry");
    writeLineInFile(output, linha);
    int ciclos = 0;
    for (const auto& estado : log) {
        ciclos++;
        string registro = to_string(estado.regPC + 1) + "\t\t" + to_string(estado.regPC + 1) + "\t"
            + estado.regIR + "\t"
            + paraBinario32bits(estado.A) + "\t"
            + paraBinario32bits(estado.B) + "\t"
            + paraBinario32bits(estado.S) + "\t"
            + to_string(estado.Carry);
        writeLineInFile(output, registro);
    }
    writeLineInFile(output, linha);
    writeLineInFile(output, "Ciclo " + to_string(ciclos + 1));
    writeLineInFile(output, "PC " + to_string(ciclos + 1));
    writeLineInFile(output, "Line is empty, EOP");
    cout << "Log salvo em: " << output << endl;
}