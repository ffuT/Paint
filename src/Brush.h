#pragma once

#include <functional>
#include <map>

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

enum tool{
    fill,
    select_,
    NONE_
};

class Brush{
    public:
    Brush();
    ~Brush();

    void setBrush(brush);
    void nextBrush();
    void stamp(unsigned int*, int, int, int, int) const;
    void useTool(unsigned int*, int, int, int, int);
    
    bool isOnTool();
    void setTool(const tool);
    
    void setColor(unsigned int color);
    unsigned int getColor(){return m_currentColor;}
    
    void setRadius(float r){m_brushRadius = r;}
    float getRadius(){return m_brushRadius;}
    
    private:

    void floodFill(unsigned int*, int, int, int, int);
    unsigned int m_fillcolor = 0;

    bool m_onTool = false;
    tool m_currentTool = fill;
    // brush vars
    brush m_currentBrush = circle;
    float m_brushRadius = 5.0f;
    unsigned int m_currentColor = Color::Black;

    std::map<const brush, std::function<bool(int, int, int)>> brushmap = {
        { brush::circle, [](int dx, int dy, int r) 
            {return dx*dx + dy*dy < r*r;}},
        { brush::square, [](int dx, int dy, int r) 
            {return true;}}
    };
};