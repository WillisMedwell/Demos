#include "Math.hpp"
#include "Neural.hpp"
#include "Render.hpp"

#include <iomanip>
#include <iostream>

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
void draw(float width, float height, float yaw, float pitch, float roll, float fov_degrees);
void update(float dt);
void handleInput(int mouse_x, int mouse_y, char key_0, char key_1);
}

Rgb96 img_data_float[MAX_WIDTH * MAX_WIDTH];

Objects objects;
Camera prev_camera;

void createRandomObjects()
{
}

void draw(float width, float height, float yaw, float pitch, float roll, float fov_degrees)
{
    if (objects.spheres.size() == 0) {
        // ground
        objects.spheres.emplace_back(Math::LinearAlgebra::Pos<3, float> { 0.0, -100.5, 1.0 }, 100.0f);
        // center
        objects.spheres.emplace_back(Math::LinearAlgebra::Pos<3, float> { 0.0, 0.0, 2.0 }, 0.5);
        // left
        objects.spheres.emplace_back(Math::LinearAlgebra::Pos<3, float> { -1.0, 0.0, 2.0 }, 0.5);
        // left
        objects.spheres.emplace_back(Math::LinearAlgebra::Pos<3, float> { -1.0, 0.0, 2.0 }, -0.45);
        // right
        objects.spheres.emplace_back(Math::LinearAlgebra::Pos<3, float> { 1.0, 0.0, 2.0 }, 0.5);

        objects.sphere_materials.push_back({ MaterialType::lambert, Rgb96 { 0.6f, 0.6f, 0.4f }, 0, 0 }); // ground
        objects.sphere_materials.push_back({ MaterialType::lambert, Rgb96 { 0.1, 0.2, 0.5 }, 0, 0 }); // center
        objects.sphere_materials.push_back({ MaterialType::glass, Rgb96 { 1.0f, 1.0f, 1.0f }, 0, 1.5f }); // left
        objects.sphere_materials.push_back({ MaterialType::glass, Rgb96 { 1.0f, 1.0f, 1.0f }, 0, 1.5f }); // left
        objects.sphere_materials.push_back({ MaterialType::metal, Rgb96 { 0.8, 0.6, 0.2 }, 0, 0 }); // right
    }
    Camera camera {
        { 0, 0, 0 },
        Math::Degrees(yaw), Math::Degrees(pitch), Math::Degrees(roll),
        Math::Degrees(fov_degrees)
    };
    Viewport viewport {
        static_cast<uint16_t>(width),
        static_cast<uint16_t>(height),
        reinterpret_cast<Rgba*>(img_data),
        img_data_float
    };

    static float accumulation_count = 0.0f;

    if (camera == prev_camera) {
        if (accumulation_count == 0.0f) {
            renderReducedResolution<3>(camera, viewport, objects);
            ++accumulation_count;
        } else if (accumulation_count < 500) {
            renderAccumulate(camera, viewport, objects, ++accumulation_count);
        } else {
            // enough
        }
    } else {
        accumulation_count = 0.0f;
        renderReducedResolution<4>(camera, viewport, objects);
    }
    prev_camera = camera;

    // std::array<float, 35> inputs;

    // for (size_t x = 0; x < width; x++) {
    //     for (size_t y = 0; y < height; y++) {
    //         inputs[0] = x;
    //         inputs[1] = y;
    //         inputs[2] = width;
    //         inputs[3] = height;
    //         inputs[4] = yaw;
    //         inputs[5] = pitch;
    //         inputs[6] = roll;
    //         inputs[7] = fov_degrees;
    //         size_t ii = 7;
    //         for (int y_off = -1; y_off < 2; y_off++) {
    //             for (int x_off = -1; x_off < 2; x_off++) {
    //                 size_t index = (y + y_off) * width + size_t(x) + x_off;
    //                 if (index < 0 || index >= width * height) {
    //                     inputs[ii++] = (-1.0f);
    //                     inputs[ii++] = (-1.0f);
    //                     inputs[ii++] = (-1.0f);
    //                 } else {
    //                     inputs[ii++] = img_data_float[index].r;
    //                     inputs[ii++] = img_data_float[index].g;
    //                     inputs[ii++] = img_data_float[index].b;
    //                 }
    //             }
    //         }
    //         std::array<float, 3> net_result = network.propagateForward(inputs);
    //         Rgb96 f { net_result[0], net_result[1], net_result[2] };
    //         size_t iii = (y * width + x);
    //         viewport.image[iii] = static_cast<Rgba>(f);
    //     }
    // }
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
}