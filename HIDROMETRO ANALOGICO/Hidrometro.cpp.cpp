#include "Hidrometro.h"
#include <iostream>
#include <iomanip>

Hidrometro::Hidrometro(int id, const std::string& localizacao) 
    : id(id), localizacao(localizacao), consumoTotal(0), 
      vazaoAtual(0), vazamentoDetectado(false) {}

void Hidrometro::registrarConsumo(double consumo) {
    consumoTotal += consumo;
    historicoConsumo.push_back(consumo);
    
    // Detectar vazamento se consumo for anormal
    if (consumo > 50.0 && vazaoAtual < 1.0) {
        vazamentoDetectado = true;
    }
}

void Hidrometro::detectarVazamento() {
    if (vazaoAtual > 0 && consumoTotal < 0.1) {
        vazamentoDetectado = true;
    }
}

void Hidrometro::atualizarVazao(double vazao) {
    vazaoAtual = vazao;
    detectarVazamento();
}

int Hidrometro::getId() const { return id; }
std::string Hidrometro::getLocalizacao() const { return localizacao; }
double Hidrometro::getConsumoTotal() const { return consumoTotal; }
double Hidrometro::getVazaoAtual() const { return vazaoAtual; }
bool Hidrometro::isVazamentoDetectado() const { return vazamentoDetectado; }
std::vector<double> Hidrometro::getHistoricoConsumo() const { return historicoConsumo; }

void Hidrometro::exibirStatus() const {
    std::cout << "=== HIDRÔMETRO " << id << " ===\n";
    std::cout << "Localização: " << localizacao << "\n";
    std::cout << "Consumo Total: " << std::fixed << std::setprecision(2) 
              << consumoTotal << " m³\n";
    std::cout << "Vazão Atual: " << vazaoAtual << " L/min\n";
    std::cout << "Vazamento: " << (vazamentoDetectado ? "DETECTADO" : "Normal") << "\n";
    std::cout << "=========================\n";
}