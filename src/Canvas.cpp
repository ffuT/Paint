#include "Canvas.h"
#include "Brush.h"
#include "math.h"
#include <cstring>

Canvas::Canvas(unsigned int w, unsigned int h) : 
    m_canvasWidth(w), m_canvasHeight(h){
    snapShots = std::vector<unsigned int*>(maxSnapshots, nullptr);
    pixels = new unsigned int[w * h];
    clearCanvas();
}

Canvas::~Canvas(){
    delete[] pixels;
}

void Canvas::saveSnapshot(){
    currentSnapshot++;
    if(currentSnapshot >= maxSnapshots) // wrap back from max
        currentSnapshot = 0;
    if(snapShots[currentSnapshot] != nullptr) 
        delete[] snapShots[currentSnapshot];
    snapShots[currentSnapshot] = new unsigned int[m_canvasWidth * m_canvasHeight];
    std::memcpy(snapShots[currentSnapshot], pixels, m_canvasWidth * m_canvasHeight * sizeof(unsigned int));
    //printf("saved sn at %d\n", currentSnapshot);
    currentSnapDepth = 0;
}

void Canvas::goToLastSnap(){
    currentSnapshot--;
    if(currentSnapshot < 0)
        currentSnapshot = maxSnapshots-1;
    if(currentSnapDepth >= maxSnapshots-1 || snapShots[currentSnapshot] == nullptr){ // cant go back
        currentSnapshot++;
        if(currentSnapshot >= maxSnapshots) 
            currentSnapshot = 0;
        return;
    } 
    currentSnapDepth++;
    //printf("copy snapshot at %d\n", currentSnapshot);
    std::memcpy(pixels, snapShots[currentSnapshot], m_canvasWidth * m_canvasHeight * sizeof(unsigned int));
}

void Canvas::goToNextSnap(){
    currentSnapshot++;
    if(currentSnapshot >= maxSnapshots) // cant go forward
        currentSnapshot = 0;
    if(currentSnapDepth <= 0 || snapShots[currentSnapshot] == nullptr){
        currentSnapshot--;
        if(currentSnapshot < 0) 
            currentSnapshot = maxSnapshots-1;
        return;
    }
    currentSnapDepth--;
    //printf("copy snapshot at %d\n", currentSnapshot);
    std::memcpy(pixels, snapShots[currentSnapshot], m_canvasWidth * m_canvasHeight * sizeof(unsigned int));
}

void Canvas::clearCanvas(){
    for(int i = 0; i < m_canvasHeight * m_canvasWidth; i++){
        pixels[i] = Color::White;
    }
    saveSnapshot();
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

    for(int i = 0; i < maxSnapshots; i++){ // cleanup the snapshots
        if(snapShots[i] != nullptr)
            delete[] snapShots[i];
    }
}