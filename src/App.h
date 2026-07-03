#pragma once

#include <glad.h>
#include <GLFW/glfw3.h>

#include "Canvas.h"
#include "Brush.h"
#include "Renderer.h"
#include "imgui/imgui.h"

class App{
    public:
    App();
    ~App();
    
    bool initialize(int argc, char* argv[]); // return true on success
    void start();
    
    void updateViewport();
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

    Canvas m_canvas = Canvas(1,1);
    Brush m_brush;
    private:

    void loadLuaconf(const char*);
    void render();
    void renderUI();
    
    void drag();
    void draw();

    vec2f mouseToPixels();

    // input control
    bool m_CTRLDown = false;
    bool m_SHIFTDown = false;
    bool m_mouseLeftDown = false;
    bool m_mouseRightDown = false;
    bool m_openCanvasPopup = false;
    bool m_ImGuiCaptureMouse = false;
    vec2f m_mouse;
    vec2f m_dragStart;

    // canvas var
    float m_canvasOffsetWidth = 0;
    float m_canvasOffsetHeight = 0;
    float m_zoom = 1.0f;
    vec2f m_scale;
    
    //config vars
    bool m_clearAlhpa = false;
    float m_MaxZoom = 10;

    // window + GL
    Renderer m_renderer;
    int m_width = 800, m_height = 800; // OS window size
    int m_fbwidth, m_fbheight; // raw window pix size
    GLFWwindow* m_window;
    const ImGuiWindowFlags m_flags;
};