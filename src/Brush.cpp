#include "Brush.h"
#include <cstdio>
#include <stack>

Brush::Brush(){};
Brush::~Brush(){};

void Brush::stamp(unsigned int* pixels, int cx, int cy, int w, int h) const{
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
            }
        }
    }
}

void Brush::setTool(const tool tool){
    m_currentTool = tool;
    m_onTool = tool != NONE_ ? true : false;
}

bool Brush::isOnTool(){
    return m_onTool;
}

void Brush::useTool(unsigned int* pixels, int cx, int cy, int w, int h){
    unsigned int col = pixels[cy*w+cx];
    switch(m_currentTool){
        case fill:
            m_fillcolor = pixels[cy*w+cx];
            floodFill(pixels, cx, cy, w, h);
        break;

        default:
        break;
    }
}

void Brush::floodFill(unsigned int* pixels, int cx, int cy, int w, int h){
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
        pixels[y*w + x] = m_currentColor;
        stack.push({x+1,y});
        stack.push({x-1,y});
        stack.push({x,y+1});
        stack.push({x,y-1});
    }
}

void Brush::setColor(unsigned int newcol){
    m_currentColor = newcol;
}

void Brush::nextBrush(){
    int t = m_currentBrush +1;
    if(t >= (int) brush::NONE)
        t = 0;
    printf("new brush %d\n", t);
    m_currentBrush = (brush)t;
}