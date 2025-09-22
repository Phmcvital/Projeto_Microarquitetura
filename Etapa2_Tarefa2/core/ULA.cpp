#include "ULA.h"

int charParaInt(char c) {
    return (c == '1') ? 1 : 0;
}

// Função para extrair instruções de 21 bits
vector<SinaisCompletos> extractInstructionCompleta(vector<string> inst) {
    vector<SinaisCompletos> inputData;
    
    for(auto var : inst) {
        if(var.length() != 21) continue;
        
        SinaisCompletos data;
        
        // 8 bits da ULA (bits 0-7)
        data.ULA.SLL8 = charParaInt(var[0]);
        data.ULA.SRA1 = charParaInt(var[1]);
        data.ULA.F0 = charParaInt(var[2]);
        data.ULA.F1 = charParaInt(var[3]);
        data.ULA.ENA = charParaInt(var[4]);
        data.ULA.ENB = charParaInt(var[5]);
        data.ULA.INVA = charParaInt(var[6]);
        data.ULA.INC = charParaInt(var[7]);
        
        // 9 bits do barramento C (bits 8-16)
        data.barramentoC = 0;
        for(int i = 0; i < 9; i++) {
            data.barramentoC |= (charParaInt(var[8 + i]) << (8 - i));
        }
        
        // 4 bits do barramento B (bits 17-20)
        data.barramentoB = 0;
        for(int i = 0; i < 4; i++) {
            data.barramentoB |= (charParaInt(var[17 + i]) << (3 - i));
        }
        
        inputData.push_back(data);
    }
    
    return inputData;
}

// Função para ler sinais completos
vector<SinaisCompletos> readSinaisCompletos(const string input, const string output){
    vector<string> inputString = lerArquivo(input, output);
    return extractInstructionCompleta(inputString);
}

// Decodificador do barramento B
int decodificadorBarramentoB(int codigo, const Registradores& regs) {
    switch(codigo) {
        case 0: return regs.MDR;
        case 1: return regs.PC;
        case 2: return signExtend8to32(regs.MBR);  // MBR com extensão de sinal
        case 3: return regs.MBR & 0xFF;            // MBRU sem extensão de sinal
        case 4: return regs.SP;
        case 5: return regs.LV;
        case 6: return regs.CPP;
        case 7: return regs.TOS;
        case 8: return regs.OPC;
        default: return 0;
    }
}

// Obter nome do registrador B
string getRegistradorBNome(int codigo) {
    switch(codigo) {
        case 0: return "MDR";
        case 1: return "PC";
        case 2: return "MBR";
        case 3: return "MBRU";
        case 4: return "SP";
        case 5: return "LV";
        case 6: return "CPP";
        case 7: return "TOS";
        case 8: return "OPC";
        default: return "NONE";
    }
}

// Seletor do barramento C
vector<int> seletorBarramentoC(int codigo) {
    vector<int> habilitados;
    if(codigo & 0x001) habilitados.push_back(0);  // MAR
    if(codigo & 0x002) habilitados.push_back(1);  // MDR
    if(codigo & 0x004) habilitados.push_back(2);  // PC
    if(codigo & 0x008) habilitados.push_back(3);  // SP
    if(codigo & 0x010) habilitados.push_back(4);  // LV
    if(codigo & 0x020) habilitados.push_back(5);  // CPP
    if(codigo & 0x040) habilitados.push_back(6);  // TOS
    if(codigo & 0x080) habilitados.push_back(7);  // OPC
    if(codigo & 0x100) habilitados.push_back(8);  // H
    return habilitados;
}

// Obter nomes dos registradores C
vector<string> getRegistradoresCNomes(int codigo) {
    vector<string> nomes;
    if(codigo & 0x001) nomes.push_back("MAR");
    if(codigo & 0x002) nomes.push_back("MDR");
    if(codigo & 0x004) nomes.push_back("PC");
    if(codigo & 0x008) nomes.push_back("SP");
    if(codigo & 0x010) nomes.push_back("LV");
    if(codigo & 0x020) nomes.push_back("CPP");
    if(codigo & 0x040) nomes.push_back("TOS");
    if(codigo & 0x080) nomes.push_back("OPC");
    if(codigo & 0x100) nomes.push_back("H");
    return nomes;
}

// Atualizar registradores
void atualizarRegistradores(Registradores& regs, int saida, const vector<int>& habilitados) {
    for(int reg : habilitados) {
        switch(reg) {
            case 0: regs.MAR = saida; break;
            case 1: regs.MDR = saida; break;
            case 2: regs.PC = saida; break;
            case 3: regs.SP = saida; break;
            case 4: regs.LV = saida; break;
            case 5: regs.CPP = saida; break;
            case 6: regs.TOS = saida; break;
            case 7: regs.OPC = saida; break;
            case 8: regs.H = saida; break;
        }
    }
}

// Extensão de sinal de 8 para 32 bits
int signExtend8to32(int valor8bits) {
    if(valor8bits & 0x80) {  // Se bit de sinal está ativo
        return valor8bits | 0xFFFFFF00;
    }
    return valor8bits & 0xFF;
}

// Operação de controle modificada
EstadoULA controlOperation(const SinaisdeControle control, EstadoULA& ULAState){
    EstadoULA result = ULAState;
    
    int A = ULAState.A;
    int B = ULAState.B;
    
    if (control.ENA == 0) A = 0;
    if (control.ENB == 0) B = 0;
    if (control.INVA == 1) A = ~A;
    
    if (control.INC == 1) result.Carry = 1;
    else result.Carry = 0;
    
    if (control.F0 == 0 && control.F1 == 0) {
        result.S = A & B;
    } else if (control.F0 == 0 && control.F1 == 1) {
        result.S = A | B;
    } else if (control.F0 == 1 && control.F1 == 0) {
        result.S = A ^ B;
    } else if (control.F0 == 1 && control.F1 == 1) {
        long long soma = (long long)A + (long long)B + (long long)result.Carry;
        result.S = (int)soma;
        result.Carry = (soma > 0xFFFFFFFF || soma < 0) ? 1 : 0;
    }
    
    // Operações de shift
    if (control.SLL8 == 1) {
        result.S = (result.S << 8);
    } else if (control.SRA1 == 1) {
        result.S = (result.S >> 1);
    }
    
    // Sinais N e Z
    result.N = (result.S < 0) ? 1 : 0;
    result.Z = (result.S == 0) ? 1 : 0;
    
    return result;
}

// Execução da tarefa modificada
void execTask(const string input, const string output){
    vector<SinaisCompletos> inputData = readSinaisCompletos(input, output);
    vector<EstadoULA> log;
    
    if (inputData.empty()) {
        cerr << "Nenhuma instrução válida encontrada no arquivo de entrada." << endl;
        return;
    }
    
    // Inicialização dos registradores COM VALORES DE TESTE
    EstadoULA ULAState;
    ULAState.regs.H = 10;      // Valores de teste
    ULAState.regs.OPC = 0;
    ULAState.regs.TOS = 5;
    ULAState.regs.CPP = 0;
    ULAState.regs.LV = 15;
    ULAState.regs.SP = 0;
    ULAState.regs.PC = 0;
    ULAState.regs.MDR = 20;
    ULAState.regs.MAR = 0;
    ULAState.regs.MBR = 0;
    
    int ciclo = 0;
    
    for(auto instrucao : inputData) {
        // Salvar estado inicial para o log
        Registradores estadoInicialRegs = ULAState.regs;
        
        // Formar IR de 21 bits
        string ir = "";
        ir += to_string(instrucao.ULA.SLL8) + to_string(instrucao.ULA.SRA1);
        ir += to_string(instrucao.ULA.F0) + to_string(instrucao.ULA.F1);
        ir += to_string(instrucao.ULA.ENA) + to_string(instrucao.ULA.ENB);
        ir += to_string(instrucao.ULA.INVA) + to_string(instrucao.ULA.INC);
        
        // Adicionar bits do barramento C (9 bits)
        for(int i = 8; i >= 0; i--) {
            ir += to_string((instrucao.barramentoC >> i) & 1);
        }
        
        // Adicionar bits do barramento B (4 bits)  
        for(int i = 3; i >= 0; i--) {
            ir += to_string((instrucao.barramentoB >> i) & 1);
        }
        
        // Configurar entradas da ULA
        ULAState.A = ULAState.regs.H;  // A sempre vem de H
        ULAState.B = decodificadorBarramentoB(instrucao.barramentoB, ULAState.regs);
        
        // Obter nomes dos registradores ANTES da operação
        ULAState.registradorB = getRegistradorBNome(instrucao.barramentoB);
        vector<string> nomesC = getRegistradoresCNomes(instrucao.barramentoC);
        ULAState.registradoresC = "";
        for(size_t i = 0; i < nomesC.size(); i++) {
            ULAState.registradoresC += nomesC[i];
            if(i < nomesC.size() - 1) ULAState.registradoresC += ",";
        }
        
        // Executar operação na ULA
        EstadoULA result = controlOperation(instrucao.ULA, ULAState);
        
        // Atualizar registradores habilitados no barramento C
        vector<int> habilitados = seletorBarramentoC(instrucao.barramentoC);
        atualizarRegistradores(ULAState.regs, result.S, habilitados);
        
        // Preparar dados para o log
        EstadoULA estadoParaLog;
        estadoParaLog.regs = estadoInicialRegs; // Estado inicial
        estadoParaLog.S = result.S;
        estadoParaLog.Carry = result.Carry;
        estadoParaLog.N = result.N;
        estadoParaLog.Z = result.Z;
        estadoParaLog.regPC = ciclo;
        estadoParaLog.regIR = ir;
        estadoParaLog.registradorB = ULAState.registradorB;
        estadoParaLog.registradoresC = ULAState.registradoresC;
        estadoParaLog.A = ULAState.A;
        estadoParaLog.B = ULAState.B;
        
        // Adicionar estado final dos registradores
        estadoParaLog.regs = ULAState.regs;
        
        log.push_back(estadoParaLog);
        ciclo++;
    }
    
    saveLog(log, output);
}

// Função de salvar log
void saveLog(vector<EstadoULA> log, string output) {
    // Limpar arquivo de saída
    ofstream limpar(output, ios::trunc);
    limpar.close();
    
    string linha = string(200, '=');
    
    writeLineInFile(output, "Início do programa - Etapa 2 Tarefa 2");
    writeLineInFile(output, linha);
    
    for (size_t i = 0; i < log.size(); i++) {
        const auto& estado = log[i];
        
        writeLineInFile(output, "CICLO " + to_string(i + 1));
        writeLineInFile(output, "PC: " + to_string(estado.regPC));
        writeLineInFile(output, "IR: " + estado.regIR);
        writeLineInFile(output, "");
        
        // Estado inicial dos registradores
        writeLineInFile(output, "REGISTRADORES INICIAL:");
        writeLineInFile(output, "H: " + paraBinario32bits(estado.regs.H));
        writeLineInFile(output, "OPC: " + paraBinario32bits(estado.regs.OPC));
        writeLineInFile(output, "TOS: " + paraBinario32bits(estado.regs.TOS));
        writeLineInFile(output, "CPP: " + paraBinario32bits(estado.regs.CPP));
        writeLineInFile(output, "LV: " + paraBinario32bits(estado.regs.LV));
        writeLineInFile(output, "SP: " + paraBinario32bits(estado.regs.SP));
        writeLineInFile(output, "MBR: " + bitset<8>(estado.regs.MBR).to_string());
        writeLineInFile(output, "PC: " + paraBinario32bits(estado.regs.PC));
        writeLineInFile(output, "MDR: " + paraBinario32bits(estado.regs.MDR));
        writeLineInFile(output, "MAR: " + paraBinario32bits(estado.regs.MAR));
        writeLineInFile(output, "");
        
        // MOSTRAR OS VALORES DE A e B que entraram na ULA
        writeLineInFile(output, "ENTRADAS ULA:");
        writeLineInFile(output, "A (H): " + paraBinario32bits(estado.A));
        writeLineInFile(output, "B (" + estado.registradorB + "): " + paraBinario32bits(estado.B));
        writeLineInFile(output, "");
        
        writeLineInFile(output, "BARRAMENTO B: " + estado.registradorB);
        writeLineInFile(output, "BARRAMENTO C: " + estado.registradoresC);
        writeLineInFile(output, "");
        
        writeLineInFile(output, "RESULTADO ULA:");
        writeLineInFile(output, "S: " + paraBinario32bits(estado.S));
        writeLineInFile(output, "Carry: " + to_string(estado.Carry));
        writeLineInFile(output, "N: " + to_string(estado.N));
        writeLineInFile(output, "Z: " + to_string(estado.Z));
        writeLineInFile(output, "");

        // Estado depois dos registradores
        writeLineInFile(output, "REGISTRADORES FINAL:");
        writeLineInFile(output, "H: " + paraBinario32bits(estado.regs.H));
        writeLineInFile(output, "OPC: " + paraBinario32bits(estado.regs.OPC));
        writeLineInFile(output, "TOS: " + paraBinario32bits(estado.regs.TOS));
        writeLineInFile(output, "CPP: " + paraBinario32bits(estado.regs.CPP));
        writeLineInFile(output, "LV: " + paraBinario32bits(estado.regs.LV));
        writeLineInFile(output, "SP: " + paraBinario32bits(estado.regs.SP));
        writeLineInFile(output, "MBR: " + bitset<8>(estado.regs.MBR).to_string());
        writeLineInFile(output, "PC: " + paraBinario32bits(estado.regs.PC));
        writeLineInFile(output, "MDR: " + paraBinario32bits(estado.regs.MDR));
        writeLineInFile(output, "MAR: " + paraBinario32bits(estado.regs.MAR));
        writeLineInFile(output, "");

        writeLineInFile(output, linha);
    }
    
    cout << "Log salvo em: " << output << endl;
}

