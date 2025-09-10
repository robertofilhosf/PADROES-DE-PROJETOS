#ifndef HIDROMETRO_H
#define HIDROMETRO_H

#include <string>
#include <vector>

class Hidrometro {
private:
    int id;
    std::string localizacao;
    double consumoTotal;
    double vazaoAtual;
    bool vazamentoDetectado;
    std::vector<double> historicoConsumo;

public:
    Hidrometro(int id, const std::string& localizacao);
    
    void registrarConsumo(double consumo);
    void detectarVazamento();
    void atualizarVazao(double vazao);
    
    // Getters
    int getId() const;
    std::string getLocalizacao() const;
    double getConsumoTotal() const;
    double getVazaoAtual() const;
    bool isVazamentoDetectado() const;
    std::vector<double> getHistoricoConsumo() const;
    
    void exibirStatus() const;
};

#endif