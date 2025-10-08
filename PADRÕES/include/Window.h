#ifndef WINDOW_H
#define WINDOW_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <vector>
#include <thread>
#include <mutex>
#include <memory>

#include "Hidrometro.h"
#include "Background.h"
#include "SliderVazao.h"
#include "ResourceManager.h"

#define WM_USER_REDESENHAR (WM_USER + 1)

class Window {
private:

    HWND m_hwnd;
    HINSTANCE m_hInstance;
    std::vector<Hidrometro> m_hidrometros;
    std::mutex m_mutexDados;
    std::vector<std::thread> m_threadsDeTrabalho;

    std::unique_ptr<Background> m_background;
    std::unique_ptr<SliderVazao> m_slider;
    std::unique_ptr<ResourceManager> m_resourceManager;

    double m_tempoInicial;

    static LRESULT CALLBACK windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    LRESULT handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

    void initializeComponents();
    void update();
    void render();

public:
    Window();
    ~Window();

    bool create();
    int run();
    void iniciarSimuladores();
};

#endif // WINDOW_H