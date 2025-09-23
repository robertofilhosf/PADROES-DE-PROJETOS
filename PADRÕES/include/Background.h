#ifndef BACKGROUND_H
#define BACKGROUND_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class Background {
private:
    HBITMAP m_bitmap;

public:
    Background();
    ~Background();

    bool loadBitmap(const wchar_t* filename);
    void draw(HDC hdc, int width, int height);
    void drawWaterEffect(HDC hdc, RECT rect, double time);
};

#endif // BACKGROUND_H