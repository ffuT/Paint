#pragma once

#include "Utils.h"
#include "Brush.h"

class Canvas{
    public:
    Canvas(unsigned int W, unsigned int H);
    ~Canvas();

    void draw(vec2f c, vec2f cprev, Brush& brush);
    void clearCanvas();
    
    void newPixelBuffer(int w, int h);
    
    void saveSnapshot();
    void goToLastSnap();
    void goToNextSnap();
    
    unsigned int getWidth() const {return m_canvasWidth;}
    unsigned int getHeight() const {return m_canvasHeight;}
    const unsigned int* getPixels(){return pixels;}
    
    private:
    unsigned int* pixels;
    int currentSnapshot = 0;
    const int maxSnapshots = 50;
    int currentSnapDepth = 0;
    std::vector<unsigned int*> snapShots;

    unsigned int m_canvasWidth;
    unsigned int m_canvasHeight;
};