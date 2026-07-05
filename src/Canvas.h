#pragma once

#include <deque>
#include "Utils.h"
#include "Brush.h"

class Canvas{
    public:
    Canvas(unsigned int W, unsigned int H);
    ~Canvas();

    void draw(vec2f c, vec2f cprev, Brush& brush);
    void clearCanvas(const unsigned int color);
    
    void newPixelBuffer(int w, int h, const unsigned int clearColor);
    
    void saveSnapshot();
    void goToLastSnap();
    void goToNextSnap();
    
    unsigned int getWidth() const {return m_canvasWidth;}
    unsigned int getHeight() const {return m_canvasHeight;}
    const unsigned int* getPixels() const {return pixels;}
    
    private:
    // canvas var
    unsigned int* pixels;
    unsigned int m_canvasWidth;
    unsigned int m_canvasHeight;

    // snapshot var
    int currentSnapshot = -1;
    const int maxSnapshots = 20; // 15*1920*1080*4 ~ 118MB
    std::deque<unsigned int*> snapShots; 
};