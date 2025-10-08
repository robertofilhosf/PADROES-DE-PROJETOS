#include "Hidrometro.h"
#include "Constants.h"
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>

Hidrometro::Hidrometro(int x, int y)
    : m_volumeTotal(0.0), m_vazaoAtual(0.0), m_ultimoTempo(0.0), m_x(x), m_y(y) {
}

void Hidrometro::update(double currentTime) {
    if (m_ultimoTempo > 0) {
        double deltaTime = currentTime - m_ultimoTempo;
        m_volumeTotal += m_vazaoAtual * deltaTime;

        // Limitar o volume total para evitar overflow
        if (m_volumeTotal > 999999.999) {
            m_volumeTotal = 0.0;
        }
    }
    m_ultimoTempo = currentTime;
}

void Hidrometro::draw(HDC hdc) {
    // Desenhar os três componentes do hidrômetro
    drawHidrometroPrincipal(hdc, m_volumeTotal);

    // Calcular litros e décimos CORRETAMENTE
    int digitos[6];
    int litros, decimos;
    extrairDigitos(m_volumeTotal, digitos, litros, decimos);

    // Relógio de litros (0-9) - CORRIGIDO
    drawRelogioLitros(hdc, litros, m_x - 200, m_y + 150, 80, L"Dezenas de Litros");

    // Relógio de décimos de litro (0-9)
    drawRelogioDecimos(hdc, decimos, m_x + 200, m_y + 150, 80, L"Décimos de Litro");
}

void Hidrometro::extrairDigitos(double volume, int digitos[6], int& dezenasLitros, int& decimos) {
    // CORREÇÃO: Extrair os dígitos corretamente considerando a parte decimal

    // 1. Extrair parte inteira (metros cúbicos)
    long long parteInteira = (long long)volume;

    // Preencher os 4 primeiros dígitos com os metros cúbicos
    for (int i = 3; i >= 0; i--) {
        digitos[i] = parteInteira % 10;
        parteInteira /= 10;
    }

    // 2. Extrair parte decimal e converter para litros
    double parteDecimal = volume - (long long)volume;
    double litrosTotais = parteDecimal * 1000.0; // 1 m³ = 1000 litros

    // 3. Dígito 5: Centenas de litros (0-9)
    digitos[4] = ((int)litrosTotais / 100) % 10;

    // 4. Dígito 6: Dezenas de litros (0-9) - ESTE É O QUE DEVE CONTAR!
    digitos[5] = ((int)litrosTotais / 10) % 10;

    // 5. Para os relógios circulares:
    // Dezenas de litros para o relógio esquerdo (0-9)
    dezenasLitros = digitos[5];

    // Décimos de litro para o relógio direito (0-9)
    decimos = ((int)litrosTotais) % 10;
}

void Hidrometro::drawHidrometroPrincipal(HDC hdc, double volume) {
    int digitos[6];
    int dezenasLitros, decimos;
    extrairDigitos(volume, digitos, dezenasLitros, decimos);

    // Fundo do display principal
    HBRUSH bgBrush = CreateSolidBrush(Constants::HIDROMETRO_BG_COLOR);
    RECT bgRect = {m_x - 300, m_y - 100, m_x + 300, m_y + 80};
    FillRect(hdc, &bgRect, bgBrush);
    DeleteObject(bgBrush);

    // Desenhar os 6 dígitos do display principal
    HFONT hFont = CreateFont(48, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE,
                            DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                            CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    HGDIOBJ oldFont = SelectObject(hdc, hFont);
    SetTextColor(hdc, Constants::HIDROMETRO_DIGIT_COLOR);
    SetBkMode(hdc, TRANSPARENT);

    // Desenhar dígitos
    for (int i = 0; i < 6; i++) {
        wchar_t digito[2] = { static_cast<wchar_t>(L'0' + digitos[i], L'\0') };
        RECT rect = {m_x - 250 + i * 100, m_y - 50, m_x - 230 + i * 100, m_y + 30};
        DrawText(hdc, digito, 1, &rect, DT_CENTER | DT_VCENTER);
    }

    // Unidade de medida
    HFONT smallFont = CreateFont(20, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                                DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    SelectObject(hdc, smallFont);

    RECT unitRect = {m_x + 320, m_y - 30, m_x + 380, m_y + 10};
    DrawText(hdc, L"m³", -1, &unitRect, DT_LEFT | DT_VCENTER);

    // Legendas CORRIGIDAS
    RECT legend1 = {m_x - 300, m_y + 40, m_x - 200, m_y + 70};
    DrawText(hdc, L"m³", -1, &legend1, DT_CENTER);

    RECT legend2 = {m_x - 200, m_y + 40, m_x - 100, m_y + 70};
    DrawText(hdc, L"m³", -1, &legend2, DT_CENTER);

    RECT legend3 = {m_x - 100, m_y + 40, m_x - 0, m_y + 70};
    DrawText(hdc, L"m³", -1, &legend3, DT_CENTER);

    RECT legend4 = {m_x - 0, m_y + 40, m_x + 100, m_y + 70};
    DrawText(hdc, L"m³", -1, &legend4, DT_CENTER);

    RECT legend5 = {m_x + 100, m_y + 40, m_x + 200, m_y + 70};
    DrawText(hdc, L"Centenas", -1, &legend5, DT_CENTER);

    RECT legend6 = {m_x + 200, m_y + 40, m_x + 300, m_y + 70};
    DrawText(hdc, L"Dezenas", -1, &legend6, DT_CENTER);

    RECT legend7 = {m_x + 100, m_y + 70, m_x + 200, m_y + 90};
    DrawText(hdc, L"de Litros", -1, &legend7, DT_CENTER);

    RECT legend8 = {m_x + 200, m_y + 70, m_x + 300, m_y + 90};
    DrawText(hdc, L"de Litros", -1, &legend8, DT_CENTER);

    SelectObject(hdc, oldFont);
    DeleteObject(hFont);
    DeleteObject(smallFont);
}

void Hidrometro::drawRelogioLitros(HDC hdc, double dezenasLitros, int x, int y, int size, const std::wstring& titulo) {
    // CORREÇÃO: Limitar dezenas de litros de 0 a 9
    double dezenasLimitado = fmod(dezenasLitros, 10.0);
    if (dezenasLimitado < 0) dezenasLimitado += 10.0;

    // Corpo do relógio
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HGDIOBJ oldPen = SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(100, 100, 120)));

    int radius = size / 2;
    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

    // Ponteiro das dezenas de litros (0-9)
    double angle = (dezenasLimitado / 10.0) * 2 * 3.14159 - 3.14159/2;
    int pointerX = x + (int)((radius - 10) * cos(angle));
    int pointerY = y + (int)((radius - 10) * sin(angle));

    HGDIOBJ redPen = SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(255, 50, 50)));
    MoveToEx(hdc, x, y, nullptr);
    LineTo(hdc, pointerX, pointerY);
    DeleteObject(SelectObject(hdc, redPen));

    // Marcações (0-9)
    for (int i = 0; i < 10; i++) {
        double markAngle = i * 3.14159 / 5 - 3.14159/2;
        int x1 = x + (int)((radius - 5) * cos(markAngle));
        int y1 = y + (int)((radius - 5) * sin(markAngle));
        int x2 = x + (int)((radius - 15) * cos(markAngle));
        int y2 = y + (int)((radius - 15) * sin(markAngle));

        MoveToEx(hdc, x1, y1, nullptr);
        LineTo(hdc, x2, y2);
    }

    // Título
    HFONT font = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    HGDIOBJ oldFont = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(255, 255, 255));

    RECT textRect = {x - radius, y + radius + 5, x + radius, y + radius + 30};
    DrawText(hdc, titulo.c_str(), -1, &textRect, DT_CENTER);

    // Valor atual (dezenas de litros - 0 a 9)
    std::wstring valor = std::to_wstring((int)dezenasLimitado);
    RECT valorRect = {x - radius, y - radius - 30, x + radius, y - radius};
    DrawText(hdc, valor.c_str(), -1, &valorRect, DT_CENTER);

    SelectObject(hdc, oldFont);
    DeleteObject(font);
    DeleteObject(SelectObject(hdc, oldPen));
    SelectObject(hdc, oldBrush);
}

void Hidrometro::drawRelogioDecimos(HDC hdc, double decimos, int x, int y, int size, const std::wstring& titulo) {
    // Corpo do relógio
    HGDIOBJ oldBrush = SelectObject(hdc, GetStockObject(NULL_BRUSH));
    HGDIOBJ oldPen = SelectObject(hdc, CreatePen(PS_SOLID, 2, RGB(100, 100, 120)));

    int radius = size / 2;
    Ellipse(hdc, x - radius, y - radius, x + radius, y + radius);

    // Ponteiro dos décimos (0-9)
    double angle = (decimos / 10.0) * 2 * 3.14159 - 3.14159/2;
    int pointerX = x + (int)((radius - 10) * cos(angle));
    int pointerY = y + (int)((radius - 10) * sin(angle));

    HGDIOBJ bluePen = SelectObject(hdc, CreatePen(PS_SOLID, 3, RGB(50, 150, 255)));
    MoveToEx(hdc, x, y, nullptr);
    LineTo(hdc, pointerX, pointerY);
    DeleteObject(SelectObject(hdc, bluePen));

    // Marcações para cada décimo
    for (int i = 0; i < 10; i++) {
        double markAngle = i * 3.14159 / 5 - 3.14159/2;
        int x1 = x + (int)((radius - 5) * cos(markAngle));
        int y1 = y + (int)((radius - 5) * sin(markAngle));
        int x2 = x + (int)((radius - 15) * cos(markAngle));
        int y2 = y + (int)((radius - 15) * sin(markAngle));

        MoveToEx(hdc, x1, y1, nullptr);
        LineTo(hdc, x2, y2);
    }

    // Título
    HFONT font = CreateFont(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                           DEFAULT_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                           CLEARTYPE_QUALITY, VARIABLE_PITCH, L"Arial");
    HGDIOBJ oldFont = SelectObject(hdc, font);
    SetTextColor(hdc, RGB(255, 255, 255));

    RECT textRect = {x - radius, y + radius + 5, x + radius, y + radius + 30};
    DrawText(hdc, titulo.c_str(), -1, &textRect, DT_CENTER);

    // Valor atual
    std::wstring valor = std::to_wstring((int)decimos);
    RECT valorRect = {x - radius, y - radius - 30, x + radius, y - radius};
    DrawText(hdc, valor.c_str(), -1, &valorRect, DT_CENTER);

    SelectObject(hdc, oldFont);
    DeleteObject(font);
    DeleteObject(SelectObject(hdc, oldPen));
    SelectObject(hdc, oldBrush);
}