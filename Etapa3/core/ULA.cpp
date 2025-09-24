#include "ULA.h"
#include <sstream>
#include <cstdint>
#include "Memoria.h"

int charParaInt(char c) {
    return (c == '1') ? 1 : 0;
}

// Função para extrair instruções de 21 bits
vector<SinaisCompletos> extractInstructionCompleta(vector<string> inst) {
    vector<SinaisCompletos> inputData;
    
    for(auto var : inst) {
        if(var.length() != 23) continue;
        
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
        // 2 bits de read e write (bits 17-18)
        data.read = (var[17] == '1');
        data.write = (var[18] == '1');
        
        // 4 bits do barramento B (bits 19-22)
        data.barramentoB = 0;
        for(int i = 0; i < 4; i++) {
            data.barramentoB |= (charParaInt(var[19 + i]) << (3 - i));
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
void execTask(vector<SinaisCompletos>& inputData, Registradores& regs, Memoria& mem, const string& output) {
    vector<EstadoULA> log;
    EstadoULA ULAState;
    ULAState.regs = regs; // Usa o estado inicial dos registradores

    int ciclo = 0;

    for (const auto& instrucao : inputData) {
        Registradores estadoInicialRegs = ULAState.regs;

        // --- Formar IR de 23 bits para o log (SEU ESTILO) ---
        string ir = "";
        ir += to_string(instrucao.ULA.SLL8) + to_string(instrucao.ULA.SRA1);
        ir += to_string(instrucao.ULA.F0) + to_string(instrucao.ULA.F1);
        ir += to_string(instrucao.ULA.ENA) + to_string(instrucao.ULA.ENB);
        ir += to_string(instrucao.ULA.INVA) + to_string(instrucao.ULA.INC);
        
        for(int i = 8; i >= 0; i--) { // Barramento C
            ir += to_string((instrucao.barramentoC >> i) & 1);
        }
        
        ir += to_string(instrucao.write) + to_string(instrucao.read); // Memória

        for(int i = 3; i >= 0; i--) { // Barramento B
            ir += to_string((instrucao.barramentoB >> i) & 1);
        }

        // --- CASO ESPECIAL: FETCH ---
        if (instrucao.read && instrucao.write) {
            string byte_imediato_str = ir.substr(0, 8);
            uint8_t byte_imediato = 0;
            for(int i = 0; i < 8; i++) {
                byte_imediato |= (charParaInt(byte_imediato_str[i]) << (7-i));
            }
            
            ULAState.regs.MBR = byte_imediato;
            ULAState.regs.H = static_cast<uint32_t>(ULAState.regs.MBR);
            ciclo++;
            continue; 
        }

        // --- CICLO NORMAL ---
        ULAState.A = ULAState.regs.H;
        ULAState.B = decodificadorBarramentoB(instrucao.barramentoB, ULAState.regs);

        ULAState.registradorB = getRegistradorBNome(instrucao.barramentoB);
        vector<string> nomesC = getRegistradoresCNomes(instrucao.barramentoC);
        ULAState.registradoresC = "";
        for (size_t i = 0; i < nomesC.size(); i++) {
            ULAState.registradoresC += nomesC[i];
            if (i < nomesC.size() - 1) ULAState.registradoresC += ",";
        }
        
        EstadoULA result = controlOperation(instrucao.ULA, ULAState);
        
        vector<int> habilitados = seletorBarramentoC(instrucao.barramentoC);
        atualizarRegistradores(ULAState.regs, result.S, habilitados);

        // --- LÓGICA DE MEMÓRIA ---
        if (instrucao.write) {
            mem.escrever(ULAState.regs.MAR, ULAState.regs.MDR);
        } else if (instrucao.read) {
            ULAState.regs.MDR = mem.ler(ULAState.regs.MAR);
        }

        // --- PREPARAR LOG ---
        EstadoULA estadoParaLog;
        estadoParaLog.regs = estadoInicialRegs;
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
        
        estadoParaLog.regs = ULAState.regs;
        
        log.push_back(estadoParaLog);
        ciclo++;
    }

    regs = ULAState.regs;
    saveLog(log, output, mem); 
}

// Função de salvar log
void saveLog(vector<EstadoULA>& log, const string& output, Memoria& mem) {
    ofstream log_file(output, ios::app);
    if (!log_file.is_open()) {
        cerr << "Erro ao abrir o arquivo de log: " << output << endl;
        return;
    }

    string linha_sep(100, '=');
    
    // Log de um ciclo
    const auto& estado = log.back();
        
    log_file << "CICLO " << log.size() << "\n";
    log_file << "PC: " << estado.regPC << "\n";
    log_file << "IR: " << estado.regIR << "\n\n";
    

    string mbr_bin = "";
    for (int i = 7; i >= 0; --i) {
        mbr_bin += to_string((estado.regs.MBR >> i) & 1);
    }
    log_file << "MBR: " << mbr_bin << "\n";
    // ...
    
    // --- NOVO: LOG DA MEMÓRIA ---
    log_file << "\nESTADO DA MEMÓRIA APÓS O CICLO:\n";
    mem.log(log_file);

    log_file << linha_sep << "\n";
    
    log_file.close();
}

// Funções de tradução

vector<uint32_t> traduzir_iload(int x) {
    vector<uint32_t> microcodigo;
    // Usamos o prefixo 0b para definir as microinstruções diretamente como inteiros
    // Formato: [8 ULA][9 C][2 Mem][4 B]
    microcodigo.push_back(0b00100100100000000000101); // H = LV
    for (int i = 0; i < x; ++i) {
        microcodigo.push_back(0b00110101100000000001000); // H = H + 1
    }
    microcodigo.push_back(0b00110100000000001011000); // MAR = H; rd
    microcodigo.push_back(0b00110101000001001100100); // MAR = SP = SP + 1; wr
    microcodigo.push_back(0b00110100001000000000000); // TOS = MDR
    return microcodigo;
}

vector<uint32_t> traduzir_bipush(const string& byte_str) {
    vector<uint32_t> microcodigo;
    microcodigo.push_back(0b00110101000001001000100); // SP = MAR = SP + 1

    // Construção dinâmica da instrução FETCH
    uint8_t byte_valor = 0;
    for(int i = 0; i < 8; ++i) {
        byte_valor |= (charParaInt(byte_str[i]) << (7 - i));
    }
    
    // A parte fixa da instrução (C=0, Mem=11, B=PC, por exemplo 0100)
    const uint32_t parte_fixa = 0b000000000110100;
    // Combina o byte (deslocado para a posição correta) com a parte fixa
    uint32_t fetch_op = (static_cast<uint32_t>(byte_valor) << 15) | parte_fixa;
    microcodigo.push_back(fetch_op);

    microcodigo.push_back(0b00110100011000010101000); // MDR = TOS = H; wr
    return microcodigo;
}

vector<uint32_t> traduzir_dup() {
    vector<uint32_t> microcodigo;
    // MAR = SP = SP + 1
    microcodigo.push_back(0b00110101000001001000100);
    // MDR = TOS; wr
    microcodigo.push_back(0b00110100001000010100111);
    return microcodigo;
}

void processarArquivoIJVM(const string& arq_instrucoes, const string& arq_saida, Registradores& regs, Memoria& mem) {
    // Limpa o arquivo de log no início da execução
    ofstream limpar(arq_saida, ios::trunc);
    limpar.close();

    vector<string> linhas = lerLinhasDeArquivo(arq_instrucoes);

    for (const auto& linha : linhas) {
        stringstream ss(linha);
        string comando;
        ss >> comando;

        vector<uint32_t> microcodigo_numerico;

        if (comando == "ILOAD") {
            int x; ss >> x;
            microcodigo_numerico = traduzir_iload(x);
        } else if (comando == "BIPUSH") {
            string byte_arg; ss >> byte_arg;
            microcodigo_numerico = traduzir_bipush(byte_arg);
        } else if (comando == "DUP") {
            microcodigo_numerico = traduzir_dup();
        }

        vector<string> microcodigo_str;
        for(uint32_t mc : microcodigo_numerico) {
            string temp = "";
            for(int i = 22; i >= 0; i--) {
                temp += to_string((mc >> i) & 1);
            }
            microcodigo_str.push_back(temp);
        }
        
        vector<SinaisCompletos> sinais = extractInstructionCompleta(microcodigo_str);
        
        execTask(sinais, regs, mem, arq_saida);
    }
}