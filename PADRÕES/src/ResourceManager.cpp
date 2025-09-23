#include "ResourceManager.h"
#include <cmath>

ResourceManager::ResourceManager() : m_backgroundBitmap(nullptr), m_useGradient(true) {
}

ResourceManager::~ResourceManager() {
    if (m_backgroundBitmap) {
        DeleteObject(m_backgroundBitmap);
    }
}

bool ResourceManager::loadResources() {
    // Criar bitmap gradiente programaticamente
    HDC hdc = CreateCompatibleDC(nullptr);
    if (!hdc) return false;
    
    m_backgroundBitmap = CreateCompatibleBitmap(hdc, 1024, 768);
    if (!m_backgroundBitmap) {
        DeleteDC(hdc);
        return false;
    }
    
    HGDIOBJ oldBitmap = SelectObject(hdc, m_backgroundBitmap);
    
    // Preencher com gradiente azul
    for (int y = 0; y < 768; y++) {
        int blue = 255 - (y * 150 / 768);
        int green = 248 - (y * 100 / 768);
        HBRUSH brush = CreateSolidBrush(RGB(100, green, blue));
        RECT line = {0, y, 1024, y + 1};
        FillRect(hdc, &line, brush);
        DeleteObject(brush);
    }
    
    SelectObject(hdc, oldBitmap);
    DeleteDC(hdc);
    
    return true;
}

void ResourceManager::drawGradientBackground(HDC hdc, RECT rect) {
    if (m_useGradient) {
        int height = rect.bottom - rect.top;
        for (int y = rect.top; y < rect.bottom; y++) {
            double ratio = (double)(y - rect.top) / height;
            int r = (int)(100 + ratio * 155);
            int g = (int)(180 + ratio * 75);
            int b = (int)(255 - ratio * 100);
            
            HBRUSH brush = CreateSolidBrush(RGB(r, g, b));
            RECT line = {rect.left, y, rect.right, y + 1};
            FillRect(hdc, &line, brush);
            DeleteObject(brush);
        }
    }
}