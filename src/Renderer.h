#pragma once
#include <glad.h>
#include "Canvas.h"
#include "Utils.h"

struct renderParams{
    vec2f offset;
    float zoom;
    vec2f resolution;
    vec2f canvasRes;
    vec2f mousePos;
    float brushRadius;
    bool useTool;
};

class Renderer{
public:
Renderer();

void init();
void cleanup();
void updateTex(const Canvas&);
void render(const renderParams&);

private:
void createVAO();
void createShader();

GLuint m_tex, m_shader, m_vao, m_vbo;

GLint m_canvasWidthOffset;
GLint m_canvasheightOffset;
GLint m_zoomOffset;
GLint m_offsetOffset;
GLint m_resolutionOffset;
GLint m_mousePos;
GLint m_brushR;
};