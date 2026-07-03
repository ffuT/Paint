#include "App.h"
#include "Brush.h"
#include "Canvas.h"
#include "Renderer.h"
#include "Utils.h"

#include <GLFW/glfw3.h>
#include <filesystem>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <lua.h>
#include <lua.hpp>

#include <cstdio>
#include <math.h>

// void framebufferSizeCallback(GLFWwindow* window, int w, int h) {
//     App* app = (App*) glfwGetWindowUserPointer(window);
//     if(!app) return;
//     app->m_canvas.newPixelBuffer(w, h, (app->m_clearAlhpa & Color::noBG : Color::White));
//     glViewport(0, 0, w, h);
//     printf("resized canvas to %dx%d \n", w, h);
// }

void windowResizeCallback(GLFWwindow* window, int w, int h){
    App* app = (App*) glfwGetWindowUserPointer(window);
    if(!app) return;
    app->setWindowBounds(w, h);
    app->updateViewport();
    printf("resized window to %dx%d \n", w, h);
}

void curserMoveCallback(GLFWwindow* window, double x, double y){
    App* app = (App*) glfwGetWindowUserPointer(window);
    if(!app) return;
    app->setMousePos(x, y);
}

void scrollCallback(GLFWwindow* window, double xoffset, double yoffset){
    App* app = (App*) glfwGetWindowUserPointer(window);
    app->updateScroll(xoffset, yoffset);
}

void mouseClickCallback(GLFWwindow* window, int button, int action, int mods){
    App* app = (App*) glfwGetWindowUserPointer(window);
    if(!app) return;
    int x = app->getMouseX();
    int y = app->getMouseY();
    app->setMouseDown(button, action == GLFW_PRESS);
}

void keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    //printf("%d\n", key);
    App* app = (App*) glfwGetWindowUserPointer(window);
    if(!app) return;
    app->setKey(key, action);
}

App::App() : 
m_flags(ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoResize   |
        ImGuiWindowFlags_NoMove     |
        ImGuiWindowFlags_NoScrollbar|
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus) {
    m_scale.x = 1.0f;
    m_scale.y = 1.0f;
    printf("Creating App!\n");
}

App::~App(){}

void App::setKey(int key, int action){ // action: click = 1, release = 0
    switch (key) {
        case GLFW_KEY_LEFT_CONTROL:
        case GLFW_KEY_RIGHT_CONTROL:
            m_CTRLDown = action;
        break;
        case GLFW_KEY_LEFT_SHIFT:
        case GLFW_KEY_RIGHT_SHIFT:
            m_SHIFTDown = action;
        break;
    }

    if(action){ // only switch on click
        switch (key) {
            case GLFW_KEY_R:
                m_brush.nextBrush();
            break;
            case GLFW_KEY_C:
                if(m_CTRLDown) m_canvas.clearCanvas( m_clearAlhpa ? Color::noBG : Color::White);
            break;
            case GLFW_KEY_Z: // ctrlz
                if(m_CTRLDown) m_canvas.goToLastSnap();
            break;
            case GLFW_KEY_Y: // ctrly
                if(m_CTRLDown) m_canvas.goToNextSnap();
            break;
            case GLFW_KEY_N:
                if(m_CTRLDown) m_openCanvasPopup = true;
            break;
        }
    }
}

void App::setWindowBounds(int w, int h){
    m_width = w;
    m_height = h;
}

void App::setMouseDown(int button, bool in){
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        m_mouseLeftDown = in;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        m_mouseRightDown = in;
        m_dragStart = m_mouse;
        break;
    }

    if(!m_ImGuiCaptureMouse && (button == GLFW_MOUSE_BUTTON_LEFT && in == 0)){
        m_canvas.saveSnapshot();
    }
}

void App::setMousePos(double x, double y){
    m_mouse.x = x;
    m_mouse.y = y;
}

void App::start(){
    printf("Starting App!\n");

    // wakeup window (i guess?) so i can call methods
    glfwSwapBuffers(m_window);
    glfwWaitEvents();
    // get window scale from system
    glfwGetWindowContentScale(m_window, &m_scale.x,&m_scale.y);
    // canvas offset pos on screen:
    
    m_canvasOffsetWidth = (float) m_fbwidth/2 - (float) m_canvas.getWidth()/2 * m_zoom;
    m_canvasOffsetHeight = (float) -m_fbheight/1.8 - (float) m_canvas.getHeight()/2 * m_zoom;

    while(!glfwWindowShouldClose(m_window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
  
        drag();
        draw();
        render();
        renderUI();

        glfwSwapBuffers(m_window);
        glfwWaitEvents();
    }

    m_renderer.cleanup();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

void App::loadLuaconf(const char* path){
    lua_State* L = loadconfig(path);

    loadint(L, "Width", m_width);
    loadint(L, "Heigth", m_height);
    loadbool(L, "AlphaAsClear", m_clearAlhpa);
    {
        int w,h;
        loadint(L, "CWidth", w);
        loadint(L, "CHeigth", h);
        m_canvas.newPixelBuffer(w, h, m_clearAlhpa ? Color::noBG : Color::White);
    }
    loadfloat(L, "MaxZoom", m_MaxZoom);
    loadfloat(L, "StartZoom", m_zoom);
    
    lua_close(L);
}

bool App::initialize(int argc, char* argv[]){
    printf("Initializing App!\n");
    
    // get path to load config correctly
    std::filesystem::path dir = std::filesystem::canonical(argv[0]).parent_path();
    std::filesystem::path confpath = (dir / "../../config/config.lua");
    loadLuaconf(confpath.string().c_str());

    if(!glfwInit()){
        printf("error initializing glfw\n");
        return false;
    }
    
    m_window = glfwCreateWindow(m_width, m_height, "Paint", nullptr, nullptr);
    if (!m_window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return false;
    }
    
    glfwMakeContextCurrent(m_window);

    glfwSetWindowTitle(m_window, "Paint");
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return false;
    }

    m_renderer.init();
    
    updateViewport();
    
    //callbacks
    //glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback); // disable to lock canvas
    glfwSetWindowUserPointer(m_window, this);
    glfwSetCursorPosCallback(m_window, curserMoveCallback);
    glfwSetWindowSizeCallback(m_window, windowResizeCallback);
    glfwSetKeyCallback(m_window, keyPressCallback);
    glfwSetMouseButtonCallback(m_window, mouseClickCallback);
    glfwSetScrollCallback(m_window, scrollCallback);
    
    // init ui
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(m_window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    return true;
}

void App::drag(){
    if(!m_mouseRightDown) return;
    float dx = m_dragStart.x - m_mouse.x;
    float dy = m_dragStart.y - m_mouse.y;

    m_canvasOffsetWidth -= dx;
    m_canvasOffsetHeight += dy;
    m_dragStart.x -= dx;
    m_dragStart.y -= dy;
}

void App::updateScroll(double xoffset, double yoffset){
    if(m_CTRLDown){ // zoom on ctrl + scroll
        double oldzoom = m_zoom;
        m_zoom *= (1.0f + yoffset * 0.1f);
        int CW = m_canvas.getWidth();
        if(m_zoom >= m_MaxZoom) m_zoom = m_MaxZoom;
        if(m_zoom <= .1) m_zoom = .1;
        // not perfect but its ok
        m_canvasOffsetWidth -= ((double) CW/2) * (m_zoom - oldzoom);
        m_canvasOffsetHeight -= ((double) m_canvas.getHeight()/2) * (m_zoom - oldzoom);
    } else { // panning on scroll if not pressing CTRL
        m_canvasOffsetWidth += (xoffset / m_zoom*m_zoom) * 33;
        m_canvasOffsetHeight -= (yoffset / m_zoom*m_zoom) * 33;
    }
}

vec2f App::mouseToPixels(){
    vec2f center; 
    center.x = (m_mouse.x * m_scale.x - (m_canvasOffsetWidth)) / m_zoom;
    center.y = (m_height * m_scale.y - m_mouse.y * m_scale.y - (m_canvasOffsetHeight)) / m_zoom -m_fbheight/m_zoom;
    return center;
}

void App::draw(){
    // center click pos on canvas mouse * scale - offsetst all / by zoom scale
    vec2f center = mouseToPixels();
    static vec2f prevCenter = center;

    if(!m_mouseLeftDown || ImGui::GetIO().WantCaptureMouse){ //if not click set prev + return
        prevCenter = center;
        return;
    }
    m_canvas.draw(center, prevCenter, m_brush);
    prevCenter = center;
}

void App::render(){
    m_renderer.updateTex(m_canvas);
    m_renderer.render({
        .offset = {m_canvasOffsetWidth,m_canvasOffsetHeight},
        .zoom = m_zoom,
        .resolution = {(float) m_fbwidth, (float) m_fbheight},
        .canvasRes = {(float) m_canvas.getWidth(), (float) m_canvas.getHeight() }
    });
}

void App::updateViewport(){
    int fbw, fbh;
    glfwGetFramebufferSize(m_window, &fbw, &fbh);
    glViewport(0,0,fbw,fbh);
    m_fbwidth = fbw;    
    m_fbheight = fbh;
}

void App::renderUI(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    m_ImGuiCaptureMouse = ImGui::GetIO().WantCaptureMouse;
    
    ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_width, 140), ImGuiCond_Always);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::Begin("stuff", nullptr, m_flags);
    {
        // color picker
        static ImVec4 col = ImGui::ColorConvertU32ToFloat4(m_brush.getColor());
        ImGui::Text("Color picker");
        ImGui::SameLine();
        ImGui::ColorEdit4("##color picker", (float*) &col, ImGuiColorEditFlags_NoInputs);

        ImGui::SameLine();
        ImGui::Text(" ");
        ImGui::SameLine();
        if(ImGui::ColorButton("##color1", ImGui::ColorConvertU32ToFloat4(Color::Black)))
            col = ImGui::ColorConvertU32ToFloat4(Color::Black);
        ImGui::SameLine();
        if(ImGui::ColorButton("##color2", ImGui::ColorConvertU32ToFloat4(Color::Red)))
            col = ImGui::ColorConvertU32ToFloat4(Color::Red);
        ImGui::SameLine();
        if(ImGui::ColorButton("##color3", ImGui::ColorConvertU32ToFloat4(Color::Blue)))
            col = ImGui::ColorConvertU32ToFloat4(Color::Blue);
        ImGui::SameLine();
        if(ImGui::ColorButton("##color4", ImGui::ColorConvertU32ToFloat4(Color::Green)))
            col = ImGui::ColorConvertU32ToFloat4(Color::Green);

        m_brush.setColor(ImGui::ColorConvertFloat4ToU32(col));

        // radius picker
        float radius = m_brush.getRadius();
        ImGui::SetNextItemWidth(200);
        ImGui::SliderFloat("brush size", &radius, 1.0f, 50.0f, "%.1f");
        m_brush.setRadius(radius);
    }

    if(m_openCanvasPopup)
        ImGui::OpenPopup("New Canvas");
    if(ImGui::BeginPopupModal("New Canvas")){
        static int w = m_canvas.getWidth(), h = m_canvas.getHeight();
        static bool same = false;
        
        ImGui::InputInt("Width", &w);
        ImGui::BeginDisabled(same);
        ImGui::InputInt("Height", &h);
        ImGui::EndDisabled();

        //if(ImGui::Button("swap"))
            //std::swap(w,h);
        //ImGui::SameLine();
        ImGui::Checkbox("Square", &same);
        ImGui::Checkbox("Transparent", &m_clearAlhpa);
        
        if(ImGui::Button("OK")){
            m_canvas.newPixelBuffer(w, h, m_clearAlhpa ? Color::noBG : Color::White);
            ImGui::CloseCurrentPopup();
        } 
        ImGui::SameLine();       
        if(ImGui::Button("Cancel")){
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
        m_openCanvasPopup = false;
    }
    ImGui::End();

    {   // bottom tool bar
        ImGui::SetNextWindowPos(ImVec2(0,m_height-30), ImGuiCond_Always);
        ImGui::SetNextWindowSize(ImVec2(m_width+10, 30), ImGuiCond_Always);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        
        ImGui::Begin("info", nullptr, m_flags);
        
        ImGui::Text("(%d,%d)", (int) mouseToPixels().x, (int) mouseToPixels().y);
        ImGui::SameLine();
        ImGui::Text("(%d,%d)", m_canvas.getWidth(), m_canvas.getHeight());
        ImGui::SameLine(m_width-100);
        ImGui::Text("(%.0f %%)", m_zoom*100);
        
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    }
}