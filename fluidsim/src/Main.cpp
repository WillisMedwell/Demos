#include "LinearAlgebra.hpp"

// exports that are accessible from wasm
extern "C" {
    #define MAX_WIDTH 1000
    #define MAX_HEIGHT 1000
    typedef struct RGBA {
        uint8_t r = 0;
        uint8_t g = 0;
        uint8_t b = 0;
        uint8_t a = 0;
    } RGBA;
    RGBA img_data[MAX_WIDTH * MAX_WIDTH];
    void draw(float width, float height);
    void update(float dt);
    void handleInput(int mouse_x, int mouse_y, char key_0, char key_1);
}

void draw(float width, float height)
{
    // nullop
}

void update(float dt [[maybe_unused]])
{
    // nullop
}

void handleInput(int mouse_x [[maybe_unused]], int mouse_y [[maybe_unused]], char key_0 [[maybe_unused]], char key_1 [[maybe_unused]])
{
    // nullop
}

int main()
{
    // null op
}