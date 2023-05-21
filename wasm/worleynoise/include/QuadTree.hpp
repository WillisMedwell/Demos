#include "FixedVector.hpp"

#include <array>
#include <cmath>

template <typename T, size_t S = 20000>
class QuadTree {
public:
    struct Rectangle {
        float _x_min, _x_max, _y_min, _y_max;
        Rectangle() = delete;
        Rectangle(float x_min, float x_max, float y_min, float y_max)
            : _x_min(x_min)
            , _x_max(x_max)
            , _y_min(y_min)
            , _y_max(y_max)
        {
        }
        auto getQuarters() -> std::array<Rectangle, 4>
        {

            const float width_half = (_x_max - _x_min) / 2.0f;
            const float height_half = (_y_max - _y_min) / 2.0f;

            const float x_mid = _x_min + width_half;
            const float y_mid = _y_min + height_half;

            return {
                Rectangle(_x_min, x_mid, y_mid, _y_max),
                Rectangle(x_mid, _x_max, y_mid, _y_max),
                Rectangle(_x_min, x_mid, _y_min, y_mid),
                Rectangle(x_mid, _x_max, _y_min, y_mid)
            };
        }
        inline auto isWithin(float x, float y) const -> bool
        {
            return (_x_min <= x && x <= _x_max) && (_y_min <= y && y <= _y_max);
        }
        inline auto doesIntersect(const Rectangle& other) const -> bool
        {
            // wasm_f32x4 x_min_vec = wasm_v128_load(_x_min);
            // wasm_f32x4 x_max_vec = wasm_v128_load(_x_max);
            // wasm_f32x4 y_min_vec = wasm_v128_load(_y_min);
            // wasm_f32x4 y_max_vec = wasm_v128_load(_y_max);

            // wasm_f32x4 other_x_min_vec = wasm_v128_load(other_x_min);
            // wasm_f32x4 other_x_max_vec = wasm_v128_load(other_x_max);
            // wasm_f32x4 other_y_min_vec = wasm_v128_load(other_y_min);
            // wasm_f32x4 other_y_max_vec = wasm_v128_load(other_y_max);

            // wasm_f32x4 x_max_ge_x_min = wasm_f32x4_ge(x_max_vec, other_x_min_vec);
            // wasm_f32x4 other_x_max_ge_x_min = wasm_f32x4_ge(other_x_max_vec, x_min_vec);
            // bool x_overlap = wasm_i32x4_all_true(wasm_v128_and(x_max_ge_x_min, other_x_max_ge_x_min));

            // wasm_f32x4 y_max_ge_y_min = wasm_f32x4_ge(y_max_vec, other_y_min_vec);
            // wasm_f32x4 other_y_max_ge_y_min = wasm_f32x4_ge(other_y_max_vec, y_min_vec);
            // bool y_overlap = wasm_i32x4_all_true(wasm_v128_and(y_max_ge_y_min, other_y_max_ge_y_min));

            // return x_overlap && y_overlap;

            return (_x_max >= other._x_min && other._x_max >= _x_min)
                && (_y_max >= other._y_min && other._y_max >= _y_min );
        }
    };

    struct KeyValue {
        float x, y;
        T value;
    };

    struct Quadrant;
    using Leaf = FixedVector<KeyValue, 4>;
    using Nodes = std::array<Quadrant*, 4>;

    struct Quadrant {
        Leaf leaf;
        Nodes nodes;
        Rectangle bounds;
        bool is_leaf;
        Quadrant()
            : bounds(0.0f, 0.0f, 0.0f, 0.0f)
            , is_leaf(true)
        {
        }
        Quadrant(Rectangle bounds_)
            : bounds(bounds_)
            , is_leaf(true)
        {
        }
        inline bool needsSubdivision()
        {
            return (leaf.size() == 4);
        }
        friend std::ostream& operator<<(std::ostream& os, const Quadrant& quad)
        {
            os << '(' << quad.bounds._x_min << "," << quad.bounds._y_min << ") -> (" << quad.bounds._x_max << "," << quad.bounds._y_max << ") = " << quad.leaf.size() << '\n';
            return os;
        }
    };

    FixedVector<Quadrant, S> _tree;
    FixedVector<KeyValue, S> _query_result;

    QuadTree(float x_min, float x_max, float y_min, float y_max)
    {
        _tree.emplace_back(Rectangle { x_min, x_max, y_min, y_max });
    }

    inline auto getDeepestQuadrant(float x, float y) -> Quadrant&
    {
        Quadrant* iter = &_tree.front();

        while (!iter->is_leaf) {
            for (Quadrant* ptr : iter->nodes) {
                if (ptr->bounds.isWithin(x, y)) {
                    iter = ptr;
                    break;
                }
            }
        }
        return *iter;
    }

    auto printDebug()
    {
        std::cout << "QuadTree memory usage = " << sizeof(QuadTree<T>) << "bytes\n";
        std::cout << "quadrants.size() = " << _tree.size() << '\n';
        for (const Quadrant& quad : _tree) {
            std::cout << quad;
        }
    }

    auto query(const Rectangle& query_region) -> FixedVector<KeyValue, S>&
    {
        _query_result.resize(0);
        if (_tree.size()) {
            qureyRecursion(&_tree.front(), _query_result, query_region);
        }

        return _query_result;
    }

    auto insert(float x, float y, const T& value) -> void
    {
        Quadrant& quadrant = getDeepestQuadrant(x, y);

        if (quadrant.needsSubdivision()) {
            const auto new_quadrants_bounds = quadrant.bounds.getQuarters();
            Quadrant& q0 = _tree.emplace_back(new_quadrants_bounds[0]);
            Quadrant& q1 = _tree.emplace_back(new_quadrants_bounds[1]);
            Quadrant& q2 = _tree.emplace_back(new_quadrants_bounds[2]);
            Quadrant& q3 = _tree.emplace_back(new_quadrants_bounds[3]);

            for (KeyValue& kv : quadrant.leaf) {
                if (q0.bounds.isWithin(kv.x, kv.y)) {
                    q0.leaf.push_back(kv);
                } else if (q1.bounds.isWithin(kv.x, kv.y)) {
                    q1.leaf.push_back(kv);
                } else if (q2.bounds.isWithin(kv.x, kv.y)) {
                    q2.leaf.push_back(kv);
                } else if (q3.bounds.isWithin(kv.x, kv.y)) {
                    q3.leaf.push_back(kv);
                }
            }
            quadrant.nodes[0] = &q0;
            quadrant.nodes[1] = &q1;
            quadrant.nodes[2] = &q2;
            quadrant.nodes[3] = &q3;

            quadrant.is_leaf = false;
            quadrant.leaf.resize(0);
            insert(x, y, value);

        } else {
            quadrant.leaf.emplace_back(x, y, value);
        }
    }

    auto nearest(float x, float y)
    {
        const float total_width = _tree.front().bounds._x_max - _tree.front().bounds._x_min;
        const float total_height = _tree.front().bounds._y_max - _tree.front().bounds._y_min;

        constexpr float init_scale = 0.01f;
        float scale = init_scale;
        Rectangle iter_rect {
            x - (total_width * init_scale),
            x + (total_width * init_scale),
            y - (total_height * init_scale),
            y + (total_width * init_scale),
        };
        while (query({ iter_rect }).size() < 5 && scale < 1.0f) {
            iter_rect = Rectangle {
                x - (total_width * scale),
                x + (total_width * scale),
                y - (total_height * scale),
                y + (total_width * scale)
            };
            scale *= 2;
        }

        if (_query_result.size() == 1) {
            return _query_result.front();
        } else {
            auto distance = [&](const KeyValue& kv) {
                return sqrtf((kv.x - x) * (kv.x - x) + (kv.y - y) * (kv.y - y));
            };
            auto minDistance = [&](const auto& lhs, const auto& rhs) {
                return distance(lhs) <= distance(rhs);
            };
            auto iter = std::min_element(_query_result.begin(), _query_result.end(), minDistance);
            return *iter;
        }
    }

private:
    auto qureyRecursion(Quadrant* quadrant, FixedVector<KeyValue, S>& query_result, const Rectangle& region) -> void
    {
        if (quadrant->bounds.doesIntersect(region)) {
            if (quadrant->is_leaf) {
                for (const KeyValue& kv : quadrant->leaf) {
                    if (region.isWithin(kv.x, kv.y)) {
                        query_result.push_back(kv);
                    }
                }
            } else {
                qureyRecursion(quadrant->nodes[0], query_result, region);
                qureyRecursion(quadrant->nodes[1], query_result, region);
                qureyRecursion(quadrant->nodes[2], query_result, region);
                qureyRecursion(quadrant->nodes[3], query_result, region);
            }
        }
    }
};