#include "Brush.h"
#include <cstdint>
#include <cstdio>
#include <stack>

Brush::Brush(){};
Brush::~Brush(){};

bool Brush::stamp(uint32_t* pixels, int cx, int cy, int w, int h) const{
    bool change = false;
    int r = m_brushRadius;
    for(int y = cy-r; y <= cy+r; y++){
        for(int x = cx-r; x <= cx+r; x++){
            if(x < 0 || x >= w || y < 0 || y >= h)
                continue;   
            int dx = (x - cx);
            int dy = (y - cy);
            bool fill = brushmap.at(m_currentBrush)(dx, dy, r);
            if(fill){
                pixels[y * w + x] = m_currentColor;
                change = true;
            }
        }
    }
    return change;
}

void Brush::setTool(const tool tool){
    m_currentTool = tool;
}

bool Brush::isOnTool(){
    return m_currentTool != tool::NONE;
}

bool Brush::useTool(uint32_t* pixels, int cx, int cy, int w, int h){
    bool change = false;
    if(cx < 0 || cx >= w || cy < 0 || cy >= h)
        return change; // exit if outside canvas
    uint32_t col = pixels[cy*w+cx];
    switch(m_currentTool){
        case tool::fill:
            m_fillcolor = pixels[cy*w+cx];
            change |= floodFill(pixels, cx, cy, w, h);
        break;

        default:
        break;
    }
    return change;
}

bool Brush::floodFill(unsigned int* pixels, int cx, int cy, int w, int h){
    bool change = false;
    std::stack<std::pair<int, int>> stack;
    stack.push({cx, cy});
    while(!stack.empty()) {
        int x = stack.top().first;
        int y = stack.top().second;
        stack.pop();
        if(x < 0 || x >= w || y < 0 || y >= h)
            continue;
        unsigned int col = pixels[y*w + x];
        if(col == m_currentColor || col != m_fillcolor)
            continue;
        change = true;
        pixels[y*w + x] = m_currentColor;
        stack.push({x+1,y});
        stack.push({x-1,y});
        stack.push({x,y+1});
        stack.push({x,y-1});
    }
    return change;
}

void Brush::setColor(unsigned int newcol){
    m_currentColor = newcol;
}

void Brush::nextBrush(){
    int t = (uint32_t) m_currentBrush + 1;
    if(t >= (int) brush::NONE)
        t = 0;
    printf("new brush %d\n", t);
    m_currentBrush = (brush)t;
}