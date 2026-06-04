#include "Canvas.h"
#include "Brush.h"
#include "math.h"

Canvas::Canvas(unsigned int w, unsigned int h) : 
    m_canvasWidth(w), m_canvasHeight(h){
    pixels = new unsigned int[w * h];
    clearCanvas();
}

Canvas::~Canvas(){
    delete[] pixels;
}

void Canvas::clearCanvas(){
    for(int i = 0; i < m_canvasHeight * m_canvasWidth; i++){
        pixels[i] = Color::White;
    }
}

void Canvas::draw(vec2f c, vec2f cprev, Brush& brush){
    // simple interp between prevc and currentc
    float dx = c.x - cprev.x, dy = c.y - cprev.y;
    float dist = std::sqrt(dx*dx+dy*dy);
    int steps = std::max(1, (int)std::ceil(dist));
    for (int i = 0; i <= steps; i++) {
        float t = (float) i/steps;
        brush.stamp(pixels, cprev.x + t * dx, cprev.y + t *dy, m_canvasWidth, m_canvasHeight);
    }
}

void Canvas::newPixelBuffer(int w, int h){
    delete[] pixels;
    pixels = new unsigned int[w * h];
    for(int i = 0; i < w * h; i++){
        pixels[i] = Color::White;
    }
    m_canvasWidth = w;
    m_canvasHeight = h;
}