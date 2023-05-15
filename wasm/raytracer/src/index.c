#include "render.h"

// Image limits.
#define WIDTH_MAX 1920
#define HEIGHT_MAX 1080

// Image output.
rgba_t img_data[HEIGHT_MAX*WIDTH_MAX];

// Special constant for degrees to radians
#define DEG_TO_RAD 0.01745329251f

// Render options.
#define INTERSECTION 0
#define DEPTH 1
#define BASIC 2
#define LAMBERT 3

/*
*   the entry point for the wasm, it allows for threading and choice of style of render.
*/
void render_entry_point(int screen_width, int screen_height, int fov, int height_start, int height_end, int render_option){
    // intialising the camera and what region this thread renders.
    const viewport_t viewport = (viewport_t) {
        .x_max        = screen_width, 
        .y_max        = screen_height, 
        .aspect_ratio = (float)(screen_width) / screen_height,
        .y_start      = height_start, 
        .y_end        = height_end,
        .z            = 0.5 / wtan(fov * DEG_TO_RAD / 2)
    };

    switch(render_option){
        case INTERSECTION: {
            render_intersection(img_data, &viewport);
        } break;
        case DEPTH: {
            render_depth(img_data, &viewport);
        } break;
        case BASIC: {
            render_basic(img_data, &viewport);
        } break;
        case LAMBERT: {
            //render_lambert(img_data, &viewport);
        } break;
        default: {

        }
    }
}
