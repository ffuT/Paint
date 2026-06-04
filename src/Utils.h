#include <cstdint>

struct vec2f{
    float x;
    float y;
};

static unsigned int colorToUInt(float* col){
    uint8_t r = (uint8_t)(col[0] * 255);
    uint8_t g = (uint8_t)(col[1] * 255);
    uint8_t b = (uint8_t)(col[2] * 255);
    uint8_t a = (uint8_t)(col[3] * 255);
    return (a << 24) | (b << 16) | (g << 8) | r;
};