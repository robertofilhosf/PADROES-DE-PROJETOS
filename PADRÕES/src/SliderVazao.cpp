#include "SliderVazao.h"
#include "Constants.h"
#include <algorithm>
#include <string>

SliderVazao::SliderVazao(int x, int y, int width, int height)
    : m_x(x), m_y(y), m_width(width), m_height(height),
      m_valorMin(Constants::VAZAO_MINIMA),
      m_valorMax(Constants::VAZAO_MAXIMA),
      m_valorAtual(Constants::VAZAO_MINIMA),
      m_arrastando(false) {
    
    m_railBrush = CreateSolidBrush(RGB(200, 200, 200));
    m_thumbBrush = CreateSolidBrush(Constants::SLIDER_COLOR);
    m_hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                         DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                         CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    m_smallFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                             DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                             CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
}

SliderVazao::~SliderVazao() {
    DeleteObject(m_railBrush);
    DeleteObject(m_thumbBrush);
    DeleteObject(m_hFont);
    DeleteObject(m_smallFont);
}

void SliderVazao::draw(HDC hdc) {
    RECT railRect = {m_x, m_y + m_height / 2 - 5, m_x + m_width, m_y + m_height / 2 + 5};
    FillRect(hdc, &railRect, m_railBrush);

    int posX = valorParaPosicao(m_valorAtual);
    RECT thumbRect = {posX - 10, m_y, posX + 10, m_y + m_height};
    FillRect(hdc, &thumbRect, m_thumbBrush);

    HGDIOBJ oldFont = SelectObject(hdc, m_hFont);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    double vazaoDecilitros = m_valorAtual * 10000.0;
    std::wstring texto = L"Vazão: " + std::to_wstring(vazaoDecilitros).substr(0, 6) + L" dL/s";
    RECT textRect = {m_x, m_y - 30, m_x + m_width, m_y};
    DrawText(hdc, texto.c_str(), -1, &textRect, DT_CENTER);

    // Legenda das escalas
    SelectObject(hdc, m_smallFont);
    std::wstring minText = L"1 dL/s";
    std::wstring maxText = L"10 dL/s";
    RECT minRect = {m_x - 30, m_y + m_height + 5, m_x + 30, m_y + m_height + 25};
    RECT maxRect = {m_x + m_width - 30, m_y + m_height + 5, m_x + m_width + 30, m_y + m_height + 25};
    DrawText(hdc, minText.c_str(), -1, &minRect, DT_CENTER);
    DrawText(hdc, maxText.c_str(), -1, &maxRect, DT_CENTER);

    SelectObject(hdc, oldFont); 
}

bool SliderVazao::handleMouseEvent(UINT message, int mouseX, int mouseY) {
    switch (message) {
        case WM_LBUTTONDOWN:
            if (mouseX >= m_x && mouseX <= m_x + m_width &&
                mouseY >= m_y && mouseY <= m_y + m_height) {
                m_arrastando = true;
                setValor(posicaoParaValor(mouseX));
                return true;
            }
            break;

        case WM_LBUTTONUP:
            if (m_arrastando) {
                m_arrastando = false;
                return true; 
            }
            break;

        case WM_MOUSEMOVE:
            if (m_arrastando) {
                setValor(posicaoParaValor(mouseX));
                return true;
            }
            break;
    }
    return false;
}

bool SliderVazao::pontoDentroSlider(int x, int y) {
    return (x >= m_x && x <= m_x + m_width &&
            y >= m_y && y <= m_y + m_height);
}

void SliderVazao::setValor(double valor) {
    m_valorAtual = std::clamp(valor, m_valorMin, m_valorMax);
}

int SliderVazao::valorParaPosicao(double valor) {
    if (m_valorMax <= m_valorMin) return m_x;
    double ratio = (valor - m_valorMin) / (m_valorMax - m_valorMin);
    return m_x + static_cast<int>(ratio * m_width);
}

double SliderVazao::posicaoParaValor(int pos) {
    double ratio = static_cast<double>(pos - m_x) / m_width;
    return m_valorMin + ratio * (m_valorMax - m_valorMin);
}