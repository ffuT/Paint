#pragma once
#include <glad.h>
#include "Canvas.h"
#include "Utils.h"

struct renderParams{
    vec2f offset;
    float zoom;
    vec2f resolution;
    vec2f canvasRes;
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

GLuint m_tex, m_shader, m_vao;

GLuint m_canvasWidthOffset;
GLuint m_canvasheightOffset;
GLuint m_zoomOffset;
GLuint m_offsetOffset;
GLuint m_resolutionOffset;
};