#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

class ResourceManager {
private:
    HBITMAP m_backgroundBitmap;
    bool m_useGradient;

public:
    ResourceManager();
    ~ResourceManager();

    bool loadResources();
    HBITMAP getBackgroundBitmap() const { return m_backgroundBitmap; }
    void drawGradientBackground(HDC hdc, RECT rect);
};

#endif // RESOURCEMANAGER_H