#include "Window.h"
#include "Constants.h"
#include "Hidrometro.h"
#include "Background.h"
#include "SliderVazao.h"
#include "ResourceManager.h"
#include <windowsx.h>
#include <chrono>
#include <string>

void funcao_worker_hidrometro(Hidrometro* hidrometro, std::mutex* mutex, HWND hwnd) {
    // MessageBoxA(hwnd, "Checkpoint 3: A thread de trabalho está rodando!", "Depuração", MB_OK);
    try {
        // O loop infinito da simulação agora está dentro de um bloco 'try'
        while (true) {
            double tempoAtual = GetTickCount() / 1000.0;
            
            {
                std::lock_guard<std::mutex> lock(*mutex);
                // A causa do travamento provavelmente está nesta chamada -> update()
                if (hidrometro) {
                    hidrometro->update(tempoAtual);
                }
            }

            PostMessage(hwnd, WM_USER_REDESENHAR, 0, 0);

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
    catch (const std::exception& e) {
        // Se um erro padrão do C++ ocorrer (ex: bad_alloc), ele será capturado.
        std::string error_msg = "Uma exceção ocorreu na thread de simulação:\n\n";
        error_msg += e.what();
        MessageBoxA(hwnd, error_msg.c_str(), "Erro Crítico na Simulação", MB_ICONERROR | MB_OK);
    }
    catch (...) {
        // Se ocorrer qualquer outro tipo de erro (ex: acesso a ponteiro nulo).
        MessageBoxA(hwnd, "Um erro desconhecido e fatal ocorreu na thread de simulação.", "Erro Crítico na Simulação", MB_ICONERROR | MB_OK);
    }
}

Window::Window()
    : m_hwnd(nullptr), m_hInstance(GetModuleHandle(nullptr)) {
    // A inicialização do unique_ptr e vetores é feita automaticamente
}

Window::~Window() {
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
    m_resourceManager = std::make_unique<ResourceManager>();
    m_resourceManager->loadResources();

    m_background = std::make_unique<Background>();

    // Centralizar hidrômetro principal
    int hidrometroX = Constants::WINDOW_WIDTH / 2;
    int hidrometroY = Constants::WINDOW_HEIGHT / 2 - 100;
    m_hidrometros.emplace_back(hidrometroX, hidrometroY);

    // Slider na parte inferior com vazão controlada
    int sliderWidth = 600;
    int sliderX = (Constants::WINDOW_WIDTH - sliderWidth) / 2;
    int sliderY = Constants::WINDOW_HEIGHT - 120;
    m_slider = std::make_unique<SliderVazao>(sliderX, sliderY, sliderWidth, 30);
}

void Window::iniciarSimuladores() {
    if (!m_hwnd) return;

    // MessageBoxA(m_hwnd, "Checkpoint 2: Prestes a lançar as threads.", "Depuração", MB_OK);

    for (size_t i = 0; i < m_hidrometros.size(); ++i) {
        m_threadsDeTrabalho.emplace_back(
            funcao_worker_hidrometro,
            &m_hidrometros[i],           // Ponteiro para o Hidrômetro
            &m_mutexDados,              // Ponteiro para o mutex compartilhado
            m_hwnd                     // Handle da janela para enviar mensagens
        );
    }

    for (auto& th : m_threadsDeTrabalho) {
        th.detach();    // Threads rodam livres em Background
    }
}

int Window::run() {

    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
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

        case WM_USER_REDESENHAR:
            InvalidateRect(m_hwnd, nullptr, FALSE);
            return 0;

        case WM_PAINT:
            render();
            return 0;

        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MOUSEMOVE: {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);

            // A chamada ao handleMouseEvent do slider está correta
            if (m_slider && m_slider->handleMouseEvent(uMsg, mouseX, mouseY)) {
                
                // A atualização do hidrômetro (protegida) também está correta
                {
                    std::lock_guard<std::mutex> lock(m_mutexDados);
                    if (!m_hidrometros.empty()) {
                        m_hidrometros[0].setVazao(m_slider->getValor());
                    }
                }

                // --- CORREÇÃO CRÍTICA ADICIONADA DE VOLTA ---
                // Precisamos forçar um redesenho IMEDIATAMENTE após a interação do usuário
                // para que a resposta visual seja instantânea.
                InvalidateRect(m_hwnd, NULL, FALSE);
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

// void Window::update() {
//     double tempoAtual = GetTickCount() / 1000.0;
//     if (!m_hidrometros.empty()) {
//         m_hidrometros->update(tempoAtual);
//     }
// }

void Window::render() {
    PAINTSTRUCT ps;
    HDC hdc = BeginPaint(m_hwnd, &ps);

    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);
    int width = clientRect.right - clientRect.left;
    int height = clientRect.bottom - clientRect.top;

    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
    HGDIOBJ oldBitmap = SelectObject(memDC, memBitmap);

    {
        std::lock_guard<std::mutex> lock(m_mutexDados);

        if (m_background) {
            m_background->draw(memDC, width, height);

            double tempoAtual = GetTickCount() / 1000.0;
            RECT waterRect = {0, height - 200, width, height};
            m_background->drawWaterEffect(memDC, waterRect, tempoAtual);
        }

        // Título Principal 
        {
            HFONT titleFont = CreateFont(32, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                                         DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                         CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
            
            HGDIOBJ oldFont = SelectObject(memDC, titleFont); 
            SetTextColor(memDC, RGB(255, 255, 255));
            SetBkMode(memDC, TRANSPARENT);

            RECT titleRect = {0, 20, width, 80};
            DrawText(memDC, L"SIMULADOR DE HIDRÔMETRO", -1, &titleRect, DT_CENTER | DT_VCENTER);

            SelectObject(memDC, oldFont);
            DeleteObject(titleFont);
        }

        // Componentes
        if (!m_hidrometros.empty()) {
            m_hidrometros[0].draw(memDC);
        }

        if (m_slider) {
            m_slider->draw(memDC);
        }

        // Informações Adicionais
        if (!m_hidrometros.empty()) {
                HFONT infoFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                            CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");

                HGDIOBJ oldFont = SelectObject(memDC, infoFont);
                SetTextColor(memDC, RGB(255, 255, 255));

                double volumeTotal = m_hidrometros[0].getVolumeTotal();
                double vazaoAtual = m_hidrometros[0].getVazaoAtual() * 1000.0; // L/s

                wchar_t buffer[256];
                swprintf_s(buffer, L"Volume Total: %.3f m³ | Vazão Atual: %.2f litros/segundo", 
                           volumeTotal, vazaoAtual);

                RECT infoRect = {0, height - 180, width, height - 150};
                DrawText(memDC, buffer, -1, &infoRect, DT_CENTER);

                SelectObject(memDC, oldFont);
                DeleteObject(infoFont);
            }
        }

    BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

    // Limpeza 
    SelectObject(memDC, oldBitmap);
    DeleteObject(memBitmap);
    DeleteDC(memDC);

    EndPaint(m_hwnd, &ps);
}

void Window::setVazaoPrincipal(double vazao) {
    std::lock_guard<std::mutex> lock(m_mutexDados);
    if (!m_hidrometros.empty()) m_hidrometros[0].setVazao(vazao);
}

void Window::forcarRedesenho() {
    InvalidateRect(m_hwnd, NULL, FALSE);
}