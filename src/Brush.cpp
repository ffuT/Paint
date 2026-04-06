#include "Brush.h"
#include <cstdio>

Brush::Brush(){};
Brush::~Brush(){};

void Brush::stamp(unsigned int* pixels, int cx, int cy, int w, int h){
    int r = brushRadius;
    for(int y = cy-r; y <= cy+r; y++){
        for(int x = cx-r; x <= cx+r; x++){
            if(x < 0 || x >= w || y < 0 || y >= h)
                continue;   
            int dx = (x - cx);
            int dy = (y - cy);
            bool fill = brushmap[currentBrush](dx, dy, r);
            if(fill){
                pixels[y * w + x] = Color::Black;
            }
        }
    }
}

void Brush::nextBrush(){
    int t = currentBrush +1;
    if(t >= (int) brush::NONE)
        t = 0;
    printf("new brush %d\n", t);
    currentBrush = (brush)t;
}