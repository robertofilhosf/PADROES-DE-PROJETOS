#include "SimuladorFacade.h"
#include <iostream>
#include <windows.h>

int main() {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Iniciando Sistema Hidrométrico..." << std::endl;

    SimuladorFacade simulador;

    if (!simulador.inicializar()) {
        MessageBox(nullptr, L"Falha ao inicializar o sistema", L"Erro", MB_ICONERROR);
        return -1;
    }

    std::cout << "Sistema iniciado com sucesso!" << std::endl;
    simulador.executar();

    return 0;
}
