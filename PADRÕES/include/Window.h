#ifndef WINDOW_H
#define WINDOW_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// Forward declarations
class Hidrometro;
class Background;
class SliderVazao;
class ResourceManager;

class Window {
private:
    HWND m_hwnd;
    HINSTANCE m_hInstance;

    Hidrometro* m_hidrometro;
    Background* m_background;
    SliderVazao* m_slider;
    ResourceManager* m_resourceManager;

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
};

#endif // WINDOW_H