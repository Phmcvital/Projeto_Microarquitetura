#include "ULA.h"

int charParaInt(char c) {
    return (c == '1') ? 1 : 0;
}

array<int, 23> stringParaMicroinstrucao(const string& s) {
    array<int, 23> micro;
    for (size_t i = 0; i < 23 && i < s.length(); ++i) {
        micro[i] = s[i] - '0';
    }
    return micro;
}

vector<SinaisCompletos23> readSinais23Bits(const string input) {
    vector<SinaisCompletos23> instrucoes;
    ifstream arquivo(input);
    string linha;
    
    if (!arquivo.is_open()) {
        cerr << "Não foi possível abrir o arquivo: " << input << endl;
        return instrucoes;
    }
    
    while (getline(arquivo, linha)) {
        // Remover espaços e caracteres inválidos
        linha.erase(remove_if(linha.begin(), linha.end(), ::isspace), linha.end());
        if (linha.empty()) continue;
        
        if (linha.length() == 23) {
            bool valida = true;
            for (char c : linha) {
                if (c != '0' && c != '1') {
                    valida = false;
                    break;
                }
            }
            
            if (valida) {
                SinaisCompletos23 instrucao;
                instrucao.bits = stringParaMicroinstrucao(linha);
                instrucoes.push_back(instrucao);
            }
        }
    }
    
    arquivo.close();
    return instrucoes;
}

int32_t decodificadorBarramentoB(uint8_t codigo, const Registradores& regs) {
    switch(codigo) {
        case 0: return regs.MDR;
        case 1: return regs.PC;
        case 2: return static_cast<int32_t>(static_cast<int8_t>(regs.MBR)); // MBR com sinal
        case 3: return static_cast<int32_t>(regs.MBR); // MBR sem sinal
        case 4: return regs.SP;
        case 5: return regs.LV;
        case 6: return regs.CPP;
        case 7: return regs.TOS;
        default: return regs.OPC;
    }
}

string getRegistradorBNome(uint8_t codigo) {
    switch(codigo) {
        case 0: return "MDR";
        case 1: return "PC";
        case 2: return "MBR";
        case 3: return "MBRU";
        case 4: return "SP";
        case 5: return "LV";
        case 6: return "CPP";
        case 7: return "TOS";
        default: return "OPC";
    }
}

vector<int> seletorBarramentoC(int codigo) {
    vector<int> habilitados;
    if(codigo & 0x100) habilitados.push_back(8); // H
    if(codigo & 0x080) habilitados.push_back(7); // OPC
    if(codigo & 0x040) habilitados.push_back(6); // TOS
    if(codigo & 0x020) habilitados.push_back(5); // CPP
    if(codigo & 0x010) habilitados.push_back(4); // LV
    if(codigo & 0x008) habilitados.push_back(3); // SP
    if(codigo & 0x004) habilitados.push_back(2); // PC
    if(codigo & 0x002) habilitados.push_back(1); // MDR
    if(codigo & 0x001) habilitados.push_back(0); // MAR
    return habilitados;
}

string getRegistradoresCNomes(int codigo) {
    vector<string> nomes;
    if(codigo & 0x100) nomes.push_back("H");
    if(codigo & 0x080) nomes.push_back("OPC");
    if(codigo & 0x040) nomes.push_back("TOS");
    if(codigo & 0x020) nomes.push_back("CPP");
    if(codigo & 0x010) nomes.push_back("LV");
    if(codigo & 0x008) nomes.push_back("SP");
    if(codigo & 0x004) nomes.push_back("PC");
    if(codigo & 0x002) nomes.push_back("MDR");
    if(codigo & 0x001) nomes.push_back("MAR");
    
    string resultado;
    for (size_t i = 0; i < nomes.size(); ++i) {
        if (!resultado.empty()) resultado += ", ";
        resultado += nomes[i];
    }
    return resultado.empty() ? "Nenhum" : resultado;
}

void atualizarRegistradores(Registradores& regs, int32_t saida, const vector<int>& habilitados) {
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

void carregarRegistradores(const string& arquivo, Registradores& regs) {
    ifstream file(arquivo);
    if (!file.is_open()) {
        cerr << "ERRO: Não foi possível abrir arquivo de registradores: " << arquivo << endl;
        return;
    }
    
    map<string, int32_t> valores;
    string linha;
    
    while (getline(file, linha)) {
        size_t pos = linha.find('=');
        if (pos == string::npos) continue;
        
        string nome = linha.substr(0, pos);
        string valorBinario = linha.substr(pos + 1);
        
        // Remover espaços
        nome.erase(remove_if(nome.begin(), nome.end(), ::isspace), nome.end());
        valorBinario.erase(remove_if(valorBinario.begin(), valorBinario.end(), ::isspace), valorBinario.end());
        
        if (!valorBinario.empty()) {
            valores[nome] = stoll(valorBinario, nullptr, 2);
        }
    }
    
    // Atribuir valores aos registradores
    regs.MAR = valores["mar"];
    regs.MDR = valores["mdr"];
    regs.PC = valores["pc"];
    regs.MBR = static_cast<uint8_t>(valores["mbr"]);
    regs.SP = valores["sp"];
    regs.LV = valores["lv"];
    regs.CPP = valores["cpp"];
    regs.TOS = valores["tos"];
    regs.OPC = valores["opc"];
    regs.H = valores["h"];
    
    file.close();
}

bool lerMemoria32Bin(const string& path, vector<int32_t>& MEM) {
    ifstream fin(path);
    if (!fin) return false;
    
    MEM.clear();
    string line;
    
    while (fin >> line) {
        if (line.size() == 32) {
            MEM.push_back(stoll(line, nullptr, 2));
        }
    }
    return true;
}

string conversor_binario(int32_t valor) {
    string s(32, '0');
    for (int i = 0; i < 32; i++) {
        if (valor & (1 << (31 - i))) {
            s[i] = '1';
        }
    }
    return s;
}

EstadoULA controlOperation(const SinaisdeControle control, EstadoULA& ULAState) {
    EstadoULA result = ULAState;
    
    int32_t A = ULAState.A;
    int32_t B = ULAState.B;
    
    if (control.ENA == 0) A = 0;
    if (control.ENB == 0) B = 0;
    if (control.INVA == 1) A = ~A;
    
    // Operações da ULA
    if (control.F0 == 0 && control.F1 == 0) {
        result.S = A & B;
    } else if (control.F0 == 0 && control.F1 == 1) {
        result.S = A | B;
    } else if (control.F0 == 1 && control.F1 == 0) {
        result.S = ~B;
    } else if (control.F0 == 1 && control.F1 == 1) {
        uint64_t soma = (uint64_t)(uint32_t)A + (uint64_t)(uint32_t)B + (uint64_t)control.INC;
        result.S = (int32_t)soma;
        result.Carry = (soma > 0xFFFFFFFF) ? 1 : 0;
    }
    
    // Shifts
    if (control.SLL8 == 1) {
        result.S = result.S << 8;
    }
    if (control.SRA1 == 1) {
        result.S = result.S >> 1;
    }
    
    result.N = (result.S < 0) ? 1 : 0;
    result.Z = (result.S == 0) ? 1 : 0;
    
    return result;
}

void executarMicroinstrucoes(vector<SinaisCompletos23>& microinstrucoes, 
                           Registradores& regs, vector<int32_t>& MEM, 
                           ofstream& saida, int& ciclo_global) {
    
    for (const auto& instrucao : microinstrucoes) {
        saida << "--- Microinstrução " << ciclo_global++ << " ---" << endl;
        
        // Converter instrução para string
        string instrucao_str;
        for (int bit : instrucao.bits) instrucao_str += to_string(bit);
        saida << "Instrução (IR): " << instrucao_str << endl;
        
        saida << "Registradores no Início:" << endl;
        saida << "H = " << regs.H << " OPC = " << regs.OPC << " TOS = " << regs.TOS 
              << " CPP = " << regs.CPP << " LV = " << regs.LV << " SP = " << regs.SP 
              << " PC = " << regs.PC << " MDR = " << regs.MDR << " MAR = " << regs.MAR 
              << " MBR = " << (int)regs.MBR << endl;
        
        uint8_t codigoB = instrucao.barramentoB();
        saida << "Barramento B comandado por: " << getRegistradorBNome(codigoB) << endl;
        saida << "Barramento C habilitado para: " << getRegistradoresCNomes(instrucao.barramentoC()) 
              << " | READ = " << instrucao.READ() << " WRITE = " << instrucao.WRITE() << endl;
        
        // CASO ESPECIAL FETCH (BIPUSH)
        if (instrucao.READ() && instrucao.WRITE()) {
            saida << "[FETCH] Carregando byte para MBR e H" << endl;
            uint8_t byte_param = 0;
            for (int i = 0; i < 8; ++i) {
                byte_param = (byte_param << 1) | instrucao.bits[i];
            }
            regs.MBR = byte_param;
            regs.H = static_cast<uint32_t>(regs.MBR);
        } else {
            // Operação normal da ULA
            int32_t A = regs.H;
            int32_t B = decodificadorBarramentoB(codigoB, regs);
            
            SinaisdeControle controleULA;
            controleULA.SLL8 = instrucao.SLL8();
            controleULA.SRA1 = instrucao.SRA1();
            controleULA.F0 = instrucao.F0();
            controleULA.F1 = instrucao.F1();
            controleULA.ENA = instrucao.ENA();
            controleULA.ENB = instrucao.ENB();
            controleULA.INVA = instrucao.INVA();
            controleULA.INC = instrucao.INC();
            
            EstadoULA estadoULA;
            estadoULA.A = A;
            estadoULA.B = B;
            
            EstadoULA resultado = controlOperation(controleULA, estadoULA);
            
            int32_t Sd = resultado.S;
            if (instrucao.SLL8()) Sd <<= 8;
            if (instrucao.SRA1()) Sd >>= 1;
            
            vector<int> habilitados = seletorBarramentoC(instrucao.barramentoC());
            atualizarRegistradores(regs, Sd, habilitados);
            
            // Operações de memória
            if (instrucao.READ() && !instrucao.WRITE()) {
                saida << "[READ] MDR <- MEM[" << regs.MAR << "]";
                if (regs.MAR >= 0 && regs.MAR < (int)MEM.size()) {
                    saida << "(" << MEM[regs.MAR] << ")" << endl;
                    regs.MDR = MEM[regs.MAR];
                } else {
                    saida << endl << "[READ][ERRO] Endereço MAR inválido: " << regs.MAR << endl;
                }
            }
            
            if (instrucao.WRITE() && !instrucao.READ()) {
                saida << "[WRITE] MEM[" << regs.MAR << "] <- MDR(" << regs.MDR << ")" << endl;
                if (regs.MAR >= 0 && regs.MAR < (int)MEM.size()) {
                    MEM[regs.MAR] = regs.MDR;
                } else {
                    saida << "[WRITE][ERRO] Endereço MAR inválido: " << regs.MAR << endl;
                }
            }
        }
        
        saida << "Registradores no Fim:" << endl;
        saida << "H = " << regs.H << " OPC = " << regs.OPC << " TOS = " << regs.TOS 
              << " CPP = " << regs.CPP << " LV = " << regs.LV << " SP = " << regs.SP 
              << " PC = " << regs.PC << " MDR = " << regs.MDR << " MAR = " << regs.MAR 
              << " MBR = " << (int)regs.MBR << endl << endl;
        
        saida << "Memória de Dados:" << endl;
        for (size_t i = 0; i < MEM.size(); ++i) {
            saida << "MEM[" << i << "] = " << conversor_binario(MEM[i]) << endl;
        }
        saida << endl;
    }
}

void execTask(const string arquivoInstrucoes, const string output, 
              const string arquivoMemoria, const string arquivoRegistradores) {
    
    cout << "Arquivo de instruções: " << arquivoInstrucoes << endl;
    cout << "Arquivo de saída: " << output << endl;
    cout << "Memória: " << arquivoMemoria << endl;
    cout << "Registradores: " << arquivoRegistradores << endl;

    // 1. Carregar memória
    vector<int32_t> MEM;
    if (!lerMemoria32Bin(arquivoMemoria, MEM)) {
        cerr << "ERRO: Não foi possível carregar memória inicial" << endl;
        return;
    }

    // 2. Carregar registradores iniciais
    Registradores regs;
    carregarRegistradores(arquivoRegistradores, regs);

    // 3. Abrir arquivo de instruções de alto nível
    ifstream arquivoInstrucoesAltoNivel(arquivoInstrucoes);
    if (!arquivoInstrucoesAltoNivel) {
        cerr << "ERRO: Não foi possível abrir arquivo de instruções: " << arquivoInstrucoes << endl;
        return;
    }

    // 4. Preparar arquivo de saída
    ofstream saida(output);
    int ciclo_global = 1;

    // H = LV
    const auto H_EQ_LV = stringParaMicroinstrucao("00010100100000000000101");
    // H = H + 1 
    const auto H_EQ_H_MAIS_1 = stringParaMicroinstrucao("00111001100000000000000");
    // MAR = H; READ
    const auto MAR_EQ_H_RD = stringParaMicroinstrucao("00111000000000001010000");        
    // MAR = SP; SP = SP + 1; WRITE
    const auto MAR_SP_EQ_SP_MAIS_1_WR = stringParaMicroinstrucao("00110101000001001100100");
    // TOS = MDR
    const auto TOS_EQ_MDR = stringParaMicroinstrucao("00110100001000000000000");
    // MAR = SP; SP = SP + 1
    const auto MAR_SP_EQ_SP_MAIS_1 = stringParaMicroinstrucao("00110101000001001000100");
    // MDR = TOS; WRITE
    const auto MDR_EQ_TOS_WR = stringParaMicroinstrucao("00010100000000010100111");
    // MDR = TOS; WRITE (com MBR/H já carregado)
    const auto MDR_TOS_EQ_H_WR = stringParaMicroinstrucao("00111000001000010100000");                                     

    string linha_instrucao;
    
    saida << "=== SIMULAÇÃO MIC-1 ===" << endl;
    saida << "Arquivo de instruções: " << arquivoInstrucoes << endl;
    saida << "==========================================" << endl << endl;

    // 5. Ler e executar instruções de alto nível
    while (getline(arquivoInstrucoesAltoNivel, linha_instrucao)) {
        stringstream ss(linha_instrucao);
        string comando;
        ss >> comando;

        vector<array<int, 23>> micro_instrucoes_para_executar;

        saida << "========================================================" << endl;
        saida << "EXECUTANDO INSTRUÇÃO: " << linha_instrucao << endl;
        saida << "========================================================" << endl;

        if (comando == "ILOAD") {
            int x;
            ss >> x;
            if (x < 0) {
                saida << "!!!! ERRO: Argumento inválido para ILOAD: " << x << " !!!!\n";
                continue;
            }
            if (regs.SP + 1 >= (int)MEM.size()) { // verificacao se haveria estouro de pilha
                saida << "!!!! ERRO: ILOAD causaria estouro de pilha: SP(" << regs.SP 
                      << ") + 1 = " << (regs.SP + 1) 
                      << " (Limite: " << (MEM.size()-1) << ") !!!!" << endl;
                continue;
            }
            if (regs.LV + x < 0 || regs.LV + x >= (int)MEM.size()) { // verificacao de endereco valido
                saida << "!!!! ERRO: Endereço inválido para ILOAD: LV(" << regs.LV 
                      << ") + " << x << " = " << (regs.LV + x) 
                      << " (Limite: 0 a " << (MEM.size()-1) << ") !!!!" << endl;
                continue;
            }

            //microinstrucoes do iload
            micro_instrucoes_para_executar.push_back(H_EQ_LV);
            for (int i = 0; i < x; ++i) {
                micro_instrucoes_para_executar.push_back(H_EQ_H_MAIS_1);
            }
            micro_instrucoes_para_executar.push_back(MAR_EQ_H_RD);
            micro_instrucoes_para_executar.push_back(MAR_SP_EQ_SP_MAIS_1_WR);
            micro_instrucoes_para_executar.push_back(TOS_EQ_MDR);

        } else if (comando == "DUP") {
            //microinstrucoes do dup
             if (regs.SP + 1 >= (int)MEM.size()) { // verificacao se haveria estouro de pilha
                saida << "!!!! ERRO: DUP causaria estouro de pilha: SP(" << regs.SP 
                      << ") + 1 = " << (regs.SP + 1) 
                      << " (Limite: " << (MEM.size()-1) << ") !!!!" << endl;
                continue;
            }
            micro_instrucoes_para_executar.push_back(MAR_SP_EQ_SP_MAIS_1);
            micro_instrucoes_para_executar.push_back(MDR_EQ_TOS_WR);

        } else if (comando == "BIPUSH") {
            //microinstrucoes do bipush
            string byte_arg;
            ss >> byte_arg;
            if (byte_arg.empty()) {
                saida << "!!!! ERRO: Argumento ausente para BIPUSH !!!!\n";
                continue;
            }
            // Validação do argumento 
            if (byte_arg.length() > 8 || byte_arg.find_first_not_of("01") != string::npos) {
                cerr << "ERRO: Argumento inválido para BIPUSH: " << byte_arg << endl;
                saida << "!!!! ERRO: Argumento inválido para BIPUSH: " << byte_arg << " !!!!\n";
                continue;
            }

            // Adiciona zeros à esquerda se necessário
            while (byte_arg.length() < 8) {
                byte_arg = "0" + byte_arg;
            }

            micro_instrucoes_para_executar.push_back(MAR_SP_EQ_SP_MAIS_1);

            // Criar microinstrução FETCh
            string fetch_micro_str = byte_arg + "000000000110000";
            micro_instrucoes_para_executar.push_back(stringParaMicroinstrucao(fetch_micro_str));

            micro_instrucoes_para_executar.push_back(MDR_TOS_EQ_H_WR);
        }

        // 6. Converter vector<array<int,23>> para vector<SinaisCompletos23>
        vector<SinaisCompletos23> instrucoesBinarias;
        for (const auto& micro_array : micro_instrucoes_para_executar) {
            SinaisCompletos23 instrucao;
            instrucao.bits = micro_array;
            instrucoesBinarias.push_back(instrucao);
        }

        // 7. Executar as microinstruções geradas
        if (!instrucoesBinarias.empty()) {
            executarMicroinstrucoes(instrucoesBinarias, regs, MEM, saida, ciclo_global);
        }

        // Incremento seguro do PC
        if (regs.PC < (int)MEM.size() - 1) {
            regs.PC++;
        } else {
            saida << "!!!! AVISO: PC atingiu o limite máximo da memória !!!!" << endl;
            break;
        }
    }

    saida << "=== FIM DA SIMULAÇÃO ===" << endl;
     saida << "Ciclos executados: " << (ciclo_global - 1) << endl;
    saida << "Estado final do PC: " << regs.PC << endl;
    saida.close();
    arquivoInstrucoesAltoNivel.close();

    cout << "Simulação concluída. Verifique " << output << endl;
}
