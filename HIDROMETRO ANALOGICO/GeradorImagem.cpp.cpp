#include "GeradorImagem.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

void GeradorImagem::gerarImagemHidrometro(const Hidrometro& hidrometro, const std::string& nomeArquivo) {
    std::ofstream svgFile(nomeArquivo);
    
    if (!svgFile.is_open()) {
        std::cerr << "Erro ao criar arquivo SVG: " << nomeArquivo << std::endl;
        return;
    }
    
    // Cabeçalho SVG
    svgFile << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    svgFile << "<svg width=\"800\" height=\"600\" xmlns=\"http://www.w3.org/2000/svg\">\n";
    
    // Fundo
    svgFile << "<rect width=\"100%\" height=\"100%\" fill=\"#f0f8ff\"/>\n";
    
    // Título
    svgFile << "<text x=\"400\" y=\"50\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"24\" fill=\"#0066cc\">\n";
    svgFile << "  SIMULADOR DE HIDRÔMETRO ANALÓGICO\n";
    svgFile << "</text>\n";
    
    // Círculo do hidrômetro
    svgFile << "<circle cx=\"400\" cy=\"250\" r=\"120\" fill=\"white\" stroke=\"#0066cc\" stroke-width=\"3\"/>\n";
    
    // Mostrador analógico (ponteiro)
    double angle = (hidrometro.getVazaoAtual() / 10.0) * 270.0; // 0-10 L/min -> 0-270 graus
    if (angle > 270) angle = 270;
    
    double rad = (angle - 135) * M_PI / 180.0; // -135 para começar na esquerda
    int x2 = 400 + 80 * cos(rad);
    int y2 = 250 + 80 * sin(rad);
    
    svgFile << "<line x1=\"400\" y1=\"250\" x2=\"" << x2 << "\" y2=\"" << y2 
            << "\" stroke=\"red\" stroke-width=\"3\" stroke-linecap=\"round\"/>\n";
    
    // Marcas do mostrador
    for (int i = 0; i <= 10; i += 2) {
        double markAngle = (i / 10.0 * 270) - 135;
        double rad1 = markAngle * M_PI / 180.0;
        double rad2 = (markAngle + 5) * M_PI / 180.0;
        
        int x1 = 400 + 100 * cos(rad1);
        int y1 = 250 + 100 * sin(rad1);
        int x2 = 400 + 110 * cos(rad1);
        int y2 = 250 + 110 * sin(rad1);
        
        svgFile << "<line x1=\"" << x1 << "\" y1=\"" << y1 << "\" x2=\"" << x2 
                << "\" y2=\"" << y2 << "\" stroke=\"#0066cc\" stroke-width=\"2\"/>\n";
        svgFile << "<text x=\"" << (400 + 125 * cos(rad1)) << "\" y=\"" 
                << (250 + 125 * sin(rad1)) << "\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"12\">"
                << i << " L/min</text>\n";
    }
    
    // Informações do hidrômetro
    svgFile << "<rect x=\"200\" y=\"400\" width=\"400\" height=\"150\" fill=\"white\" stroke=\"#0066cc\" stroke-width=\"2\" rx=\"10\"/>\n";
    
    svgFile << "<text x=\"400\" y=\"430\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"18\" fill=\"#0066cc\">\n";
    svgFile << "  HIDRÔMETRO " << hidrometro.getId() << "\n";
    svgFile << "</text>\n";
    
    svgFile << "<text x=\"250\" y=\"460\" font-family=\"Arial\" font-size=\"14\" fill=\"#333\">\n";
    svgFile << "  Localização: " << hidrometro.getLocalizacao() << "\n";
    svgFile << "</text>\n";
    
    svgFile << "<text x=\"250\" y=\"490\" font-family=\"Arial\" font-size=\"14\" fill=\"#333\">\n";
    svgFile << "  Consumo Total: " << std::fixed << std::setprecision(2) 
            << hidrometro.getConsumoTotal() << " m³\n";
    svgFile << "</text>\n";
    
    svgFile << "<text x=\"250\" y=\"520\" font-family=\"Arial\" font-size=\"14\" fill=\"#333\">\n";
    svgFile << "  Vazão Atual: " << hidrometro.getVazaoAtual() << " L/min\n";
    svgFile << "</text>\n";
    
    // Status de vazamento
    std::string statusColor = hidrometro.isVazamentoDetectado() ? "red" : "green";
    std::string statusText = hidrometro.isVazamentoDetectado() ? "VAZAMENTO DETECTADO!" : "Status: Normal";
    
    svgFile << "<text x=\"400\" y=\"550\" text-anchor=\"middle\" font-family=\"Arial\" font-size=\"16\" fill=\"" 
            << statusColor << "\" font-weight=\"bold\">\n";
    svgFile << "  " << statusText << "\n";
    svgFile << "</text>\n";
    
    svgFile << "</svg>\n";
    svgFile.close();
    
    std::cout << "Imagem gerada: " << nomeArquivo << std::endl;
}