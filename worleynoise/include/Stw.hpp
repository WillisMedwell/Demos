/*
 *  As Wasm lib isn't always avaliable in WASM context,
 *  this header provides an alternate solution.
 *
 *  Created by Willis Medwell.
 */
#pragma once

#ifdef __wasm__
#include <wasm_simd128.h>
using ptrdiff_t = long long;
using size_t = __SIZE_TYPE__;
using int8_t = signed char;
using uint8_t = unsigned char;
using int16_t = signed short int;
using uint16_t = unsigned short int;
using int32_t = signed int;
using uint32_t = unsigned int;
using int64_t = signed long long int;
using uint64_t = unsigned long long int;
#else
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <iostream>

#endif //__wasm__

#ifdef __wasm__
extern "C" {
void printf(float number) __attribute__((import_module("env"), import_name("printf")));
void assert(bool result) __attribute__((import_module("env"), import_name("assert")));
void* memset(void* ptr, int value, size_t num)
{
    uint8_t* p = reinterpret_cast<uint8_t*>(ptr);
    while (num--)
        *p++ = static_cast<uint8_t>(value);
    return ptr;
}
int __cxa_atexit(void (*func [[maybe_unused]])(void*), void* arg [[maybe_unused]], void* dso_handle [[maybe_unused]])
{
    return 0;
}
void setHeapBase(void* heap_base);
}
uint8_t* g_heap_base;
uint8_t* g_heap_offset;
uint8_t* g_heap_last_offset;
size_t g_heap_alloc_count;
void setHeapBase(void* heap_base)
{
    g_heap_base = reinterpret_cast<uint8_t*>(heap_base);
    g_heap_offset = reinterpret_cast<uint8_t*>(heap_base);
    g_heap_last_offset = reinterpret_cast<uint8_t*>(heap_base);
    g_heap_alloc_count = 0;
}
void resetHeap()
{
    g_heap_offset = g_heap_base;
    g_heap_last_offset = g_heap_base;
    g_heap_alloc_count = 0;
}
void* operator new(size_t n)
{
    ++g_heap_alloc_count;
    g_heap_last_offset = g_heap_offset;
    void* ptr = reinterpret_cast<void*>(g_heap_offset);
    g_heap_offset += n;
    return ptr;
}
void* operator new[](size_t n)
{
    ++g_heap_alloc_count;
    g_heap_last_offset = g_heap_offset;
    void* ptr = reinterpret_cast<void*>(g_heap_offset);
    g_heap_offset += n;
    return ptr;
}
void operator delete(void* ptr [[maybe_unused]]) noexcept
{
    if (g_heap_last_offset == ptr) {
        g_heap_offset = g_heap_last_offset;
    }
}
void operator delete[](void* ptr [[maybe_unused]]) noexcept
{
    if (g_heap_last_offset == ptr) {
        g_heap_offset = g_heap_last_offset;
    }
}
#endif //__wasm__

namespace Stw {
namespace HeapManagement {
    class ArenaResource;

    template <typename T, typename Resource>
    class ArenaAllocator;
}
namespace Utility {
    constexpr void enforce(bool result);
    constexpr auto sq(const float x) -> float;
    constexpr auto sqrt(const float x) -> float;
    constexpr auto isqrt(const float x) -> float;
    constexpr auto tan(const float x) -> float;
    auto rand(const float min, const float max) -> float;

    template <typename T>
    constexpr auto clamp(const T value, const T min, const T max) -> T;
    template <typename T>
    constexpr auto min(const T x, const T y) -> T;
    template <typename T>
    constexpr auto max(const T x, const T y) -> T;
    template <typename T>
    constexpr auto abs(const T x) -> T;
}
namespace Algo {
    template <typename ForwardIter>
    constexpr auto min_element(ForwardIter iter_begin, ForwardIter iter_end);
    template <typename ForwardIter, typename Compare>
    constexpr auto min_element(ForwardIter iter_begin, ForwardIter iter_end, Compare cmp) -> ForwardIter;
    template <typename ForwardIter, typename Value>
    constexpr auto contains(ForwardIter iter_begin, ForwardIter iter_end, const Value& value) -> bool;
}
namespace Math2D {
    template <typename T>
    constexpr auto distance(T x0, T y0, T x1, T y1) -> T;
}
namespace Containers {
    namespace Stack {
        template <typename T, size_t S>
        class Array;

        template <typename T, size_t S>
        class Vector;
    }
    namespace Heap {
        template <typename T>
        class Vector;
    }
}
namespace Simd {
    struct Mask32x4;
    struct F32x4;
}
}

#ifdef STW_IMPLEMENATION
#undef STW_IMPLEMENATION

#ifndef STW_DEFAULT_ALLOCATOR
#define STW_DEFAULT_ALLOCATOR std::allocator<T>
#endif // STW_DEFAULT_ALLOCATOR

namespace Stw::HeapManagement {
class ArenaResource {
    uint8_t* m_memory;
    size_t m_size;
    size_t m_capacity;

public:
    ArenaResource(size_t n)
        : m_memory(new uint8_t[n])
        , m_size(0)
        , m_capacity(n)
    {
    }
    ArenaResource() = delete;
    ArenaResource(const ArenaResource&) = delete;
    ArenaResource(ArenaResource&&) = delete;

    uint8_t* allocate(size_t n)
    {
        uint8_t* ptr = m_memory + m_size;
        m_size += n;
        Stw::Utility::enforce(m_size < m_capacity);
        return ptr;
    }
    void resetSize()
    {
        m_size = 0;
    }
    ~ArenaResource()
    {
        delete[] m_memory;
    }
};

template <typename T, typename Resource>
class ArenaAllocator {
    Resource* m_resource;

public:
    using value_type = T;
    using size_type = size_t;
    using difference_type = ptrdiff_t;

    ArenaAllocator(Resource* resource)
        : m_resource(resource)
    {
        static_assert(
            requires(Resource* res) { res->allocate(1); }, "Resource does not have allocate function");
    }
    ~ArenaAllocator()
    {
    }
    auto allocate(size_type n) -> value_type*
    {
        return reinterpret_cast<value_type*>(m_resource->allocate(n * sizeof(T)));
    }
    void deallocate(value_type* ptr [[maybe_unused]], size_type n [[maybe_unused]] = 0)
    {
    }
};
}

namespace Stw::Utility {
constexpr void enforce(bool result [[maybe_unused]])
{
#ifdef __wasm__
#ifndef NDEBUG
    assert(result);
#endif // NDEBUG
#else
    assert(result);
#endif // __wasm__
}

constexpr auto sq(const float x) -> float
{
    return x * x;
}
constexpr auto sqrt(const float x) -> float
{
    if consteval {
        // Newtons method -> Ref: https://math.mit.edu/~stevenj/18.335/newton-sqrt.pdf
        float i = x;
        if (x == 0) {
            return 0;
        }
        for (int j = 0; j < 40; j++) {
            i = 0.5f * (i + (x / i));
        }
        return i;
    } else {
        // One of the few supported builtins!!!!
        return __builtin_sqrtf(x);
    }
}
constexpr auto isqrt(const float x) -> float
{
    return 1 / sqrt(x);
}
constexpr auto tan(const float x) -> float
{
    // Power Series calculated from -> https://www.wolframalpha.com/widgets/view.jsp?id=f9476968629e1163bd4a3ba839d60925
    // For more information check out -> https://brilliant.org/wiki/taylor-series/
    double x2, x3, x5, x7, x9, x11, x13, x15, x17, x19, x21;
    x2 = (double)(x * x);
    x3 = x2 * x;
    x5 = x3 * x2;
    x7 = x5 * x2;
    x9 = x7 * x2;
    x11 = x9 * x2;
    x13 = x11 * x2;
    x15 = x13 * x2;
    x17 = x15 * x2;
    x19 = x17 * x2;
    x21 = x19 * x2;
    constexpr double a = 1.0;
    constexpr double b = 1.0 / 3.0;
    constexpr double c = 2.0 / 15.0;
    constexpr double d = 17.0 / 315.0;
    constexpr double e = 62.0 / 2835.0;
    constexpr double f = 1382.0 / 155925.0;
    constexpr double g = 21844.0 / 6081075.0;
    constexpr double h = 929569.0 / 638512875.0;
    constexpr double i = 6404582.0 / 10854718875.0;
    constexpr double j = 443861162.0 / 1856156927625.0;
    constexpr double l = 18888466084.0 / 194896477400625.0;
    return (float)((a * x) + (b * x3) + (c * x5) + (d * x7) + (e * x9) + (f * x11) + (g * x13) + (h * x15) + (i * x17) + (j * x19) + (l * x21));
}
template <typename T>
constexpr auto min(const T x, const T y) -> T
{
    return (x < y) ? x : y;
}
template <typename T>
constexpr auto max(const T x, const T y) -> T
{
    return (x >= y) ? x : y;
}
template <typename T>
constexpr auto abs(const T x) -> T
{
    return (x >= 0) ? x : -x;
}
template <typename T>
constexpr auto clamp(const T value, const T min, const T max) -> T
{
    if (value > max) {
        return max;
    } else if (value < min) {
        return min;
    } else {
        return value;
    }
}

uint32_t rand_seed = 123456743;
auto rand(const float min, const float max) -> float
{
    rand_seed = (rand_seed * 1103515245 + 12345) & 0x7fffffff;

    const uint32_t range = static_cast<uint32_t>(max - min);
    const float scaled = static_cast<float>(rand_seed % range);
    return min + scaled;
}
}

namespace Stw::Algo {
constexpr auto min_element(auto iter_begin, auto iter_end)
{
    auto current_min = iter_begin;
    for (auto iter = iter_begin; iter != iter_end; ++iter) {
        if (*iter < *current_min) {
            current_min = *iter;
        }
    }
    return current_min;
}

template <typename ForwardIter, typename Compare>
constexpr auto min_element(ForwardIter iter_begin, ForwardIter iter_end, Compare cmp) -> ForwardIter
{
    ForwardIter current_min = iter_begin;

    for (auto iter = iter_begin; iter != iter_end; ++iter) {
        if (cmp(*iter, *current_min)) {
            current_min = iter;
        }
    }
    return current_min;
}

template <typename ForwardIter, typename Value>
constexpr auto contains(ForwardIter iter_begin, ForwardIter iter_end, const Value& value) -> bool
{
    for (auto iter = iter_begin; iter != iter_end; ++iter) {
        if (*iter == value) {
            return true;
        }
    }
    return false;
}
}

namespace Stw::Math2D {

template <typename T>
constexpr auto distance(T x0, T y0, T x1, T y1) -> T
{
    using namespace Stw::Utility;
    return sqrt(sq(x0 - x1) + sq(y0 - y1));
}
}

namespace Stw::Containers::Stack {

template <typename T, size_t S>
class Array {
private:
    T m_data[S];

public:
    constexpr Array()
    {
        for (size_t i = 0; i < S; i++) {
            m_data[i] = T {};
        }
    }
    constexpr Array(const Array<T, S>& array)
    {
        for (size_t i = 0; i < S; ++i) {
            m_data[i] = array.m_data[i];
        }
    }
    constexpr Array(Array&& array)
    {
        for (size_t i = 0; i < S; ++i) {
            m_data[i] = array.m_data[i];
        }
    }
    constexpr Array& operator=(const Array<T, S>& array)
    {
        for (size_t i = 0; i < S; ++i) {
            m_data[i] = array.m_data[i];
        }
        return *this;
    }

    constexpr auto operator[](const size_t index) -> T&
    {
        Stw::Utility::enforce(index < S);
        return m_data[index];
    }
    constexpr auto operator[](const size_t index) const -> const T&
    {
        Stw::Utility::enforce(index < S);
        return m_data[index];
    }
    constexpr auto at(const size_t index) -> T&
    {
        Stw::Utility::enforce(index < S);
        if (index < S) {
            return m_data[index];
        } else {
            return T {};
        }
    }
    constexpr auto begin() -> T*
    {
        return m_data;
    }
    constexpr auto end() -> T*
    {
        return m_data + S;
    }
    constexpr auto front() -> T&
    {
        return m_data[0];
    }
    constexpr auto back() -> T&
    {
        return m_data[S - 1];
    }
    constexpr auto size()
    {
        return S;
    }
};

template <typename T, size_t S>
class Vector {
private:
    Stw::Containers::Stack::Array<T, S> m_data;
    size_t m_size;

public:
    constexpr Vector()
        : m_data({})
        , m_size(0)
    {
    }

    constexpr Vector(const Vector& other)
        : m_data(other.m_data)
        , m_size(other.m_size)
    {
    }
    constexpr Vector(Vector&& other)
        : m_data(other.m_data)
        , m_size(other.m_size)
    {
    }
    constexpr auto operator=(const Vector& other)
    {
        m_data = other.m_data;
        m_size = other.m_size;
        return *this;
    }
    constexpr auto can_push() const -> bool
    {
        return m_size < S;
    }
    constexpr auto push_back(const T& element) -> T&
    {
        Stw::Utility::enforce(m_size < S);
        return m_data[m_size++] = element;
    }
    constexpr auto at(size_t index) -> T&
    {
        Stw::Utility::enforce(index < m_size);
        if (index < m_size) {
            return m_data[index];
        } else {
            return m_data[0];
        }
    }
    constexpr auto pop_back() -> void
    {
        Stw::Utility::enforce(m_size != 0);
        --m_size;
    }
    constexpr auto begin() -> T*
    {
        return m_data.begin();
    }
    constexpr auto end() -> T*
    {
        return m_data.begin() + m_size;
    }
    constexpr auto resize(size_t n) -> void
    {
        m_size = Stw::Utility::min(n, S);
    }
    constexpr auto front() -> T&
    {
        return m_data[0];
    }
    constexpr auto back() const -> T&
    {
        return m_data[m_size - 1];
    }
    constexpr auto size() const
    {
        return m_size;
    }
    constexpr auto operator[](size_t index) -> T&
    {
        Stw::Utility::enforce(index < m_size);
        return m_data[index];
    }
};
}

namespace Stw::Containers::Heap {
template <typename T>
class Vector {
    T* m_data;
    size_t m_capacity;
    size_t m_size;

public:
    constexpr Vector()
        : m_data(new T[1])
        , m_capacity(1)
        , m_size(0)
    {
    }
    constexpr Vector(const Vector& other)
        : m_data(new T[other.m_capacity])
        , m_capacity(other.m_capacity)
        , m_size(other.m_size)
    {
        for (size_t i = 0; i < other.m_size; ++i) {
            m_data[i] = other.m_data[i];
        }
    }
    constexpr Vector(Vector&& other)
        : m_data(other.m_data)
        , m_capacity(other.m_capacity)
        , m_size(other.m_size)
    {
        other.m_data = nullptr;
        m_capacity = 0;
        m_size = 0;
    }
    constexpr auto begin() -> T*
    {
        return m_data;
    }
    constexpr auto end() -> T*
    {
        return m_data + m_size;
    }
    constexpr auto front() const -> T&
    {
        Stw::Utility::enforce(m_size > 0);
        return m_data[0];
    }
    constexpr auto back()
    {
        return m_data[m_size - 1];
    }
    constexpr auto reserve(size_t n) -> void
    {
        if (n > m_capacity) {
            T* new_data = new T[n];
            for (size_t i = 0; i < m_size; ++i) {
                new_data[i] = (T&&)(m_data[i]);
            }
            delete[] m_data;
            m_data = new_data;
            m_capacity = n;
        }
    }
    constexpr auto resize(size_t n) -> void
    {
        if (n < m_capacity) {
            m_size = n;
        } else {
            reserve(n);
            m_size = n;
        }
    }
    constexpr auto push_back(const T& element) -> T&
    {
        if (m_size == m_capacity) {
            reserve(m_capacity * 2);
        }
        return m_data[m_size++] = element;
    }
    constexpr auto pop_back()
    {
        Stw::Utility::enforce(m_size != 0);
        --m_size;
    }
    constexpr auto operator[](size_t index) -> T&
    {
        Stw::Utility::enforce(index < m_size);
        return m_data[index];
    }
    constexpr auto operator[](size_t index) const -> const T&
    {
        Stw::Utility::enforce(index < m_size);
        return m_data[index];
    }
    constexpr ~Vector()
    {
        if (m_data != nullptr) {
            delete[] m_data;
            m_data = nullptr;
        }
    }
    constexpr auto size() const -> size_t
    {
        return m_size;
    }
};
}

namespace Stw::Simd {
#ifdef NO_SIMD
struct Mask32x4 {
    using Underlying = int[4];

    Underlying _data;
    Mask32x4(const Underlying& mask)
    {
        for (int i = 0; i < 4; ++i) {
            _data[i] = mask[i];
        }
    }
    bool operator[](const size_t index) const
    {
        return _data[index] != 0;
    }
    friend Mask32x4 invert(const Mask32x4& mask)
    {
        Underlying inverted = { -mask._data[0], -mask._data[1], -mask._data[2], -mask._data[3] };
        return Mask32x4(inverted);
    }
    friend Mask32x4 operator&(const Mask32x4& lhs, const Mask32x4& rhs)
    {
        Underlying result = { lhs._data[0] & rhs._data[0], lhs._data[1] & rhs._data[1], lhs._data[2] & rhs._data[2], lhs._data[3] & rhs._data[3] };
        return Mask32x4(result);
    }
    friend Mask32x4 operator|(const Mask32x4& lhs, const Mask32x4& rhs)
    {
        Underlying result = { lhs._data[0] | rhs._data[0], lhs._data[1] | rhs._data[1], lhs._data[2] | rhs._data[2], lhs._data[3] | rhs._data[3] };
        return Mask32x4(result);
    }
    friend bool any_of(const Mask32x4& mask)
    {
        return mask._data[0] != 0 || mask._data[1] != 0 || mask._data[2] != 0 || mask._data[3] != 0;
    }
};
struct F32x4 {
    using Underlying = float[4];

    Underlying _data;
    F32x4(float e)
        : _data { e, e, e, e }
    {
    }
    F32x4(float e0, float e1, float e2, float e3)
        : _data { e0, e1, e2, e3 }
    {
    }
    F32x4(const F32x4& other)
    {
        for (int i = 0; i < 4; ++i) {
            _data[i] = other._data[i];
        }
    }
    F32x4(const Underlying& data)
    {
        for (int i = 0; i < 4; ++i) {
            _data[i] = data[i];
        }
    }
    friend F32x4 operator+(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4(lhs._data[0] + rhs._data[0], lhs._data[1] + rhs._data[1], lhs._data[2] + rhs._data[2], lhs._data[3] + rhs._data[3]);
    }
    friend F32x4 operator-(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4(lhs._data[0] - rhs._data[0], lhs._data[1] - rhs._data[1], lhs._data[2] - rhs._data[2], lhs._data[3] - rhs._data[3]);
    }
    friend F32x4 operator*(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4(lhs._data[0] * rhs._data[0], lhs._data[1] * rhs._data[1], lhs._data[2] * rhs._data[2], lhs._data[3] * rhs._data[3]);
    }
    friend F32x4 operator/(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4(lhs._data[0] / rhs._data[0], lhs._data[1] / rhs._data[1], lhs._data[2] / rhs._data[2], lhs._data[3] / rhs._data[3]);
    }
    const float& operator[](const size_t index) const
    {
        return _data[index];
    }
    friend Mask32x4 operator==(const F32x4& lhs, const F32x4& rhs)
    {
        return Mask32x4({ lhs._data[0] == rhs._data[0], lhs._data[1] == rhs._data[1], lhs._data[2] == rhs._data[2], lhs._data[3] == rhs._data[3] });
    }
    friend Mask32x4 operator!=(const F32x4& lhs, const F32x4& rhs)
    {
        return Mask32x4({ lhs._data[0] != rhs._data[0], lhs._data[1] != rhs._data[1], lhs._data[2] != rhs._data[2], lhs._data[3] != rhs._data[3] });
    }
    friend Mask32x4 operator<(const F32x4& lhs, const F32x4& rhs)
    {
        return Mask32x4({ lhs._data[0] < rhs._data[0], lhs._data[1] < rhs._data[1], lhs._data[2] < rhs._data[2], lhs._data[3] < rhs._data[3] });
    }
    friend Mask32x4 operator>(const F32x4& lhs, const F32x4& rhs)
    {
        return Mask32x4({ lhs._data[0] > rhs._data[0], lhs._data[1] > rhs._data[1], lhs._data[2] > rhs._data[2], lhs._data[3] > rhs._data[3] });
    }
    friend Mask32x4 operator<=(const F32x4& lhs, const F32x4& rhs)
    {
        return Mask32x4({ lhs._data[0] <= rhs._data[0], lhs._data[1] <= rhs._data[1], lhs._data[2] <= rhs._data[2], lhs._data[3] <= rhs._data[3] });
    }
    friend Mask32x4 operator>=(const F32x4& lhs, const F32x4& rhs)
    {
        return Mask32x4({ lhs._data[0] >= rhs._data[0], lhs._data[1] >= rhs._data[1], lhs._data[2] >= rhs._data[2], lhs._data[3] >= rhs._data[3] });
    }
    friend F32x4 sqrt(const F32x4& value)
    {
        return F32x4 {
            Stw::Utility::sqrt(static_cast<float>(value._data[0])),
            Stw::Utility::sqrt(static_cast<float>(value._data[1])),
            Stw::Utility::sqrt(static_cast<float>(value._data[2])),
            Stw::Utility::sqrt(static_cast<float>(value._data[3]))
        };
    }
    F32x4 reverse()
    {
        return F32x4(_data[3], _data[2], _data[1], _data[0]);
    }
    F32x4 sq()
    {
        return F32x4(_data[0] * _data[0], _data[1] * _data[1], _data[2] * _data[2], _data[3] * _data[3]);
    }
    friend bool all_eq(const F32x4& lhs, const F32x4& rhs)
    {
        return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];
    }
    friend F32x4 max(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4 {
            Stw::Utility::max(lhs._data[0], rhs._data[0]),
            Stw::Utility::max(lhs._data[1], rhs._data[1]),
            Stw::Utility::max(lhs._data[2], rhs._data[2]),
            Stw::Utility::max(lhs._data[3], rhs._data[3])
        };
    }
    friend F32x4 min(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4 {
            Stw::Utility::min(lhs._data[0], rhs._data[0]),
            Stw::Utility::min(lhs._data[1], rhs._data[1]),
            Stw::Utility::min(lhs._data[2], rhs._data[2]),
            Stw::Utility::min(lhs._data[3], rhs._data[3])
        };
    }
};

#elifdef __wasm__
#include <wasm_simd128.h>
struct Mask32x4 {
    using Underlying = v128_t;

    Underlying _data;
    Mask32x4(const Underlying& mask)
        : _data(mask)
    {
    }
    bool operator[](const size_t index) const
    {
        return *(reinterpret_cast<const float*>(&_data) + index) != 0.0f;
    }
    friend Mask32x4 invert(const Mask32x4& mask)
    {
        return wasm_v128_xor(mask._data, wasm_i32x4_splat(-1));
    }
    friend Mask32x4 operator&(const Mask32x4& lhs, const Mask32x4& rhs)
    {
        return wasm_v128_and(lhs._data, rhs._data);
    }
    friend Mask32x4 operator|(const Mask32x4& lhs, const Mask32x4& rhs)
    {
        return wasm_v128_or(lhs._data, rhs._data);
    }
    friend bool any_of(const Mask32x4& mask)
    {
        return mask[0] != 0 || mask[1] != 0 || mask[2] != 0 || mask[3] != 0;
    }
};
struct F32x4 {
    using Underlying = v128_t;

    Underlying _data;
    F32x4(float e)
        : _data(wasm_f32x4_splat(e))
    {
    }
    F32x4(float e0, float e1, float e2, float e3)
        : _data(wasm_f32x4_make(e0, e1, e2, e3))
    {
    }
    F32x4(const F32x4& other)
        : _data(other._data)
    {
    }
    F32x4(const Underlying& data)
        : _data(data)
    {
    }
    friend F32x4 operator+(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4(wasm_f32x4_add(lhs._data, rhs._data));
    }
    friend F32x4 operator-(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4(wasm_f32x4_sub(lhs._data, rhs._data));
    }
    friend F32x4 operator*(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4(wasm_f32x4_mul(lhs._data, rhs._data));
    }
    friend F32x4 operator/(const F32x4& lhs, const F32x4& rhs)
    {
        return F32x4(wasm_f32x4_div(lhs._data, rhs._data));
    }
    const float& operator[](const size_t index) const
    {
        return *(reinterpret_cast<const float*>(&_data) + index);
    }
    friend Mask32x4 operator==(const F32x4& lhs, const F32x4& rhs)
    {
        return wasm_f32x4_eq(lhs._data, rhs._data);
    }
    friend Mask32x4 operator!=(const F32x4& lhs, const F32x4& rhs)
    {
        return wasm_f32x4_ne(lhs._data, rhs._data);
    }
    friend Mask32x4 operator<(const F32x4& lhs, const F32x4& rhs)
    {
        return wasm_f32x4_lt(lhs._data, rhs._data);
    }
    friend Mask32x4 operator>(const F32x4& lhs, const F32x4& rhs)
    {
        return wasm_f32x4_gt(lhs._data, rhs._data);
    }
    friend Mask32x4 operator<=(const F32x4& lhs, const F32x4& rhs)
    {
        return wasm_f32x4_le(lhs._data, rhs._data);
    }
    friend Mask32x4 operator>=(const F32x4& lhs, const F32x4& rhs)
    {
        return wasm_f32x4_ge(lhs._data, rhs._data);
    }

    friend F32x4 operator&(const F32x4& value, const Mask32x4& mask)
    {
        return wasm_v128_and(value._data, mask._data);
    }
    friend F32x4 sqrt(const F32x4& value)
    {
        return wasm_f32x4_sqrt(value._data);
    }
    F32x4 reverse()
    {
        return wasm_i32x4_shuffle(_data, _data, 3, 2, 1, 0);
    }
    F32x4 sq()
    {
        return _data * _data;
    }
    friend bool all_eq(const F32x4& lhs, const F32x4& rhs)
    {
        return lhs[0] == rhs[0] && lhs[1] == rhs[1] && lhs[2] == rhs[2] && lhs[3] == rhs[3];
    }
    friend F32x4 max(const F32x4& lhs, const F32x4& rhs)
    {
        return wasm_f32x4_max(lhs._data, rhs._data);
    }
    friend F32x4 min(const F32x4& lhs, const F32x4& rhs)
    {
        return wasm_f32x4_min(lhs._data, rhs._data);
    }
};

#elifdef __x86_64__
#include <emmintrin.h> // For SSE2 intrinsics
#include <xmmintrin.h> // For SSE intrinsics
struct Mask32x4 {
    using Underlying = __m128;

    Underlying _data;
    Mask32x4(const Underlying& mask)
    {
        _mm_store_ps((float*)&_data, mask);
    }
    auto operator[](const size_t index) const -> int32_t
    {
        alignas(16) float arr[4];
        _mm_store_ps(reinterpret_cast<float*>(arr), _data);
        return arr[index];
    }
    friend Mask32x4 invert(const Mask32x4& mask)
    {
        return _mm_xor_ps(mask._data, _mm_set1_ps(-1));
    }
    friend Mask32x4 operator&(const Mask32x4& lhs, const Mask32x4& rhs)
    {
        return _mm_and_ps(lhs._data, rhs._data);
    }
    friend Mask32x4 operator|(const Mask32x4& lhs, const Mask32x4& rhs)
    {
        return _mm_or_ps(lhs._data, rhs._data);
    }
    friend bool any_of(const Mask32x4& mask)
    {
        auto zero = _mm_setzero_ps();
        auto cmp = _mm_cmpneq_ps(mask._data, zero);
        return _mm_movemask_ps(cmp) != 0;
    }
};
struct F32x4 {
    using Underlying = __m128;

    Underlying _data;
    F32x4(float e)
        : _data(_mm_set1_ps(e))
    {
    }
    F32x4(float e0, float e1, float e2, float e3)
        : _data(_mm_setr_ps(e0, e1, e2, e3))
    {
    }
    F32x4(const F32x4& other)
        : _data(other._data)
    {
    }
    F32x4(const Underlying& data)
        : _data(data)
    {
    }
    friend F32x4 operator+(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_add_ps(lhs._data, rhs._data);
    }
    friend F32x4 operator-(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_sub_ps(lhs._data, rhs._data);
    }
    friend F32x4 operator*(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_mul_ps(lhs._data, rhs._data);
    }
    friend F32x4 operator/(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_div_ps(lhs._data, rhs._data);
    }
    float operator[](const size_t index) const
    {
        alignas(16) float arr[4];
        _mm_store_ps(arr, _data);
        return arr[index];
    }
    friend Mask32x4 operator==(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_cmpeq_ps(lhs._data, rhs._data);
    }
    friend Mask32x4 operator!=(const F32x4& lhs, const F32x4& rhs)
    {
        F32x4 lhsf(lhs);
        F32x4 rhsf(rhs);
        std::cout << "\t(" << lhsf[0] << ", " << lhsf[1] << ", " << lhsf[2] << ", " << lhsf[3] << ") -> ";
        std::cout << "\t(" << rhsf[0] << ", " << rhsf[1] << ", " << rhsf[2] << ", " << rhsf[3] << ") -> ";
        return Mask32x4 { _mm_cmpneq_ps(lhs._data, rhs._data) };
    }
    friend Mask32x4 operator<(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_cmplt_ps(lhs._data, rhs._data);
    }
    friend Mask32x4 operator>(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_cmpgt_ps(lhs._data, rhs._data);
    }
    friend Mask32x4 operator<=(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_cmple_ps(lhs._data, rhs._data);
    }
    friend Mask32x4 operator>=(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_cmpge_ps(lhs._data, rhs._data);
    }
    friend F32x4 operator&(const F32x4& value, const Mask32x4& mask)
    {
        return _mm_and_ps(value._data, mask._data);
    }
    friend F32x4 sqrt(const F32x4& value)
    {
        return _mm_sqrt_ps(value._data);
    }
    F32x4 reverse()
    {
        return _mm_shuffle_ps(_data, _data, _MM_SHUFFLE(0, 1, 2, 3));
    }
    F32x4 sq()
    {
        return _mm_mul_ps(_data, _data);
    }
    friend bool all_eq(const F32x4& lhs, const F32x4& rhs)
    {
        __m128 cmp_result = _mm_cmpeq_ps(lhs._data, rhs._data);
        alignas(16) float arr[4];
        _mm_store_ps(arr, cmp_result);
        return arr[0] != 0 && arr[1] != 0 && arr[2] != 0 && arr[3] != 0;
    }
    friend F32x4 max(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_max_ps(lhs._data, rhs._data);
    }
    friend F32x4 min(const F32x4& lhs, const F32x4& rhs)
    {
        return _mm_min_ps(lhs._data, rhs._data);
    }
};
#endif

}

#endif // STW_IMPLEMENATION