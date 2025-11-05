#include <iostream>
#include <thread>
#include <chrono>
#include <windows.h>
#include "SHAFacade.h"

void mostrarMenu() {
    std::cout << "\n=== Sistema Hidrometrico Automatico (SHA) ===\n";
    std::cout << "1. Configurar Simulador\n";
    std::cout << "2. Criar e Iniciar SHA (GUI)\n";
    std::cout << "3. Finalizar SHA\n";
    std::cout << "4. Modificar Vazao\n";
    std::cout << "5. Habilitar/Desabilitar Geracao de Imagens\n";
    std::cout << "6. Alterar vazao por porcentagem (0% a 100%)\n";
    std::cout << "7. Sair\n";
    std::cout << "Escolha uma opcao: ";
}

int main() {
    // Garante que o console apareça mesmo em builds GUI
    #ifdef _WIN32
    AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);
    freopen_s(&fp, "CONIN$", "r", stdin);
    #endif

    SHAFacade& sha = SHAFacade::getInstance();

    int opcao = 0;
    int intervalo = 1000;
    bool gerarImagem = true;
    double vazao = 2.0;

    while (true) {
        mostrarMenu();
        std::cin >> opcao;

        switch (opcao) {
            case 1:
                std::cout << "Digite o intervalo de tempo (ms): ";
                std::cin >> intervalo;
                std::cout << "Gerar imagens (1 = sim, 0 = nao): ";
                std::cin >> gerarImagem;
                std::cout << "Digite a vazao inicial: ";
                std::cin >> vazao;
                sha.configSimuladorSHA(intervalo, gerarImagem, vazao);
                std::cout << "Configuracao aplicada.\n";
                break;

            case 2:
                if (!sha.estaExecutando()) {
                    sha.criaSHA();
                    std::cout << "SHA iniciado (janela aberta).\n";
                } else {
                    std::cout << "Ja existe uma instancia em execucao.\n";
                }
                break;

            case 3:
                if (sha.estaExecutando()) {
                    sha.finalizaSHA();
                    std::cout << "SHA finalizado.\n";
                } else {
                    std::cout << "Nenhum SHA em execucao.\n";
                }
                break;

            case 4:
                std::cout << "Digite nova vazao: ";
                std::cin >> vazao;
                sha.modificaVazaoSHA(vazao);
                std::cout << "Vazao alterada.\n";
                break;

            case 5: {
                int escolha;
                std::cout << "1 = habilitar, 0 = desabilitar geracao de imagens: ";
                std::cin >> escolha;
                sha.habilitaGeracaoImagemSHA(escolha == 1);
                std::cout << "Estado da geracao de imagens atualizado.\n";
                break;
            }
            case 6:
                double porcentagem;
                std::cout << "Informe a porcentagem de abertura da vazao (0 a 100): ";
                std::cin >> porcentagem;
                sha.alteraVazaoPorcentagem(porcentagem);
                break;

            case 7:
                std::cout << "Encerrando programa...\n";
                if (sha.estaExecutando()) sha.finalizaSHA();
                return 0;


            default:
                std::cout << "Opcao invalida.\n";
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    return 0;
}
