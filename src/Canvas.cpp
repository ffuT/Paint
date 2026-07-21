#include "Canvas.h"
#include "Brush.h"
#include "math.h"
#include <cstring>

Canvas::Canvas(unsigned int w, unsigned int h) :
    m_canvasWidth(w), m_canvasHeight(h),
    pixels(new unsigned int[w * h]()){
}

Canvas::~Canvas(){
    delete[] pixels;
    while(!snapShots.empty()){
        delete[] snapShots.back();
        snapShots.pop_back(); 
    }
}

void Canvas::saveSnapshot(){
    currentSnapshot++;
    if(currentSnapshot >= maxSnapshots){
        currentSnapshot--;
        delete[] snapShots.front(); 
        snapShots.pop_front();
    }
    while(currentSnapshot < snapShots.size() && currentSnapshot != snapShots.size()){
        delete[] snapShots.back();
        snapShots.pop_back();
    }
    snapShots.push_back(new unsigned int[m_canvasWidth * m_canvasHeight]);
    std::memcpy(snapShots[currentSnapshot], pixels, m_canvasWidth * m_canvasHeight * sizeof(unsigned int));
    //printf("saved sn at %d\n", currentSnapshot);
}

void Canvas::goToLastSnap(){
    currentSnapshot--;
    if(currentSnapshot < 0){
        currentSnapshot = 0; // reset to first snapshot
        return;
    }

    //printf("copy snapshot at %d\n", currentSnapshot);
    std::memcpy(pixels, snapShots[currentSnapshot], m_canvasWidth * m_canvasHeight * sizeof(unsigned int));
}

void Canvas::goToNextSnap(){
    currentSnapshot++;
    int i = snapShots.size();
    if(currentSnapshot > snapShots.size()-1){ // cant go forward
        currentSnapshot--; // cancel redo
        return;
    }

    //printf("copy snapshot at %d\n", currentSnapshot);
    std::memcpy(pixels, snapShots[currentSnapshot], m_canvasWidth * m_canvasHeight * sizeof(unsigned int));
}

void Canvas::clearCanvas(const unsigned int color){
    for(int i = 0; i < m_canvasHeight * m_canvasWidth; i++){
        pixels[i] = color;
    }
    saveSnapshot();
}

void Canvas::draw(vec2f c, vec2f cprev, Brush& brush){
    if(brush.isOnTool()){
        brush.useTool(pixels, c.x, c.y, m_canvasWidth, m_canvasHeight);
        return;
    }
    // simple interp between prevc and currentc
    float dx = c.x - cprev.x, dy = c.y - cprev.y;
    float dist = std::sqrt(dx*dx+dy*dy);
    int steps = std::max(1, (int)std::ceil(dist));
    for (int i = 0; i <= steps; i++) {
        float t = (float) i/steps;
        brush.stamp(pixels, cprev.x + t * dx, cprev.y + t *dy, m_canvasWidth, m_canvasHeight);
    }
}

void Canvas::newPixelBuffer(int w, int h, const unsigned int clearColor){
    delete[] pixels;
    pixels = new unsigned int[w * h];
    for(int i = 0; i < w * h; i++){
        pixels[i] = clearColor;
    }
    m_canvasWidth = w;
    m_canvasHeight = h;
    
    while(!snapShots.empty()){
        delete[] snapShots.back();
        snapShots.pop_back(); 
    }
    currentSnapshot = -1;
    saveSnapshot();
}