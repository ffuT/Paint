#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

struct vec2f{
    float x;
    float y;
};

class App{
    public:
    App();
    ~App();
    
    int initialize();
    void start();
    
    void setWindowBounds(int, int);
    void setFrameBounds(int, int);
    
    int getWidth() {return m_width;};
    int getHeight() {return m_height;};
    
    void setMouseDown(int, bool);
    void setMousePos(double, double);
    double getMouseX() {return m_mouse.x;}
    double getMouseY() {return m_mouse.y;}
    

    private:

    vec2f m_scale;
    int m_canvasOffsetWidth, m_canvasOffsetHeight;
    int m_canvasWidth, m_canvasHeight;
    int m_width, m_height;
    unsigned int* pixels;

    vec2f m_drag;
    vec2f m_draggedOffset = vec2f();

    // draw tools
    bool m_mouseLeftDown = false;
    bool m_mouseRightDown = false;
    int m_toolRadius = 5;

    vec2f m_mouse;
    GLFWwindow* m_window;
    GLuint m_tex, m_shader, m_vao;

    void createVAO();
    void createShader();
    void render();

    void drag();

    void draw();
    void stampCircle(int, int);
};

const float verts[] = {
    -1,-1, 0,1,
     1,-1, 1,1,
     1, 1, 1,0,
    -1,-1, 0,1,
     1, 1, 1,0,
    -1, 1, 0,0,
};