#include "Stw.hpp"

template <typename T>
struct QuadTreeKeyValue {
    float x, y;
    T value;
};

template <typename T>
class QuadTree {
public:
private: // Type definitions
    struct Circle;
    struct Rectangle {
        float x0, x1, y0, y1;
        constexpr Rectangle()
            : x0(0.0f)
            , x1(0.0f)
            , y0(0.0f)
            , y1(0.0f)
        {
        }
        constexpr Rectangle(float x_min, float x_max, float y_min, float y_max)
            : x0(x_min)
            , x1(x_max)
            , y0(y_min)
            , y1(y_max)
        {
        }

        constexpr auto isWithin(float x, float y) const
        {
            return (x0 <= x && x <= x1) && (y0 <= y && y <= y1);
        }
        constexpr auto width() const -> float
        {
            return Stw::Utility::abs(x1 - x0);
        }
        constexpr auto height() const -> float
        {
            return Stw::Utility::abs(y1 - y0);
        }
        constexpr auto intersects(const Circle& circle) const -> bool
        {
            // Ref: https://yal.cc/rectangle-circle-intersection-test/
            const Rectangle& rectangle = *this;
            const auto dx = circle.x_center - Stw::Utility::max(rectangle.x0, Stw::Utility::min(circle.x_center, rectangle.x1));
            const auto dy = circle.y_center - Stw::Utility::max(rectangle.y0, Stw::Utility::min(circle.y_center, rectangle.y1));
            return ((dx * dx) + (dy * dy)) < (circle.radius * circle.radius);
        }
        consteval static auto test()
        {
            constexpr Rectangle rect(1.0f, 3.0f, 2.0f, 4.0f);
            static_assert(rect.x0 == 1.0f && rect.x1 == 3.0f && rect.y0 == 2.0f && rect.y1 == 4.0f, "Rectangle parameterized constructor test failed!");

            // Test Case 3: Test the isWithin function
            constexpr Rectangle rect2(0.0f, 2.0f, 0.0f, 2.0f);
            static_assert(rect2.isWithin(1.0f, 1.0f), "Rectangle isWithin test failed!");
            static_assert(!rect2.isWithin(3.0f, 3.0f), "Rectangle isWithin test failed!");

            // Test Case 4: Test the width function
            static_assert(rect.width() == 2.0f, "Rectangle width test failed!");

            // Test Case 5: Test the height function
            static_assert(rect.height() == 2.0f, "Rectangle height test failed!");
        }
    };
    struct Circle {
        float x_center, y_center, radius;
        constexpr Circle(float x, float y, float rad)
            : x_center(x)
            , y_center(y)
            , radius(rad)
        {
        }
        constexpr auto isWithin(float x, float y) const -> bool
        {
            return (x_center - x) * (x_center - x) + (y_center - y) * (y_center - y) <= (radius * radius);
        }
        auto intersects(const Rectangle& r0, const Rectangle& r1, const Rectangle& r2, const Rectangle& r3)
        {
            using F32x4 = Stw::Simd::F32x4;
            using Mask32x4 = Stw::Simd::Mask32x4;

            const F32x4 rect_x_min { r0.x0, r1.x0, r2.x0, r3.x0 };
            const F32x4 rect_x_max { r0.x1, r1.x1, r2.x1, r3.x1 };
            const F32x4 rect_y_min { r0.y0, r1.y0, r2.y0, r3.y0 };
            const F32x4 rect_y_max { r0.y1, r1.y1, r2.y1, r3.y1 };

            const F32x4 circle_x { this->x_center };
            const F32x4 circle_y { this->y_center };

            const F32x4 dx = circle_x - max(rect_x_min, min(circle_x, rect_x_max));
            const F32x4 dy = circle_y - max(rect_y_min, min(circle_y, rect_y_max));

            const F32x4 dx2 = dx * dx;
            const F32x4 dy2 = dy * dy;
            const F32x4 distance2 = dx2 + dy2;
            const F32x4 radius2 { this->radius * this->radius };
            const Mask32x4 result = distance2 <= radius2;
            return result;
        }
        consteval static auto test()
        {
            // Test Case 2: Test the parameterized constructor of Circle
            constexpr Circle circle(1.0f, 2.0f, 3.0f);
            static_assert(circle.x_center == 1.0f && circle.y_center == 2.0f && circle.radius == 3.0f, "Circle parameterized constructor test failed!");

            // Test Case 3: Test the isWithin function
            constexpr Circle circle2(0.0f, 0.0f, 5.0f);
            static_assert(circle2.isWithin(3.0f, 4.0f), "Circle isWithin test failed!");
            static_assert(!circle2.isWithin(6.0f, 0.0f), "Circle isWithin test failed!");
        }
    };
    class Quadrant {
    public:
        using KeyValues = Stw::Containers::Stack::Vector<QuadTreeKeyValue<T>, 4>;
        using Children = Stw::Containers::Stack::Array<size_t, 4>;
        union Data {
            KeyValues key_values;
            Children child_indices;
        };
        Rectangle m_bounds;
        Data m_data;
        bool m_has_children;

    public:
        constexpr Quadrant()
            : m_bounds(0, 0, 0, 0)
            , m_data(KeyValues {})
            , m_has_children(false)
        {
        }
        constexpr Quadrant(const Rectangle& bounds)
            : m_bounds(bounds)
            , m_data(KeyValues {})
            , m_has_children(false)
        {
        }
        constexpr Quadrant& operator=(const Quadrant& other)
        {
            m_bounds = other.m_bounds;
            m_has_children = other.m_has_children;
            if (other.m_has_children) {
                m_data.child_indices = other.m_data.child_indices;
            } else {
                m_data.key_values = other.m_data.key_values;
            }
            return *this;
        }
        constexpr Quadrant(const Quadrant& other) = default;

        constexpr auto hasKeyValues() const -> bool
        {
            return !m_has_children;
        }
        constexpr auto hasChildren() const -> bool
        {
            return m_has_children;
        }
        constexpr auto getKeyValues() -> KeyValues&
        {
            Stw::Utility::enforce(hasKeyValues());
            return m_data.key_values;
        }
        constexpr auto getChildren() -> Children&
        {
            Stw::Utility::enforce(this->hasChildren());
            return m_data.child_indices;
        }
        constexpr auto isWithin(float x, float y)
        {
            return m_bounds.isWithin(x, y);
        }
        constexpr auto needsSubdivision()
        {
            // return m_data.key_values.size() >= 4;

            return !m_data.key_values.can_push();
        }
        constexpr auto subdivide()
        {
            const float x_mid = static_cast<float>(static_cast<double>(m_bounds.x0) + static_cast<double>(m_bounds.width()) / 2.0);
            const float y_mid = static_cast<float>(static_cast<double>(m_bounds.y0) + static_cast<double>(m_bounds.height()) / 2.0);

            Stw::Containers::Stack::Array<Quadrant, 4> children;

            children[0] = Quadrant(Rectangle { m_bounds.x0, x_mid, y_mid, m_bounds.y1 });
            children[1] = Quadrant(Rectangle { m_bounds.x0, x_mid, m_bounds.y0, y_mid });
            children[2] = Quadrant(Rectangle { x_mid, m_bounds.x1, y_mid, m_bounds.y1 });
            children[3] = Quadrant(Rectangle { x_mid, m_bounds.x1, m_bounds.y0, y_mid });

            for (const QuadTreeKeyValue<T>& kv : m_data.key_values) {
                if (children[0].isWithin(kv.x, kv.y)) {
                    children[0].m_data.key_values.push_back(kv);
                } else if (children[1].isWithin(kv.x, kv.y)) {
                    children[1].m_data.key_values.push_back(kv);
                } else if (children[2].isWithin(kv.x, kv.y)) {
                    children[2].m_data.key_values.push_back(kv);
                } else if (children[3].isWithin(kv.x, kv.y)) {
                    children[3].m_data.key_values.push_back(kv);
                } else {
                    Stw::Utility::enforce(false);
                }
            }
            m_data.key_values.resize(0);
            m_data.child_indices = Children {};
            m_has_children = true;
            return children;
        }
        constexpr auto setChildren(size_t c0, size_t c1, size_t c2, size_t c3)
        {
            m_has_children = true;
            m_data.child_indices = Children {};
            m_data.child_indices[0] = c0;
            m_data.child_indices[1] = c1;
            m_data.child_indices[2] = c2;
            m_data.child_indices[3] = c3;
        }
        constexpr auto width()
        {
            return m_bounds.width();
        }
        constexpr auto height()
        {
            return m_bounds.width();
        }
        constexpr auto reset()
        {
            *this = Quadrant(m_bounds);
        }
        consteval static auto test()
        {
            // Test Case 2: Test the parameterized constructor of Quadrant
            constexpr Rectangle bounds(-1.0f, 1.0f, 2.0f, 3.0f);
            constexpr Quadrant quadrant(bounds);
            static_assert(quadrant.m_bounds.x0 == -1.0f
                    && quadrant.m_bounds.x1 == 1.0f
                    && quadrant.m_bounds.y0 == 2.0f
                    && quadrant.m_bounds.y1 == 3.0f,
                "Quadrant parameterized constructor test failed!");
        }
    };
    struct NearestKeyValue {
        QuadTreeKeyValue<T> key_value;
        float distance;
        bool has_value = false;
    };

public:
    class Iterator {
        friend QuadTree;
        Quadrant* m_quadrant_iter;
        const Quadrant* m_quadrant_iter_end;
        size_t m_key_value_index;

    public:
        Iterator(Quadrant* quadrant_iter, Quadrant* quadrant_iter_end)
            : m_quadrant_iter(quadrant_iter)
            , m_quadrant_iter_end(quadrant_iter_end)
            , m_key_value_index(0)
        {
        }
        auto operator*() -> QuadTreeKeyValue<T>&
        {
            return m_quadrant_iter->getKeyValues()[m_key_value_index];
        }
        auto operator++() -> Iterator&
        {
            ++m_key_value_index;
            if (m_quadrant_iter == m_quadrant_iter_end) {
                return *this;
            }
            if (m_quadrant_iter->hasKeyValues() && m_key_value_index < m_quadrant_iter->getKeyValues().size()) {
                return *this;
            }
            m_key_value_index = 0;
            for (++m_quadrant_iter; m_quadrant_iter != m_quadrant_iter_end; ++m_quadrant_iter) {
                if (m_quadrant_iter->hasKeyValues() && m_quadrant_iter->getKeyValues().size() > 0) {
                    break;
                }
            }
            return *this;
        }
        auto operator++(int) -> Iterator
        {
            Iterator cpy = *this;
            ++(*this);
            return cpy;
        }
        auto operator==(const Iterator& other) const -> bool
        {
            return m_quadrant_iter == other.m_quadrant_iter && m_key_value_index == other.m_key_value_index;
        }
        auto operator!=(const Iterator& other) const -> bool
        {
            return !(*this == other);
        }
    };

private: // Members
    Stw::Containers::Heap::Vector<Quadrant> m_quadrants;
    size_t m_point_count;

private:
    constexpr auto getDeepestQuadrantIndex(float x, float y)
    {
        Stw::Utility::enforce(m_quadrants.front().isWithin(x, y));

        size_t i = 0;
        // 1.
        while (m_quadrants[i].hasChildren()) {
            auto children_indices = m_quadrants[i].getChildren();
            for (size_t j : children_indices) {
                Stw::Utility::enforce(m_quadrants[j].width() > 0);
                Stw::Utility::enforce(m_quadrants[j].height() > 0);

                if (m_quadrants[j].isWithin(x, y)) {
                    i = j;
                    break;
                }
            }
        }
        return i;
    }
    constexpr auto getDeepestQuadrantIndices(float x, float y)
    {
        Stw::Utility::enforce(m_quadrants.front().isWithin(x, y));
        size_t i = 0;
        size_t s = 0;
        // 1.
        while (m_quadrants[i].hasChildren()) {
            s = i;
            auto children_indices = m_quadrants[i].getChildren();
            for (size_t j : children_indices) {
                Stw::Utility::enforce(m_quadrants[j].width() > 0);
                Stw::Utility::enforce(m_quadrants[j].height() > 0);

                if (m_quadrants[j].isWithin(x, y)) {
                    i = j;
                    break;
                }
            }
        }
        return s;
    }
    constexpr auto queryNearestRecursion(Quadrant* quadrant, NearestKeyValue& nearest_key_value, const Circle& region) -> void
    {
        using F32x4 = Stw::Simd::F32x4;
        using Mask32x4 = Stw::Simd::Mask32x4;

        auto& indices = quadrant->getChildren();

        const Rectangle& b0 = m_quadrants[indices[0]].m_bounds;
        const Rectangle& b1 = m_quadrants[indices[1]].m_bounds;
        const Rectangle& b2 = m_quadrants[indices[2]].m_bounds;
        const Rectangle& b3 = m_quadrants[indices[3]].m_bounds;

        const F32x4 rect_x_min = { b0.x0, b1.x0, b2.x0, b3.x0 };
        const F32x4 rect_x_max = { b0.x1, b1.x1, b2.x1, b3.x1 };
        const F32x4 rect_y_min = { b0.y0, b1.y0, b2.y0, b3.y0 };
        const F32x4 rect_y_max = { b0.y1, b1.y1, b2.y1, b3.y1 };

        const F32x4 circle_x { region.x_center };
        const F32x4 circle_y { region.y_center };

        const F32x4 dx = circle_x - max(rect_x_min, min(region.x_center, rect_x_max));
        const F32x4 dy = circle_y - max(rect_y_min, min(region.y_center, rect_y_max));

        const F32x4 radius { region.radius };
        const F32x4 radius2 = radius * radius;
        const F32x4 dx2 = dx * dx;
        const F32x4 dy2 = dy * dy;
        const F32x4 distance2 = dx2 + dy2;
        const Mask32x4 resultMask = distance2 < radius2;

        auto getDistance = [&](const QuadTreeKeyValue<T>& kv) {
            return Stw::Math2D::distance(
                kv.x, kv.y,
                region.x_center, region.y_center);
        };

        for (size_t i = 0; i < 4; ++i) {
            if (resultMask[i]) {
                auto& q = m_quadrants[indices[i]];
                if (q.hasKeyValues()) {
                    for (const QuadTreeKeyValue<T>& kv : q.getKeyValues()) {
                        float dist = getDistance(kv);
                        if (nearest_key_value.has_value) [[likely]] {
                            if (dist < nearest_key_value.distance) {
                                nearest_key_value = { kv, dist, true };
                            }
                        } else {
                            nearest_key_value = { kv, dist, true };
                        }
                    }
                } else {
                    queryNearestRecursion(&q, nearest_key_value, region);
                }
            }
        }
        // for (int i = 0; i < 4; ++i) {
        //     if (resultMask[i]) {
        //         auto& q = m_quadrants[indices[i]];
        //         if (q.hasKeyValues()) {
        //             if (!nearest_key_value.has_value) [[likely]] {
        //                 const KeyValue& kv = *q.getKeyValues().begin(); // assuming getKeyValues() returns a container with begin() method
        //                 nearest_key_value = { kv, getDistance(kv), true };
        //             }

        //             for (const KeyValue& kv : q.getKeyValues()) {
        //                 float dist = getDistance(kv);
        //                 if (dist < nearest_key_value.distance) {
        //                     nearest_key_value = { kv, dist, true };
        //                 }
        //             }
        //         } else {
        //             queryNearestRecursion(&q, nearest_key_value, region);
        //         }
        //     }
        // }
    }
    constexpr auto getSomeRadius(const float x, const float y, size_t index)
    {
        Stw::Utility::enforce(m_quadrants[index].hasChildren());
        float radius = 0;

        for (auto i : m_quadrants[index].getChildren()) {
            if (m_quadrants[i].hasKeyValues() && m_quadrants[i].getKeyValues().size() > 0) {
                return Stw::Math2D::distance(x, y, m_quadrants[i].getKeyValues()[0].x, m_quadrants[i].getKeyValues()[0].y);
            }
        }
        for (auto i : m_quadrants[index].getChildren()) {
            if (m_quadrants[i].hasChildren()) {
                return getSomeRadius(x, y, i);
            }
        }
        return radius;
    };

public: // Methods
    constexpr QuadTree(float x0, float y0, float x1, float y1)
        : m_quadrants({})
        , m_point_count(0)
    {
        m_quadrants.reserve(10000);
        m_quadrants.push_back(Rectangle { x0, x1, y0, y1 });

        Rectangle::test();
        Circle::test();
        Quadrant::test();
    }

    constexpr auto insert(const float x, const float y, const T& value) -> void
    {
        Stw::Utility::enforce(m_quadrants.size() > 0);
        Stw::Utility::enforce(m_quadrants.front().isWithin(x, y));

        bool has_inserted = false;
        while (!has_inserted) {
            auto i = getDeepestQuadrantIndex(x, y);
            if (m_quadrants[i].needsSubdivision()) {
                if (!(m_quadrants[i].width() > 0.0f)) {
                    printf(m_quadrants[i].width());
                }
                if (!(m_quadrants[i].height() > 0.0f)) {
                    printf(m_quadrants[i].height());
                }
                Stw::Utility::enforce(m_quadrants[i].width() > 0.0f);
                Stw::Utility::enforce(m_quadrants[i].height() > 0.0f);

                const auto k = m_quadrants.size();
                for (Quadrant& quad : m_quadrants[i].subdivide()) {
                    Stw::Utility::enforce(quad.width() > 0);
                    Stw::Utility::enforce(quad.height() > 0);
                    if (quad.width() == 0.0f || quad.height() == 0.0f) {
                        // error
                        return;
                    }
                    m_quadrants.push_back(quad);
                }
                m_quadrants[i].setChildren(k, k + 1, k + 2, k + 3);
            } else {
                m_quadrants[i].getKeyValues().push_back(QuadTreeKeyValue<T> { x, y, value });
                has_inserted = true;
            }
        }
        ++m_point_count;
    }
    constexpr auto nearest(const float x, const float y)
    {
        Stw::Utility::enforce(m_quadrants.size() > 1);
        auto i = getDeepestQuadrantIndices(x, y);
        float radius = getSomeRadius(x, y, i);
        Circle circle { x, y, radius + 0.1f };
        NearestKeyValue nearest_key_value;
        queryNearestRecursion(m_quadrants.begin(), nearest_key_value, circle);
        return nearest_key_value.key_value;
    }

    constexpr auto size() const
    {
        return m_point_count;
    }
    constexpr auto reset()
    {
        for (auto iter = m_quadrants.begin() + 1; iter != m_quadrants.end(); ++iter) {
            *iter = Quadrant {};
        }
        m_quadrants.resize(1);
        m_quadrants[0] = Quadrant { m_quadrants[0].m_bounds };
        m_point_count = 0;
    }

    auto begin() -> Iterator
    {
        if (m_quadrants.size() != 1) {
            return ++Iterator(m_quadrants.begin(), m_quadrants.end());
        } else {
            return Iterator(m_quadrants.begin(), m_quadrants.end());
        }
    }
    auto end() -> Iterator
    {
        return Iterator(m_quadrants.end(), m_quadrants.end());
    }

    constexpr auto width()
    {
        return m_quadrants.front().width();
    }
    constexpr auto height()
    {
        return m_quadrants.front().height();
    }
};
