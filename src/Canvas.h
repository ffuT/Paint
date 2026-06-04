#pragma once

#include "Utils.h"
#include "Brush.h"

class Canvas{
    public:
    Canvas(unsigned int W, unsigned int H);

    void draw(vec2f c, vec2f cprev, Brush& brush);
    void clearCanvas();

    void newPixelBuffer(int w, int h);

    unsigned int getWidth() const {return m_canvasWidth;}
    unsigned int getHeight() const {return m_canvasHeight;}

    const unsigned int* getPixels(){return pixels;}

    private:
    unsigned int* pixels;

    unsigned int m_canvasWidth;
    unsigned int m_canvasHeight;
};