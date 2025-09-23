#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <bitset> 
#include <string>

using namespace std;

vector<string> lerArquivo(const string& nomeArquivo, const string &nomeArquivoSaida);
void writeLineInFile(string nomeArquivo, string linha);
string paraBinario32bits(int valor);