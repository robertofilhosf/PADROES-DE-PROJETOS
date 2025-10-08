#ifndef SLIDERVAZAO_H
#define SLIDERVAZAO_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class SliderVazao {
private:
    int m_x, m_y, m_width, m_height;
    double m_valorMin, m_valorMax, m_valorAtual;
    bool m_arrastando;

    HBRUSH m_railBrush;
    HBRUSH m_thumbBrush;
    HFONT m_hFont;
    HFONT m_smallFont;

public:
    SliderVazao(int x, int y, int width, int height);
    ~SliderVazao();

    void draw(HDC hdc);
    bool handleMouseEvent(UINT message, int mouseX, int mouseY);
    double getValor() const { return m_valorAtual; }
    
private:
    void setValor(double valor);
    bool pontoDentroSlider(int x, int y);
    int valorParaPosicao(double valor);
    double posicaoParaValor(int pos);
};

#endif // SLIDERVAZAO_H