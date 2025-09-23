#ifndef CONSTANTS_H
#define CONSTANTS_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

namespace Constants {
    inline constexpr int WINDOW_WIDTH = 1200;
    inline constexpr int WINDOW_HEIGHT = 800;

    inline const wchar_t* WINDOW_TITLE() { return L"Sistema Hidrométrico - Hidrômetro Real"; }
    inline const wchar_t* CLASS_NAME() { return L"HidrometroWindow"; }

    // Cores
    inline constexpr COLORREF BACKGROUND_COLOR = RGB(240, 248, 255);
    inline constexpr COLORREF HIDROMETRO_BG_COLOR = RGB(80, 80, 90);
    inline constexpr COLORREF HIDROMETRO_DIGIT_COLOR = RGB(0, 255, 100);
    inline constexpr COLORREF SLIDER_COLOR = RGB(70, 130, 180);

    // Configurações do hidrômetro
    inline constexpr double VAZAO_MINIMA = 0.001;  // 1 litro por segundo
    inline constexpr double VAZAO_MAXIMA = 0.01;   // 10 litros por segundo
}

#endif // CONSTANTS_H