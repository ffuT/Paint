#include "App.h"
#include "Canvas.h"
#include "Renderer.h"

Renderer::Renderer(){}

void Renderer::cleanup(){
    glDeleteProgram(m_shader);
    glDeleteTextures(1, &m_tex);
    glDeleteVertexArrays(1, &m_vao);
}

void Renderer::updateTex(const Canvas& canvas){
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, canvas.getWidth(), canvas.getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, canvas.getPixels());
}

void Renderer::render(const renderParams& params){
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(m_shader);
    
    glUniform1f(m_canvasWidthOffset, params.canvasRes.x);
    glUniform1f(m_canvasheightOffset, params.canvasRes.y);
    
    glUniform1f(m_zoomOffset, params.zoom);
    
    glUniform2f(m_offsetOffset, params.offset.x, params.offset.y);
    glUniform2f(m_resolutionOffset, params.resolution.x, params.resolution.y);

    glBindVertexArray(m_vao);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void Renderer::createVAO(){
    GLuint vbo;
    glGenVertexArrays(1, &m_vao);
    glGenBuffers(1, &vbo);

    float verts[] = {
        -1,-1, 0,1,
        1,-1, 1,1,
        1, 1, 1,0,
        -1,-1, 0,1,
        1, 1, 1,0,
        -1, 1, 0,0,
    };

    glBindVertexArray(m_vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

void Renderer::init(){
    createVAO();
    createShader();

     // gen the texture
    glGenTextures(1, &m_tex);
    glBindTexture(GL_TEXTURE_2D, m_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Renderer::createShader(){
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

            vec2 cell = floor(vUV * vec2(u_resolution.x / u_resolution.y, 1.0) * 32);
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

    m_canvasWidthOffset = glGetUniformLocation(m_shader, "u_canvasWidth");
    m_canvasheightOffset = glGetUniformLocation(m_shader, "u_canvasHeight");
    m_zoomOffset = glGetUniformLocation(m_shader, "u_zoom");
    m_offsetOffset = glGetUniformLocation(m_shader, "u_offset");
    m_resolutionOffset = glGetUniformLocation(m_shader, "u_resolution");
}