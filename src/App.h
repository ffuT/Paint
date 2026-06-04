#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Canvas.h"
#include "Brush.h"

class App{
    public:
    App();
    ~App();
    
    bool initialize(); // return true on success
    void start();
    
    void setWindowBounds(int, int);
    void setFrameBounds(int, int);
    
    int getWidth() {return m_width;};
    int getHeight() {return m_height;};
    
    void setMouseDown(int, bool);
    void setMousePos(double, double);

    void setKey(int key, int action);

    void updateScroll(double xoffset, double yoffset);
    
    double getMouseX() {return m_mouse.x;}
    double getMouseY() {return m_mouse.y;}

    Canvas m_canvas = Canvas(1200, 800);
    Brush m_brush;
    private:

    // input control
    bool m_CTRLDown = false;
    bool m_SHIFTDown = false;
    bool m_mouseLeftDown = false;
    bool m_mouseRightDown = false;
    vec2f m_mouse;
    vec2f m_dragStart;
    vec2f m_draggedOffset = vec2f();

    // canvas var
    int m_canvasOffsetWidth, m_canvasOffsetHeight;
    double m_zoom = 1.0f;
    vec2f m_scale;
    
    // window + GL
    int m_width, m_height;
    GLFWwindow* m_window;
    GLuint m_tex, m_shader, m_vao;

    void createVAO();
    void createShader();
    void render();
    
    void drag();
    void draw();
};

static constexpr float verts[] = {
    -1,-1, 0,1,
     1,-1, 1,1,
     1, 1, 1,0,
    -1,-1, 0,1,
     1, 1, 1,0,
    -1, 1, 0,0,
};