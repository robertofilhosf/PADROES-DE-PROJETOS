#ifndef HIDROMETRO_H
#define HIDROMETRO_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>

class Hidrometro {
private:
    double m_volumeTotal;  // Em metros cúbicos
    double m_vazaoAtual;   // Em m³ por segundo
    double m_ultimoTempo;
    int m_x, m_y;          // Posição do hidrômetro principal

public:
    Hidrometro(int x, int y);

    void update(double currentTime);
    void draw(HDC hdc);
    void setVazao(double vazao) { m_vazaoAtual = vazao; }
    double getVolumeTotal() const { return m_volumeTotal; }
    double getVazaoAtual() const { return m_vazaoAtual; }

private:
    void drawHidrometroPrincipal(HDC hdc, double volume);
    void drawRelogioLitros(HDC hdc, double litros, int x, int y, int size, const std::wstring& titulo);
    void drawRelogioDecimos(HDC hdc, double decimos, int x, int y, int size, const std::wstring& titulo);
    void extrairDigitos(double volume, int digitos[6], int& litros, int& decimos);
};

#endif // HIDROMETRO_H