#ifndef _RENDER_H_
#define _RENDER_H_

#include "winearAlgebra.h"
#include "wath.h"


typedef unsigned char           uint8_t;
typedef unsigned short int      uint16_t;
typedef unsigned long           uint32_t;
typedef unsigned long long      uint64_t;


// RGBA consistant with <canvas>.
typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
} rgba_t;

// Required screen space info.
typedef struct {
    uint16_t x_max;
    uint16_t y_max;
    float aspect_ratio;
    uint16_t y_start;
    uint16_t y_end;
    float z;
} viewport_t;

// Tests basic intersection between circles and triangles.
void render_intersection(rgba_t* img, const viewport_t* viewport);

// Test depth buffer.
void render_depth(rgba_t* img, const viewport_t* viewport);

// Do some basic direct lighting
void render_basic(rgba_t* img, const viewport_t* viewport);

#endif