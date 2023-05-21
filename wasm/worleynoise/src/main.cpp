#include <algorithm>
#include <array>
#include <chrono>
#include <iostream>
#include <random>
#include <ranges>


#include "QuadTree.hpp"

constexpr size_t WIDTH = 300;
constexpr size_t HEIGHT = 600;

struct RGBA {
    uint8_t r = 0;
    uint8_t g = 0;
    uint8_t b = 0;
    uint8_t a = 255;
};

struct Pos2 {
    int x;
    int y;
    [[nodiscard]] auto distance(int x2, int y2) const -> float
    {
        return sqrtf((x - x2) * (x - x2) + (y - y2) * (y - y2));
    }
};

std::array<RGBA, WIDTH * HEIGHT> canvas;

extern "C" {
void generate(int, float);
void* getCanvas();
}
void* getCanvas()
{
    return canvas.data();
}

void generate(int num_points, float range)
{
    QuadTree<uint8_t> quadtree { 0, WIDTH, 0, HEIGHT };

    std::random_device rand_device;
    std::mt19937 rand_generator(rand_device());
    std::uniform_int_distribution<> x_distribution(0, WIDTH - 1);
    std::uniform_int_distribution<> y_distribution(0, HEIGHT - 1);

    for(int i = 0; i < num_points; i++)
    {
        quadtree.insert(x_distribution(rand_generator), y_distribution(rand_generator), 0);
    }

    for (int x = 0; x < WIDTH; x++) {
        for (int y = 0; y < HEIGHT; y++) {
            const auto [nearest_x, nearest_y, _] = quadtree.nearest(x, y); 

            const auto nearest = Pos2{(int)nearest_x, (int)nearest_y};
            const auto dist = static_cast<uint8_t>((std::clamp(nearest.distance(x, y), 0.0f, range)/range)*255);
            canvas.at(x * HEIGHT + y) = RGBA { dist, dist, dist, 255 };
        }
    }
}

int main()
{
    generate(60, 60);
    // // temporary testng of quad tree
    // QuadTree<float> qt(0, 600, 0, 300);

    // auto start = std::chrono::high_resolution_clock::now();
    // qt.insert(3.001f, 3.002f, 1);
    // qt.insert(3.002f, 3.002f, 2);
    // qt.insert(3.001f, 3.001f, 3);
    // qt.insert(3.002f, 3.001f, 4);
    // qt.insert(4.0015f, 4.0015f, 5);

    // std::cout << std::chrono::high_resolution_clock::now() - start << '\n';

    // // qt.printDebug();

    // start = std::chrono::high_resolution_clock::now();
    // auto& thing = qt.query({ 2, 4, 2, 4 });
    // std::cout << std::chrono::high_resolution_clock::now() - start << '\n';

    // // for(const auto [x, y, value] : thing)
    // // {
    // //     std::cout << "(" << x << "," << y << ") -> " << value << '\n';
    // // }
    // start = std::chrono::high_resolution_clock::now();
    // const auto [x, y, value] = qt.nearest(4, 4);
    // std::cout << std::chrono::high_resolution_clock::now() - start << '\n';

    // std::cout << "(" << x << "," << y << ") -> " << value << '\n';
}