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
}

void SliderVazao::draw(HDC hdc) {
    // Desenhar trilho do slider
    HBRUSH railBrush = CreateSolidBrush(RGB(200, 200, 200));
    RECT railRect = {m_x, m_y + m_height/2 - 5, m_x + m_width, m_y + m_height/2 + 5};
    FillRect(hdc, &railRect, railBrush);
    DeleteObject(railBrush);

    // Desenhar indicador de valor atual
    int posX = valorParaPosicao(m_valorAtual);
    HBRUSH thumbBrush = CreateSolidBrush(Constants::SLIDER_COLOR);
    RECT thumbRect = {posX - 10, m_y, posX + 10, m_y + m_height};
    FillRect(hdc, &thumbRect, thumbBrush);
    DeleteObject(thumbBrush);

    // Desenhar texto com vazão CORRIGIDA (agora em dL/s)
    HFONT hFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    HGDIOBJ oldFont = SelectObject(hdc, hFont);
    SetTextColor(hdc, RGB(255, 255, 255));
    SetBkMode(hdc, TRANSPARENT);

    // CORREÇÃO: Converter m³/s para decilitros/segundo (dL/s)
    double vazaoDecilitros = m_valorAtual * 10000.0; // 1 m³ = 10.000 dL
    std::wstring texto = L"Vazão: " + std::to_wstring(vazaoDecilitros).substr(0, 6) + L" decilitros/segundo";
    RECT textRect = {m_x, m_y - 30, m_x + m_width, m_y};
    DrawText(hdc, texto.c_str(), -1, &textRect, DT_CENTER);

    // Legenda das escalas CORRIGIDA
    HFONT smallFont = CreateFont(12, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    SelectObject(hdc, smallFont);

    // CORREÇÃO: Agora 1 dL/s no mínimo
    std::wstring minText = L"1 dL/s";
    std::wstring maxText = L"10 dL/s";

    RECT minRect = {m_x - 30, m_y + m_height + 5, m_x + 30, m_y + m_height + 25};
    RECT maxRect = {m_x + m_width - 30, m_y + m_height + 5, m_x + m_width + 30, m_y + m_height + 25};

    DrawText(hdc, minText.c_str(), -1, &minRect, DT_CENTER);
    DrawText(hdc, maxText.c_str(), -1, &maxRect, DT_CENTER);

    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    DeleteObject(smallFont);
}

// ... o resto do código do slider permanece igual ...
bool SliderVazao::handleMouseEvent(UINT message, int mouseX, int mouseY) {
    switch (message) {
        case WM_LBUTTONDOWN:
            if (pontoDentroSlider(mouseX, mouseY)) {
                m_arrastando = true;
                setValor(posicaoParaValor(mouseX));
                return true;
            }
            break;

        case WM_LBUTTONUP:
            m_arrastando = false;
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

void SliderVazao::setValor(double valor) {
    if (valor < m_valorMin) {
        m_valorAtual = m_valorMin;
    } else if (valor > m_valorMax) {
        m_valorAtual = m_valorMax;
    } else {
        m_valorAtual = valor;
    }
}

bool SliderVazao::pontoDentroSlider(int x, int y) {
    int posX = valorParaPosicao(m_valorAtual);
    return (x >= posX - 15 && x <= posX + 15 &&
            y >= m_y && y <= m_y + m_height);
}

int SliderVazao::valorParaPosicao(double valor) {
    double ratio = (valor - m_valorMin) / (m_valorMax - m_valorMin);
    return m_x + (int)(ratio * m_width);
}

double SliderVazao::posicaoParaValor(int pos) {
    double ratio = (double)(pos - m_x) / m_width;
    double valor = m_valorMin + ratio * (m_valorMax - m_valorMin);

    // Limitar o valor aos limites definidos
    if (valor < m_valorMin) {
        return m_valorMin;
    } else if (valor > m_valorMax) {
        return m_valorMax;
    } else {
        return valor;
    }
}