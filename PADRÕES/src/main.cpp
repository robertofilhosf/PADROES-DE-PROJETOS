#include "Window.h"
#include <windows.h>
#include <iostream>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);

    std::cout << "Iniciando Sistema Hidrométrico..." << std::endl;

    try {
        Window window;

        if (window.create()) {
            std::cout << "Sistema iniciado com sucesso!" << std::endl;
            return window.run();
        } else {
            MessageBox(nullptr, L"Falha ao inicializar o sistema", L"Erro", MB_ICONERROR);
            return -1;
        }
    }
    catch (const std::exception& e) {
        std::string error = "Exceção: ";
        error += e.what();
        MessageBoxA(nullptr, error.c_str(), "Erro", MB_ICONERROR);
        return -1;
    }
    catch (...) {
        MessageBox(nullptr, L"Exceção desconhecida", L"Erro", MB_ICONERROR);
        return -1;
    }
}

// Ponto de entrada alternativo para console
int main() {
    return WinMain(GetModuleHandle(nullptr), nullptr, GetCommandLineA(), SW_SHOW);
}