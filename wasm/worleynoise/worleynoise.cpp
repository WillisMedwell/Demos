#include <algorithm>
#include <array>
#include <iostream>
#include <random>
#include <ranges>
#include <vector>

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
  float distance(int x2, int y2) const {
    return sqrtf((x - x2) * (x - x2) + (y - y2) * (y - y2));
  }
};

std::array<RGBA, WIDTH * HEIGHT> canvas;

extern "C" {
void generate(int, float);
void* getCanvas();
}
void* getCanvas() {
  return canvas.data();
}

void generate(int num_points, float range) {
  std::vector<Pos2> points;
  points.resize(num_points);

  std::random_device rand_device;
  std::mt19937 rand_generator(rand_device());
  std::uniform_int_distribution<> x_distribution(0, WIDTH - 1);
  std::uniform_int_distribution<> y_distribution(0, HEIGHT - 1);

  for (auto& point : points) {
    point = {x_distribution(rand_generator), y_distribution(rand_generator)};
  }

  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {
      auto closetPoint = [&](const Pos2& lhs, const Pos2& rhs) {
        return lhs.distance(x, y) < rhs.distance(x, y);
      };

      auto iter = std::min_element(points.begin(), points.end(), closetPoint);

      uint8_t dist = static_cast<uint8_t>(
          (std::clamp(iter->distance(x, y), 0.0f, range) / range) * 255);

      canvas.at(x * HEIGHT + y) = RGBA{dist, dist, dist, 255};
    }
  }
}

int main() {}