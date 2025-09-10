#include "LeitorArquivo.h"
#include <fstream>
#include <sstream>
#include <iostream>

std::vector<DadosConsumo> LeitorArquivo::lerDados(const std::string& arquivo) {
    std::vector<DadosConsumo> dados;
    std::ifstream file(arquivo);
    
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir arquivo: " << arquivo << std::endl;
        return dados;
    }
    
    std::string linha;
    while (std::getline(file, linha)) {
        if (linha.empty() || linha[0] == '#') continue;
        
        std::istringstream iss(linha);
        DadosConsumo dado;
        char delimiter;
        
        if (iss >> dado.idHidrometro >> delimiter >> dado.consumo >> delimiter 
                >> dado.vazao >> delimiter) {
            std::getline(iss, dado.timestamp);
            dados.push_back(dado);
        }
    }
    
    file.close();
    return dados;
}

void LeitorArquivo::processarDados(Hidrometro& hidrometro, const std::vector<DadosConsumo>& dados) {
    for (const auto& dado : dados) {
        hidrometro.registrarConsumo(dado.consumo);
        hidrometro.atualizarVazao(dado.vazao);
    }
}