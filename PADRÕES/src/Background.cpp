#include "Background.h"
#include "Constants.h"
#include <cmath>

Background::Background() : m_bitmap(nullptr) {
}

Background::~Background() {
    if (m_bitmap) {
        DeleteObject(m_bitmap);
    }
}

bool Background::loadBitmap(const wchar_t* filename) {
    m_bitmap = (HBITMAP)LoadImage(nullptr, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    return m_bitmap != nullptr;
}

void Background::draw(HDC hdc, int width, int height) {
    // Desenhar fundo gradiente simples
    for (int y = 0; y < height; y++) {
        double ratio = (double)y / height;
        int r = (int)(70 + ratio * 185);
        int g = (int)(130 + ratio * 125);
        int b = (int)(180 + ratio * 75);

        HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
        RECT line = {0, y, width, y + 1};
        FillRect(hdc, &line, brush);
        DeleteObject(brush);
    }
}

void Background::drawWaterEffect(HDC hdc, RECT rect, double time) {
    // Efeito de onda sutil na parte inferior
    int centerY = rect.bottom - 100;

    for (int y = centerY; y < rect.bottom; y++) {
        double wave = sin((y - centerY) * 0.1 + time * 2) * 5;
        
        HBRUSH brush = CreateSolidBrush(RGB(100, 150, 200));
        RECT waveRect = {
            rect.left + (int)wave,
            y,
            rect.right - (int)wave,
            y + 1
        };
        FillRect(hdc, &waveRect, brush);
        DeleteObject(brush);
    }
}