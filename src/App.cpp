#include "App.h"
#include "Brush.h"
#include "Canvas.h"

#include <GLFW/glfw3.h>
#include <filesystem>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
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
    m_canvasOffsetWidth = m_scale.x*m_width/2 - (float) m_canvas.getWidth()/2;
    m_canvasOffsetHeight = m_scale.y*m_height/2 - (float) m_canvas.getHeight()/2;
    m_canvasOffsetHeight -= (float) m_height * 1.25;
    
    m_CTRLDown = true;
    updateScroll(0, -1.33);
    m_CTRLDown = false;

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

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
    glfwTerminate();
}

bool App::initialize(int argc, char* argv[]){
    printf("Initializing App!\n");
    
    // get path to load config correctly
    std::filesystem::path dir = std::filesystem::canonical(argv[0]).parent_path();
    std::string confpath = (dir / "../../config/config.lua");

    lua_State* L = loadconfig(confpath.c_str());

    loadint(L, "Width", m_width);
    loadint(L, "Heigth", m_height);
    loadbool(L, "AlphaAsClear", m_clearAlhpa);
    printf("w: %d h: %d\n",m_width, m_height);
    {
        int w,h;
        loadint(L, "CWidth", w);
        loadint(L, "CHeigth", h);
        m_canvas.newPixelBuffer(w, h, m_clearAlhpa ? Color::noBG : Color::White);
    }

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

    createShader();
    createVAO();
    
    // gen the texture
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
        if(m_zoom >= 10) m_zoom = 10;
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
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_canvas.getWidth(), m_canvas.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, m_canvas.getPixels());
    glUseProgram(m_shader);
    
    glUniform1f(glGetUniformLocation(m_shader, "u_canvasWidth"), (float)m_canvas.getWidth());
    glUniform1f(glGetUniformLocation(m_shader, "u_canvasHeight"), (float)m_canvas.getHeight());
    
    glUniform1f(glGetUniformLocation(m_shader, "u_zoom"), m_zoom);
    
    glUniform2f(glGetUniformLocation(m_shader, "u_offset"), m_canvasOffsetWidth, m_canvasOffsetHeight);
    glUniform2f(glGetUniformLocation(m_shader, "u_resolution"), (float)m_fbwidth, (float)m_fbheight);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void App::createVAO(){
    GLuint vbo;
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &vbo);

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void App::updateViewport(){
    int fbw, fbh;
    glfwGetFramebufferSize(m_window, &fbw, &fbh);
    glViewport(0,0,fbw,fbh);
    m_fbwidth = fbw;    
    m_fbheight = fbh;
}

void App::createShader(){
    const char* vertSrc = R"(
        #version 330 core
        layout(location=0) in vec2 pos;
        layout(location=1) in vec2 uv;
        out vec2 vUV;
        void main() { gl_Position = vec4(pos, 0, 1); vUV = vec2(uv.x, -uv.y); }
    )";

    const char* fragSrc = R"(
        #version 330 core
        uniform sampler2D tex;
        uniform float u_canvasWidth;
        uniform float u_canvasHeight;
        uniform float u_zoom;
        uniform vec2 u_offset;
        uniform vec2 u_resolution;
        in vec2 vUV;
        out vec4 color;
        
        void main() { 
            vec2 fragpix = vUV * u_resolution;
            vec2 canvaspix = (fragpix - u_offset) / u_zoom;
            if(canvaspix.x < 0.0 || canvaspix.x >= u_canvasWidth || canvaspix.y < 0.0 || canvaspix.y >= u_canvasHeight){
                color = vec4(0.1, 0.1, 0.1, 0.1);
                return;
            }

            vec2 cell = floor(vUV * vec2(u_canvasWidth / u_canvasHeight, 1.0) * 32);
            float check = mod(cell.x + cell.y, 2.0);
            vec3 checkcol = mix(vec3(0.3),vec3(0.7),check);
            vec2 uv = canvaspix / vec2(u_canvasWidth, u_canvasHeight);
            vec4 texcol = texture(tex, uv);
            color = mix(vec4(checkcol, 1.0), texcol, texcol.a);
        }
    )";

    GLuint vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert, 1, &vertSrc, nullptr);
    glCompileShader(vert);

    GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag, 1, &fragSrc, nullptr);
    glCompileShader(frag);

    m_shader = glCreateProgram();
    glAttachShader(m_shader, vert);
    glAttachShader(m_shader, frag);
    glLinkProgram(m_shader);

    glDeleteShader(vert);
    glDeleteShader(frag);
}

void App::renderUI(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    m_ImGuiCaptureMouse = ImGui::GetIO().WantCaptureMouse;
    
    ImGui::SetNextWindowPos(ImVec2(0,0), ImGuiCond_Always);
    ImGui::SetNextWindowSize(ImVec2(m_width+10, 140), ImGuiCond_Always);
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