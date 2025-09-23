#include "Window.h"
#include "Constants.h"
#include "Hidrometro.h"
#include "Background.h"
#include "SliderVazao.h"
#include "ResourceManager.h"
#include <windowsx.h>

Window::Window()
    : m_hwnd(nullptr), m_hInstance(GetModuleHandle(nullptr)),
      m_hidrometro(nullptr), m_background(nullptr), m_slider(nullptr),
      m_resourceManager(nullptr) {

    m_tempoInicial = GetTickCount() / 1000.0;
}

Window::~Window() {
    delete m_hidrometro;
    delete m_background;
    delete m_slider;
    delete m_resourceManager;

    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
    UnregisterClass(Constants::CLASS_NAME(), m_hInstance);
}

bool Window::create() {
    WNDCLASS wc = {};
    wc.lpfnWndProc = Window::windowProc;
    wc.hInstance = m_hInstance;
    wc.lpszClassName = Constants::CLASS_NAME();
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wc.style = CS_HREDRAW | CS_VREDRAW;

    if (!RegisterClass(&wc)) {
        MessageBox(nullptr, L"Falha ao registrar classe da janela", L"Erro", MB_ICONERROR);
        return false;
    }

    RECT rect = {0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT};
    AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

    m_hwnd = CreateWindow(
        Constants::CLASS_NAME(),
        Constants::WINDOW_TITLE(),
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        rect.right - rect.left,
        rect.bottom - rect.top,
        nullptr, nullptr, m_hInstance, this
    );

    if (!m_hwnd) {
        MessageBox(nullptr, L"Falha ao criar janela", L"Erro", MB_ICONERROR);
        return false;
    }

    initializeComponents();
    return true;
}

void Window::initializeComponents() {
    m_resourceManager = new ResourceManager();
    m_resourceManager->loadResources();

    m_background = new Background();

    // Centralizar hidrômetro principal
    int hidrometroX = Constants::WINDOW_WIDTH / 2;
    int hidrometroY = Constants::WINDOW_HEIGHT / 2 - 100;
    m_hidrometro = new Hidrometro(hidrometroX, hidrometroY);

    // Slider na parte inferior com vazão controlada
    int sliderWidth = 600;
    int sliderX = (Constants::WINDOW_WIDTH - sliderWidth) / 2;
    int sliderY = Constants::WINDOW_HEIGHT - 120;
    m_slider = new SliderVazao(sliderX, sliderY, sliderWidth, 30);
}

int Window::run() {
    if (!m_hwnd && !create()) {
        return -1;
    }

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    MSG msg = {};
    while (true) {
        if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) break;

            TranslateMessage(&msg);
            DispatchMessage(&msg);
        } else {
            update();
            InvalidateRect(m_hwnd, nullptr, FALSE);
            Sleep(50); // ~20 FPS
        }
    }

    return (int)msg.wParam;
}

LRESULT CALLBACK Window::windowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    Window* pWindow = nullptr;

    if (uMsg == WM_NCCREATE) {
        CREATESTRUCT* pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        pWindow = reinterpret_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        if (pWindow) pWindow->m_hwnd = hwnd;
    } else {
        pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
    }

    return pWindow ? pWindow->handleMessage(uMsg, wParam, lParam)
                   : DefWindowProc(hwnd, uMsg, wParam, lParam);
}

LRESULT Window::handleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;

        case WM_PAINT:
            render();
            return 0;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MOUSEMOVE: {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);
            if (m_slider && m_slider->handleMouseEvent(uMsg, mouseX, mouseY)) {
                if (m_hidrometro) {
                    m_hidrometro->setVazao(m_slider->getValor());
                }
                InvalidateRect(m_hwnd, nullptr, FALSE);
            }
            return 0;
        }

        case WM_SIZE:
            InvalidateRect(m_hwnd, nullptr, TRUE);
            return 0;

        case WM_KEYDOWN:
            if (wParam == VK_ESCAPE) PostQuitMessage(0);
            return 0;

        default:
            return DefWindowProc(m_hwnd, uMsg, wParam, lParam);
    }
}

void Window::update() {
    double tempoAtual = GetTickCount() / 1000.0;
    if (m_hidrometro) {
        m_hidrometro->update(tempoAtual);
    }
}

void Window::render() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);

    // Double buffering
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);
    HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

    // Desenhar fundo
    if (m_background) {
        m_background->draw(memDC, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT);

        // Efeito de água
        double tempoAtual = GetTickCount() / 1000.0;
        RECT waterRect = {0, Constants::WINDOW_HEIGHT - 200, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT};
        m_background->drawWaterEffect(memDC, waterRect, tempoAtual);
    }

    // Desenhar título do sistema
    HFONT titleFont = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    HGDIOBJ oldFont = SelectObject(memDC, titleFont);
    SetTextColor(memDC, RGB(255, 255, 255));
    SetBkMode(memDC, TRANSPARENT);

    RECT titleRect = {0, 20, Constants::WINDOW_WIDTH, 80};
    DrawText(memDC, L"SIMULADOR DE HIDRÔMETRO", -1, &titleRect, DT_CENTER);

    SelectObject(memDC, oldFont);
    DeleteObject(titleFont);

    // Desenhar componentes
    if (m_hidrometro) {
        m_hidrometro->draw(memDC);
    }

    if (m_slider) {
        m_slider->draw(memDC);
    }

    // Desenhar informações adicionais
    HFONT infoFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                               DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                               CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    SelectObject(memDC, infoFont);

    if (m_hidrometro) {
        double volumeTotal = m_hidrometro->getVolumeTotal();
        double vazaoAtual = m_hidrometro->getVazaoAtual() * 1000.0; // Converter para litros/segundo

        std::wstring infoText = L"Volume Total: " + std::to_wstring(volumeTotal).substr(0, 8) + L" m³ | " +
                               L"Vazão Atual: " + std::to_wstring(vazaoAtual).substr(0, 6) + L" litros/segundo";

        RECT infoRect = {50, Constants::WINDOW_HEIGHT - 180, Constants::WINDOW_WIDTH - 50, Constants::WINDOW_HEIGHT - 150};
        DrawText(memDC, infoText.c_str(), -1, &infoRect, DT_CENTER);
    }

    SelectObject(memDC, oldFont);
    DeleteObject(infoFont);

    // Copiar para tela
    BitBlt(hdc, 0, 0, Constants::WINDOW_WIDTH, Constants::WINDOW_HEIGHT, memDC, 0, 0, SRCCOPY);

    // Limpar recursos
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);

    EndPaint(m_hwnd, &ps);
}