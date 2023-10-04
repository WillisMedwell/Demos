//#define NO_SIMD
#define STW_IMPLEMENATION
#include "Stw.hpp"
#include "QuadTree.hpp"

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
    void entry(float width, float height, int num_points, float range);
    void update(float dt);
}

struct Vec2 {
    float i, j;
};

//QuadTree<Vec2>* quadtree = nullptr;

void entry(float width, float height, int num_points, float range)
{
    // if (!quadtree) {
    //     quadtree = new QuadTree<Vec2> { 0, 0, width, height };
    // }
    // if ((int)quadtree->size() > num_points) {
    //     using KV = QuadTreeKeyValue<Vec2>;
    //     Stw::Containers::Stack::Vector<KV, 10000> kvs;
    //     auto i = 0;
    //     for (auto iter = quadtree->begin(); iter != quadtree->end() && i != num_points; ++iter, ++i) {
    //         kvs.push_back(*iter);
    //     }
    //     delete quadtree;
    //     quadtree = nullptr;
    //     resetHeap();

    //     quadtree = new QuadTree<Vec2> { 0, 0, width, height };
    //     for (auto& kv : kvs) {
    //         quadtree->insert(kv.x, kv.y, kv.value);
    //     }
    // } else {
    //     int num_extra = num_points - (int)quadtree->size();
    //     for (int i = 0; i < num_extra; i++) {
    //         quadtree->insert(
    //             Stw::Utility::rand(0, width - 1),
    //             Stw::Utility::rand(0, height - 1),
    //             Vec2 { Stw::Utility::rand(-20, 20), Stw::Utility::rand(-20, 20) });
    //     }
    // }

    // for (size_t y = 0; y < (size_t)height; y++) {
    //     for (size_t x = 0; x < (size_t)width; x++) {
    //         const auto nearest = quadtree->nearest(x, y);
    //         const float distance = Stw::Math2D::distance(nearest.x, nearest.y, static_cast<float>(x), static_cast<float>(y));
    //         const uint8_t channel = static_cast<uint8_t>((Stw::Utility::clamp(static_cast<float>(distance), 0.0f, range) / range) * 255 );
    //         img_data[(int)(y * width + x)] = RGBA { channel, channel, channel, 255 };
    //     }
    // }
}

void update(float dt)
{
    /*
    if (quadtree != nullptr) {
        using KV = QuadTreeKeyValue<Vec2>;
        Stw::Containers::Stack::Vector<KV, 10000> kvs;
        const auto width = quadtree->width();
        const auto height = quadtree->height();
        for (auto [x, y, velocity] : *quadtree) {
            x += velocity.i * dt;
            y += velocity.j * dt;
            if (x > quadtree->width()) {
                velocity.i *= -1.0f;
                x = quadtree->width();
            } else if (x < 0) {
                velocity.i *= -1.0f;
                x = 0;
            }
            if (y > quadtree->height()) {
                y = quadtree->height();
                velocity.j *= -1.0f;
            } else if (y < 0) {
                y = 0;
                velocity.j *= -1.0f;
            }
            kvs.push_back(KV(x, y, velocity));
        }
        quadtree = nullptr;
        resetHeap();
        quadtree = new QuadTree<Vec2> { 0, 0, width, height };
        for (auto kv : kvs) {
            quadtree->insert(kv.x, kv.y, kv.value);
        }
    }
    */
}

int main()
{
    return 0;
}