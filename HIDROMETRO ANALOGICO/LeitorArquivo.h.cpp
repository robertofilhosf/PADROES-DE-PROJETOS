#ifndef LEITOR_ARQUIVO_H
#define LEITOR_ARQUIVO_H

#include <string>
#include <vector>
#include "Hidrometro.h"

struct DadosConsumo {
    int idHidrometro;
    double consumo;
    double vazao;
    std::string timestamp;
};

class LeitorArquivo {
public:
    static std::vector<DadosConsumo> lerDados(const std::string& arquivo);
    static void processarDados(Hidrometro& hidrometro, const std::vector<DadosConsumo>& dados);
};

#endif