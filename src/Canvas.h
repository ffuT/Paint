#pragma once

#include <cstdint>
#include <deque>
#include "Utils.h"
#include "Brush.h"

class Canvas{
    public:
    Canvas(unsigned int W, unsigned int H);
    ~Canvas();

    void draw(vec2f c, vec2f cprev, Brush& brush);
    void clearCanvas(const uint32_t color);

    void newPixelBuffer(int w, int h, const uint32_t clearColor);
    
    void saveSnapshot();
    void goToLastSnap();
    void goToNextSnap();
    
    unsigned int getWidth() const {return m_canvasWidth;}
    unsigned int getHeight() const {return m_canvasHeight;}
    const uint32_t* getPixels() const {return pixels;}
    
    bool m_dirty = true;
    bool m_dirtyBuffer = true;
    private:
    // canvas var
    uint32_t* pixels;
    uint32_t m_canvasWidth;
    uint32_t m_canvasHeight;

    // snapshot var
    int8_t currentSnapshot = -1;
    const int8_t maxSnapshots = 20;
    std::deque<uint32_t*> snapShots; 
};