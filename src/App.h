#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>
#include <glm.hpp>

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
    
    void setMouseDown(bool);
    void setMousePos(double, double);
    double getMouseX() {return m_mouseX;}
    double getMouseY() {return m_mouseY;}
    
    private:
    float m_sx = 1.0f, m_sy = 1.0f;
    int m_canvasOffsetWidth, m_canvasOffsetHeight;
    int m_canvasWidth, m_canvasHeight;
    int m_width, m_height;
    unsigned int* pixels;

    // draw tools
    bool m_mouseLeftDown = false;
    int m_toolRadius = 10;

    double m_mouseX, m_mouseY;
    GLFWwindow* m_window;
    GLuint m_tex, m_shader, m_vao;

    void createVAO();
    void createShader();
    void render();
    void draw();

};

const float verts[] = {
    -1,-1, 0,1,
     1,-1, 1,1,
     1, 1, 1,0,
    -1,-1, 0,1,
     1, 1, 1,0,
    -1, 1, 0,0,
};