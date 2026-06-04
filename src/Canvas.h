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
    // canvas var
    unsigned int* pixels;
    unsigned int m_canvasWidth;
    unsigned int m_canvasHeight;

    // snapshot var
    int currentSnapshot = 0;
    const int maxSnapshots = 20;
    int currentSnapDepth = 0;
    std::vector<unsigned int*> snapShots;

};