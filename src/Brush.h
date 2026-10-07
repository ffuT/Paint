#pragma once

#include <cstdint>
#include <functional>
#include <map>

namespace Color {
    // unsigned int color = AABBGGRR
    constexpr uint32_t noBG = 0x00000000;
    constexpr uint32_t White = 0xffffffff;
    constexpr uint32_t Black = 0xff000000;
    constexpr uint32_t Red = 0xff0000ff;
    constexpr uint32_t Green = 0xff00ff00;
    constexpr uint32_t Blue = 0xffff0000;
};

enum class brush{
    circle,
    square,

    NONE // NONE so it can wrap to circle
};

enum class tool{
    fill,
    select_,
    NONE
};

class Brush{
    public:
    Brush();
    ~Brush();

    void setBrush(brush);
    void nextBrush();
    bool stamp(uint32_t*, int, int, int, int) const;
    bool useTool(uint32_t*, int, int, int, int);
    
    bool isOnTool();
    void setTool(const tool);
    
    void setColor(uint32_t color);
    uint32_t getColor(){return m_currentColor;}
    
    void setRadius(float r){m_brushRadius = r;}
    float getRadius(){return m_brushRadius;}
    
    private:

    bool floodFill(uint32_t*, int, int, int, int);
    uint32_t m_fillcolor = 0;

    tool m_currentTool = tool::NONE;
    // brush vars
    brush m_currentBrush = brush::circle;
    float m_brushRadius = 5.0f;
    unsigned int m_currentColor = Color::Black;

    std::map<const brush, std::function<bool(int, int, int)>> brushmap = {
        { brush::circle, [](int dx, int dy, float r) 
            {return dx*dx + dy*dy < r*r;}},
        { brush::square, [](int dx, int dy, float r) 
            {return true;}}
    };
};