#include "App.h"
#include <GLFW/glfw3.h>
#include <cstdio>

namespace Color {
    // unsigned int color = AABBGGRR
    constexpr unsigned int White = 0xffffffff;
    constexpr unsigned int Black = 0xff000000;
    constexpr unsigned int Red = 0xff0000ff;
    constexpr unsigned int Green = 0xff00ff00;
    constexpr unsigned int Blue = 0xffff0000;
};

void framebufferSizeCallback(GLFWwindow* window, int w, int h) {
    App* app = (App*) glfwGetWindowUserPointer(window);
    if(!app) return;
    app->setFrameBounds(w, h);
    glViewport(0, 0, w, h);

    printf("resized canvas to %dx%d \n", w, h);
}

void windowResizeCallback(GLFWwindow* window, int w, int h){
    App* app = (App*) glfwGetWindowUserPointer(window);
    if(!app) return;
    app->setWindowBounds(w, h);
    printf("resized window to %dx%d \n", w, h);
}

void curserMoveCallback(GLFWwindow* window, double x, double y){
    App* app = (App*) glfwGetWindowUserPointer(window);
    if(!app) return;
    app->setMousePos(x, y);
}

void mouseClickCallback(GLFWwindow* window, int button, int action, int mods){
    App* app = (App*) glfwGetWindowUserPointer(window);
    if(!app) return;
    int x = app->getMouseX();
    int y = app->getMouseY();
    app->setMouseDown(button, action == GLFW_PRESS);
    printf("mouse click %d x:%d y:%d\n", button, x, y);        
}

void keyPressCallback(GLFWwindow* window, int key, int scancode, int action, int mods){
    printf("%d\n", key);
    
    App* app = (App*) glfwGetWindowUserPointer(window);
    if(!app) return;
}

App::App() : m_canvasWidth(1200), m_canvasHeight(800) {
    m_scale.x = 1.0f;
    m_scale.y = 1.0f;
    m_width = m_canvasWidth;
    m_height = m_canvasHeight;
    pixels = new unsigned int[m_canvasWidth * m_canvasHeight];
    printf("Creating App!\n");
}

App::~App(){
    delete[] pixels;
}

void App::setWindowBounds(int w, int h){
    m_width = w;
    m_height = h;
}

void App::setFrameBounds(int w, int h){
    delete[] pixels;
    pixels = new unsigned int[w * h];
    for(int i = 0; i < w * h; i++){
        pixels[i] = Color::White;
    }
    m_canvasWidth = w;
    m_canvasHeight = h;
}

void App::setMouseDown(int button, bool in){
    switch (button) {
    case GLFW_MOUSE_BUTTON_LEFT:
        m_mouseLeftDown = in;
        break;
    case GLFW_MOUSE_BUTTON_RIGHT:
        m_mouseRightDown = in;
        m_drag = m_mouse;
        break;
    }
}

void App::setMousePos(double x, double y){
    m_mouse.x = x;
    m_mouse.y = y;
}

void App::start(){
    printf("Starting App!\n");
    
    while(!glfwWindowShouldClose(m_window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // get window scale from system
        glfwGetWindowContentScale(m_window, &m_scale.x,&m_scale.y);
   
        // canvas offset pos in on screen:
        m_canvasOffsetWidth = m_scale.x*m_width/2 - (float) m_canvasWidth/2;
        m_canvasOffsetHeight = m_scale.y*m_height/2 - (float) m_canvasHeight/2;

        drag();
        draw();
        render();

        glfwSwapBuffers(m_window);
        glfwWaitEvents();
    }

    glfwTerminate();
}

int App::initialize(){
    printf("Initializing App!\n");

    for(int i = 0; i < m_canvasWidth * m_canvasHeight; i++){
        pixels[i] = Color::White;
    }

    if(!glfwInit()){
        printf("error initializing glfw\n");
        return -1;
    }
    
    m_window = glfwCreateWindow(m_width, m_height, "Paint", nullptr, nullptr);
    if (!m_window) {
        printf("Failed to create GLFW window\n");
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(m_window);

    glfwSetWindowTitle(m_window, "Paint");
    
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        printf("Failed to initialize GLAD\n");
        return -1;
    }

    createShader();
    createVAO();
    
    // gen the texture
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glViewport(0, 0, m_canvasWidth, m_canvasHeight);
    
    //callbacks
    //glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback); // disable to lock canvas
    glfwSetWindowUserPointer(m_window, this);
    glfwSetCursorPosCallback(m_window, curserMoveCallback);
    glfwSetWindowSizeCallback(m_window, windowResizeCallback);
    glfwSetKeyCallback(m_window, keyPressCallback);
    glfwSetMouseButtonCallback(m_window, mouseClickCallback);
    
    return 0;
}

void App::drag(){
    if(!m_mouseRightDown) return;
    float dx = m_drag.x - m_mouse.x;
    float dy = m_drag.y - m_mouse.y;

    m_draggedOffset.x -= dx;
    m_draggedOffset.y += dy;
    m_drag.x -= dx;
    m_drag.y -= dy;
}

void App::draw(){
    int cx = m_mouse.x * m_scale.x - m_canvasOffsetWidth - m_draggedOffset.x;
    int cy = m_mouse.y * m_scale.y - m_canvasOffsetHeight + m_draggedOffset.y;
    static int prevCx = cx;
    static int prevCy = cy;

    if(!m_mouseLeftDown){
        //if not click set prev click to current
        prevCx = cx;
        prevCy = cy;
        return;
    }

    // simple interp between prevc and currentc
    float dx = cx - prevCx, dy = cy - prevCy;
    float dist = std::sqrt(dx*dx+dy*dy);
    int steps = std::max(1, (int)std::ceil(dist));
    for (int i = 0; i <= steps; i++) {
        float t = (float) i/steps;
        stampCircle(prevCx + t * dx, prevCy + t *dy);
        // TODO more stamps
    }

    prevCx = cx;
    prevCy = cy;
}

void App::stampCircle(int cx, int cy){
    int r = m_toolRadius;
    for(int y = cy-r; y <= cy+r; y++){
        for(int x = cx-r; x <= cx+r; x++){
            if(x < 0 || x >= m_canvasWidth || y < 0 || y >= m_canvasHeight)
                continue;   
            int dx = (x - cx);
            int dy = (y - cy);
            if(dx*dx + dy*dy <= r*r){
                pixels[y * m_canvasWidth + x] = Color::Black;
            }
        }
    }
}

void App::render(){
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_canvasWidth, m_canvasHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    
    glViewport(m_canvasOffsetWidth + m_draggedOffset.x,m_canvasOffsetHeight + m_draggedOffset.y, m_canvasWidth, m_canvasHeight);

    glUseProgram(m_shader);
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

void App::createShader(){
    const char* vertSrc = R"(
        #version 330 core
        layout(location=0) in vec2 pos;
        layout(location=1) in vec2 uv;
        out vec2 vUV;
        void main() { gl_Position = vec4(pos, 0, 1); vUV = uv; }
    )";

    const char* fragSrc = R"(
        #version 330 core
        uniform sampler2D tex;
        in vec2 vUV;
        out vec4 color;
        void main() { color = texture(tex, vUV); }
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