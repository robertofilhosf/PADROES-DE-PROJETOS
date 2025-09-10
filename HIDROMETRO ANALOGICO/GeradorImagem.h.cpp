#ifndef GERADOR_IMAGEM_H
#define GERADOR_IMAGEM_H

#include <string>
#include "Hidrometro.h"

class GeradorImagem {
public:
    static void gerarImagemHidrometro(const Hidrometro& hidrometro, const std::string& nomeArquivo);
};

#endif