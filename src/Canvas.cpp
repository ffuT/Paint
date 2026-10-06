#include "Canvas.h"
#include "Brush.h"
#include "math.h"
#include <cstdint>
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
    snapShots.push_back(new uint32_t[m_canvasWidth * m_canvasHeight]);
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
    m_dirty = true;
    m_dirtyBuffer = true;
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
    m_dirty = true;
}

void Canvas::clearCanvas(const uint32_t color){
    for(int i = 0; i < m_canvasHeight * m_canvasWidth; i++){
        pixels[i] = color;
    }
    m_dirty = true;
    saveSnapshot();
}

void Canvas::draw(vec2f c, vec2f cprev, Brush& brush){
    bool changed = false;
    if(brush.isOnTool()){
        changed |= brush.useTool(pixels, c.x, c.y, m_canvasWidth, m_canvasHeight);
        return;
    }
    // simple interp between prevc and currentc
    float dx = c.x - cprev.x, dy = c.y - cprev.y;
    float dist = std::sqrt(dx*dx+dy*dy);
    int steps = std::max(1, (int)std::ceil(dist));
    for (int i = 0; i <= steps; i++) {
        float t = (float) i/steps;
        changed |= brush.stamp(pixels, cprev.x + t * dx, cprev.y + t *dy, m_canvasWidth, m_canvasHeight);
    }
    m_dirtyBuffer = changed;
    m_dirty = changed;
}

void Canvas::newPixelBuffer(int w, int h, const uint32_t clearColor){
    int maxS = 19999;
    if(w <= 0 || h <= 0 || w >= maxS || h >= maxS){
        printf("invalid canvas size: %dx%d\n",w,h);
        return;
    }

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