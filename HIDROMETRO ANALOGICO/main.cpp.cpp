#include <iostream>
#include <vector>
#include "Hidrometro.h"
#include "LeitorArquivo.h"
#include "GeradorImagem.h"

int main() {
    std::cout << "=== SIMULADOR DE HIDRÔMETRO ANALÓGICO ===\n\n";
    
    // Criar hidrômetro
    Hidrometro hidrometro(101, "Rua das Flores, 123 - Centro");
    
    // Ler dados do arquivo
    std::string arquivoDados = "dados_consumo.txt";
    std::vector<DadosConsumo> dados = LeitorArquivo::lerDados(arquivoDados);
    
    if (dados.empty()) {
        std::cout << "Nenhum dado encontrado. Usando dados de exemplo...\n";
        // Dados de exemplo
        hidrometro.registrarConsumo(15.5);
        hidrometro.atualizarVazao(2.3);
        hidrometro.registrarConsumo(12.8);
        hidrometro.atualizarVazao(1.8);
        hidrometro.registrarConsumo(8.2);
        hidrometro.atualizarVazao(0.5);
    } else {
        // Processar dados do arquivo
        LeitorArquivo::processarDados(hidrometro, dados);
    }
    
    // Exibir status no console
    hidrometro.exibirStatus();
    
    // Gerar imagem SVG
    std::string arquivoImagem = "hidrometro_simulacao.svg";
    GeradorImagem::gerarImagemHidrometro(hidrometro, arquivoImagem);
    
    std::cout << "\nSimulação concluída. Verifique a imagem gerada: " << arquivoImagem << std::endl;
    
    return 0;
}