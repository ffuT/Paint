#pragma once

#include <functional>
#include <unordered_map>

namespace Color {
    // unsigned int color = AABBGGRR
    constexpr unsigned int noBG = 0x00000000;
    constexpr unsigned int White = 0xffffffff;
    constexpr unsigned int Black = 0xff000000;
    constexpr unsigned int Red = 0xff0000ff;
    constexpr unsigned int Green = 0xff00ff00;
    constexpr unsigned int Blue = 0xffff0000;
};

enum brush{
    circle,
    square,

    NONE // NONE so it can wrap to circle
};

class Brush{
    public:
    Brush();
    ~Brush();

    void setBrush(brush);
    void nextBrush();
    void stamp(unsigned int*, int, int, int, int) const;

    void setColor(unsigned int color);
    unsigned int getColor(){return currentColor;}
    
    void setRadius(float r){brushRadius = r;}
    float getRadius(){return brushRadius;}
    
    private:
    // brush vars
    brush currentBrush = circle;
    float brushRadius = 5.0f;
    unsigned int currentColor = Color::Black;

    std::unordered_map<const brush, std::function<bool(int, int, int)>> brushmap = {
        { brush::NONE, [](int dx, int dy, int r) 
            {return false;}},
        { brush::circle, [](int dx, int dy, int r) 
            {return dx*dx + dy*dy < r*r;}},
        { brush::square, [](int dx, int dy, int r) 
            {return true;}}
    };
};