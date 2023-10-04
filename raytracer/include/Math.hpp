/*
    Header only library
    Purpose: Provide a range of constexpr math utilities

    Created by: Willis Medwell
    Date: 9/6/2023
*/
#pragma once

#include <algorithm>
#include <array>
#include <cassert>
#include <cmath>
#include <cstddef>
#include <execution>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>

// Ubiquitous operations and types
namespace Math {
template <typename T>
constexpr T factorial(T n)
{
    T result = 1;
    for (T i = 1; i <= n; ++i) {
        result *= i;
    }
    return result;
}

template <typename T>
constexpr T pow(T x, T exponent)
{
    if (std::is_constant_evaluated()) {
        T result = 1.0;
        for (T i = 0; i < exponent; ++i) {
            result *= x;
        }
        return result;
    } else {
        return std::pow(x, exponent);
    }
}

template <typename T>
constexpr T sqrt(T x)
{
    if consteval {
        if (x < 0.0)
            return std::numeric_limits<T>::quiet_NaN();
        if (x == 0.0 || x == 1.0)
            return x;
        T current = static_cast<T>(x / 2.0);
        T prev;

        for (size_t i = 0; i < 20; i++) {
            prev = current;
            current = static_cast<T>((current + x / current) / 2.0);
        }

        return current;
    } else {
        return std::sqrt(x);
    }
}

template <typename T>
constexpr T cos(T x)
{
    if (std::is_constant_evaluated()) {
        if (x == 0) {
            return 1;
        }
        T result = 0.0;
        for (T i = 0; i < 20; ++i) {
            result += Math::pow(static_cast<T>(-1.0), static_cast<T>(i))
                * Math::pow(x, static_cast<T>(2 * i))
                / Math::factorial(static_cast<T>(2 * i));
        }
        return result;

    } else {
        return static_cast<T>(std::cos(x));
    }
}

template <typename T>
constexpr T sin(T x)
{
    if (std::is_constant_evaluated()) {
        if (x == 0) {
            return 0;
        }
        T result = 0.0;
        for (T i = 0; i < 20; ++i) {
            result += Math::pow(static_cast<T>(-1.0), static_cast<T>(i))
                * Math::pow(x, static_cast<T>(2 * i + 1))
                / Math::factorial(static_cast<T>(2 * i + 1));
        }
        return result;
    } else {
        return static_cast<T>(std::sin(x));
    }
}

template <typename T>
constexpr T tan(T x)
{
    if (std::is_constant_evaluated()) {
        return Math::sin<T>(x) / Math::cos<T>(x);
    } else {
        return static_cast<T>(std::tan(x));
    }
}

struct Radians;
struct Degrees {
    double angle;
    constexpr Degrees(auto degrees)
    {
        angle = static_cast<double>(degrees);
    }
};
struct Radians {
    double angle;
    constexpr Radians()
        : angle(0)
    {
    }
    constexpr Radians(Degrees degrees)
    {
        constexpr double PI = 3.14159265;
        angle = static_cast<double>(degrees.angle) * PI / 180.0;
    }
    constexpr Radians(auto radians)
    {
        angle = static_cast<double>(radians);
    }
    constexpr bool operator==(Radians other) const
    {
        return this->angle == other.angle;
    }
};

}

// Activation functions (Machine learning)
namespace Math::Activation {

template <typename T>
constexpr T linear(T x)
{
    return x;
}

template <typename T>
constexpr T reLU(T x)
{
    return std::max(static_cast<T>(0), x);
}

template <typename T>
constexpr T heaviside(T x)
{
    return static_cast<T>(x > 0);
}

template <std::floating_point T>
constexpr T sigmoid(T x)
{
    return 1 / (1 + std::exp(-x));
}

template <std::floating_point T>
constexpr T gELU(T x)
{
    return (x / 2) * (1 + x / Math::sqrt<T>(2));
}

template <std::floating_point T>
constexpr T siLU(T x)
{
    return x / (1 + std::exp(-x));
}

template <std::floating_point T>
constexpr T gaussian(T x)
{
    return std::exp(-(x * x));
}

template <std::floating_point T>
constexpr T tanh(T x)
{
    return std::tanh(x);
}

template <std::floating_point T>
constexpr T softplus(T x)
{
    return std::log(1 + std::exp(x));
}

template <std::floating_point T>
constexpr T leakyReLU(T x)
{
    return (x > 0) ? x : static_cast<T>(0.01) * x;
}

struct Linear {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::linear(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const Linear& activation [[maybe_unused]])
    {
        os << "Math::Activation::Linear";
        return os;
    }
};
struct ReLU {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::reLU(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const ReLU& activation [[maybe_unused]])
    {
        os << "Math::Activation::ReLU";
        return os;
    }
};
struct Heaviside {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::heaviside(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const Heaviside& activation [[maybe_unused]])
    {
        os << "Math::Activation::Heaviside";
        return os;
    }
};
struct Sigmoid {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::sigmoid(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const Sigmoid& activation [[maybe_unused]])
    {
        os << "Math::Activation::Sigmoid";
        return os;
    }
};
struct GELU {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::gELU(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const GELU& activation [[maybe_unused]])
    {
        os << "Math::Activation::GELU";
        return os;
    }
};
struct SiLU {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::siLU(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const SiLU& activation [[maybe_unused]])
    {
        os << "Math::Activation::SiLU";
        return os;
    }
};
struct Gaussian {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::gaussian(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const Gaussian& activation [[maybe_unused]])
    {
        os << "Math::Activation::Gaussian";
        return os;
    }
};
struct Tanh {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::tanh(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const Tanh& activation [[maybe_unused]])
    {
        os << "Math::Activation::Tanh";
        return os;
    }
};
struct Softplus {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::softplus(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const Softplus& activation [[maybe_unused]])
    {
        os << "Math::Activation::Softplus";
        return os;
    }
};
struct LeakyReLU {
    constexpr auto operator()(const auto& x) const
    {
        return Math::Activation::leakyReLU(x);
    }
    friend std::ostream& operator<<(std::ostream& os, const LeakyReLU& activation [[maybe_unused]])
    {
        os << "Math::Activation::LeakyReLU";
        return os;
    }
};

}

// Linear Algebra (Graphics and 3D)
namespace Math::LinearAlgebra {
// Type declarations
template <size_t N, typename T = float>
class Pos;
template <size_t N, typename T = float>
class Vec;
template <size_t N, typename T = float>
class Ray;
template <size_t R, size_t C, typename T = float>
class Mat;

template <size_t N, typename T>
class Vec {
public:
    T data[N];

    constexpr Vec()
        : data()
    {
    }

    template <typename... Args>
    constexpr Vec(Args... args)
        : data { static_cast<T>(args)... }
    {
        static_assert(sizeof...(args) == N);
    }

    constexpr explicit Vec(const Pos<N, T>& pos);
    constexpr explicit operator Pos<N, T>() const;

    constexpr auto begin() -> T*
    {
        return data;
    }
    constexpr auto begin() const -> const T*
    {
        return data;
    }
    constexpr auto cbegin() const -> const T*
    {
        return data;
    }
    constexpr auto end() -> T*
    {
        return data + N;
    }
    constexpr auto end() const -> const T*
    {
        return data + N;
    }
    constexpr auto cend() const -> const T*
    {
        return data + N;
    }

    template <size_t S>
    constexpr bool operator==(const Vec<S, T>& other) const
    {
        constexpr float TOLERANCE = static_cast<float>(1e-6);
        auto abs = [](auto x) { return (x < 0) ? -x : x; };

        if constexpr (S != N) {
            return false;
        } else {
            return std::ranges::equal(*this, other, [&](const T& lhs, const T& rhs) { return abs(lhs - rhs) < TOLERANCE; });
        }
    }

    template <size_t S>
    constexpr bool operator!=(const Vec<S, T>& other) const
    {
        return !(*this == other);
    }

    constexpr Vec<N, T> operator+(const Vec<N, T>& other) const
    {
        Vec<N, T> result = *this;
        std::ranges::transform(*this, other, result.begin(), std::plus {});
        return result;
    }
    constexpr Vec<N, T> operator-(const Vec<N, T>& other) const
    {
        Vec<N, T> result = *this;
        std::ranges::transform(*this, other, result.begin(), std::minus {});
        return result;
    }
    constexpr Vec<N, T> operator/(const Vec<N, T>& other) const
    {
        Vec<N, T> result = *this;
        std::ranges::transform(*this, other, result.begin(), std::divides {});
        return result;
    }
    constexpr Vec<N, T> operator*(const Vec<N, T>& other) const
    {
        Vec<N, T> result = *this;
        std::ranges::transform(*this, other, result.begin(), std::multiplies {});
        return result;
    }

    constexpr Vec<N, T> operator/(T scalar) const
    {
        Vec<N, T> result = *this;
        std::ranges::transform(*this, result.begin(), [&](auto& e) { return e / scalar; });
        return result;
    }
    constexpr Vec<N, T> operator*(T scalar) const
    {
        Vec<N, T> result = *this;
        for (auto& e : result) {
            e *= scalar;
        }
        return result;
    }

    friend constexpr Vec<N, T> operator*(T scalar, const Vec<N, T>& vector)
    {
        return vector * scalar;
    }
    friend constexpr Vec<N, T> operator/(T scalar, const Vec<N, T>& vector)
    {
        return vector / scalar;
    }

    constexpr Vec<N, T> operator-() const
    {
        return -1 * (*this);
    }

    constexpr T& operator[](size_t index)
    {
        assert(index < N);
        return data[index];
    }
    constexpr const T& operator[](size_t index) const
    {
        assert(index < N);
        return data[index];
    }

    constexpr T getLengthSquared() const
    {
        return std::transform_reduce(
            cbegin(), cend(),
            cbegin(),
            T {},
            std::plus {},
            std::multiplies {});
    }
    constexpr T getLength() const
    {
        return Math::sqrt(getLengthSquared());
    }

    constexpr Vec<N, T> getNormalised() const
    {
        return (*this) / getLength();
    }

    constexpr size_t size() const
    {
        return N;
    }

    friend std::ostream& operator<<(std::ostream& os, const Vec<N, T>& vec)
    {
        os << "{ ";
        for (const auto& e : vec) {
            os << e << " ";
        }
        os << '}';
        return os;
    }
};

template <size_t N, typename T>
class Pos {
public:
    T data[N];

    constexpr auto begin() -> T*
    {
        return data;
    }
    constexpr auto begin() const -> const T*
    {
        return data;
    }
    constexpr auto cbegin() const -> const T*
    {
        return data;
    }
    constexpr auto end() -> T*
    {
        return data + N;
    }
    constexpr auto end() const -> const T*
    {
        return data + N;
    }
    constexpr auto cend() const -> const T*
    {
        return data + N;
    }

    constexpr explicit operator Vec<N, T>() const;
    constexpr bool operator==(const Pos<N, T>& other) const
    {
        constexpr float TOLERANCE = static_cast<float>(1e-6);
        auto abs = [](auto x) { return (x < 0) ? -x : x; };
        return std::ranges::equal(*this, other, [&](const T& lhs, const T& rhs) { return abs(lhs - rhs) < TOLERANCE; });
    }
    constexpr T& operator[](size_t index)
    {
        return data[index];
    }
    constexpr const T& operator[](size_t index) const
    {
        return data[index];
    }
};

template <size_t N, typename T>
constexpr Pos<N, T>::operator Vec<N, T>() const
{
    Vec<N, T> vec;
    std::ranges::copy(this->data, this->data + N, vec.begin());
    return vec;
}
template <size_t N, typename T>
constexpr Vec<N, T>::operator Pos<N, T>() const
{
    Pos<N, T> pos;
    std::ranges::copy(this->cbegin(), this->cend(), pos.data);
    return pos;
}
template <size_t N, typename T>
constexpr Vec<N, T>::Vec(const Pos<N, T>& pos)
{
    std::ranges::copy(pos.data, pos.data + N, this->begin());
}

template <size_t N, typename T>
class Ray {
private:
    Pos<N, T> m_origin;
    Vec<N, T> m_direction;

public:
    constexpr Ray()
        : m_origin()
        , m_direction()
    {
    }
    constexpr Ray(const Pos<N, T>& origin, const Vec<N, T>& direction)
        : m_origin(origin)
        , m_direction(direction.getNormalised())
    {
    }
    constexpr Ray(const Ray<N, T>& other)
        : m_origin(other.m_origin)
        , m_direction(other.m_direction)
    {
    }
    constexpr Ray& operator=(const Ray<N, T>& other)
    {
        m_origin = other.m_origin;
        m_direction = other.m_direction;
        return *this;
    }
    constexpr auto getOrigin() const -> const Pos<N, T>&
    {
        return m_origin;
    }
    constexpr auto getDirection() const -> const Vec<N, T>&
    {
        return m_direction;
    }
    constexpr void setOrigin(const Pos<N, T>& origin)
    {
        m_origin = origin;
    }
    constexpr void setDirection(const Vec<N, T>& direction)
    {
        m_direction = direction.getNormalised();
    }
    constexpr LinearAlgebra::Pos<N, T> getPointAlongRay(T t) const
    {
        Pos<N, T> point;
        for (size_t i = 0; i < N; i++) {
            point[i] = m_origin[i] + (m_direction[i] * t);
        }
        return point;
    }

    friend std::ostream& operator<<(std::ostream& os, const Ray<N, T>& ray)
    {
        os << "origin: " << static_cast<LinearAlgebra::Vec<N, T>>(ray.getOrigin()) << ", direction: " << ray.getDirection();
        return os;
    }
};

template <size_t R, size_t C, typename T>
class Mat {
public:
    static constexpr size_t N = R * C;
    T data[N];
    constexpr Mat()
        : data()
    {
    }
    constexpr Mat(T fill_value)
    {
        std::fill(data, data + N, fill_value);
    }

    constexpr Mat(const std::initializer_list<std::initializer_list<T>>& elements)
    {
        // C++23
        // auto flattened = std::views::join(elements);
        // std::ranges::copy(flattened, data);

        // lowkey hate this.
        size_t i = 0;
        for (const auto& column : elements) {
            for (const auto& element : column) {
                data[i] = element;
                i++;
            }
        }
    }

    constexpr auto begin() -> T*
    {
        return data;
    }
    constexpr auto begin() const -> const T*
    {
        return data;
    }
    constexpr auto cbegin() const -> const T*
    {
        return data;
    }
    constexpr auto end() -> T*
    {
        return data + N;
    }
    constexpr auto end() const -> const T*
    {
        return data + N;
    }
    constexpr auto cend() const -> const T*
    {
        return data + N;
    }
    constexpr T* operator[](size_t row)
    {
        assert(row < R);
        return data + (row * C);
    }
    constexpr const T* operator[](size_t row) const
    {
        assert(row < R);
        return data + (row * C);
    }

    template <size_t R2, size_t C2>
    constexpr bool operator==(const Mat<R2, C2, T>& other) const
    {
        constexpr float TOLERANCE = static_cast<float>(1e-4);
        auto abs = [](auto x) { return (x < 0) ? -x : x; };

        if constexpr (R != R2 || C != C2) {
            return false;
        } else {
            return std::ranges::equal(*this, other, [&](const T& lhs, const T& rhs) { return abs(lhs - rhs) < TOLERANCE; });
        }
    }
    template <size_t R2, size_t C2>
    constexpr bool operator!=(const Mat<R2, C2, T>& other) const
    {
        return !(*this == other);
    }

    constexpr Mat<R, C, T> operator*(T scalar)
    {
        Mat<R, C, T> out;
        std::ranges::transform(*this, out.begin(), [&](const T& e) { return e * scalar; });
        return out;
    }

    template <size_t R2, size_t C2>
    constexpr Mat<R, C2> operator*(const Mat<R2, C2, T>& b) const
    {
        const auto& a = *this;
        Mat<R, C2, T> c {};
        for (size_t row = 0; row < R; ++row) {
            for (size_t col = 0; col < C2; ++col) {
                T value = {};
                for (size_t i = 0; i < C; ++i) {
                    value += a[row][i] * b[i][col];
                }
                c[row][col] = value;
            }
        }
        return c;
    }

    friend std::ostream& operator<<(std::ostream& os, const Mat<R, C, T>& mat)
    {
        os << '[';
        for (size_t i = 0; i < R; ++i) {
            os << "{ ";
            for (size_t j = 0; j < C; ++j) {
                os << mat[i][j] << ' ';
            }
            os << '}';
        }
        os << ']';
        return os;
    }
};
struct Radian;

template <typename T = float>
class Quat {
private:
    static constexpr size_t W = 0;
    static constexpr size_t X = 1;
    static constexpr size_t Y = 2;
    static constexpr size_t Z = 3;

public:
    Vec<4, T> data;
    constexpr Quat(Radians x_rotation, Radians y_rotation, Radians z_rotation)
    {
        // T cr = Math::cos<T>(static_cast<T>(x_rotation.angle / 2)); // roll
        // T cp = Math::cos<T>(static_cast<T>(y_rotation.angle / 2)); // pitch
        // T cy = Math::cos<T>(static_cast<T>(z_rotation.angle / 2)); // yaw

        // T sr = Math::sin<T>(static_cast<T>(x_rotation.angle / 2)); // roll
        // T sp = Math::sin<T>(static_cast<T>(y_rotation.angle / 2)); // pitch
        // T sy = Math::sin<T>(static_cast<T>(z_rotation.angle / 2)); // yaw

        // data[W] = cr * cp * cy + sr * sp * sy;
        // data[X] = sr * cp * cy - cr * sp * sy;
        // data[Y] = cr * sp * cy + sr * cp * sy;
        // data[Z] = cr * cp * sy - sr * sp * cy;
        Quat<T> z { Math::cos(static_cast<T>(z_rotation.angle / 2)), 0, 0, Math::sin(static_cast<T>(y_rotation.angle / 2)) };
        Quat<T> y { Math::cos(static_cast<T>(y_rotation.angle / 2)), 0, Math::sin(static_cast<T>(y_rotation.angle / 2)), 0 };
        Quat<T> x { Math::cos(static_cast<T>(x_rotation.angle / 2)), Math::sin(static_cast<T>(x_rotation.angle / 2)), 0, 0 };
        *this = z * y * x;
    }

    constexpr Quat()
    {
        data[W] = 1;
    }
    constexpr Quat(T w, T x, T y, T z)
    {
        data[W] = w;
        data[X] = x;
        data[Y] = y;
        data[Z] = z;
    }

    constexpr Quat(T w, Vec<3, float> vec)
    {
        data[W] = w;
        data[X] = vec[0];
        data[Y] = vec[1];
        data[Z] = vec[2];
    }

    constexpr Quat<T> operator*(const Quat<T>& other) const
    {
        const auto& w1 = this->data[W];
        const auto& x1 = this->data[X];
        const auto& y1 = this->data[Y];
        const auto& z1 = this->data[Z];
        const auto& w2 = other.data[W];
        const auto& x2 = other.data[X];
        const auto& y2 = other.data[Y];
        const auto& z2 = other.data[Z];

        return Quat<T> {
            w1 * w2 - x1 * x2 - y1 * y2 - z1 * z2,
            w1 * x2 + x1 * w2 + y1 * z2 - z1 * y2,
            w1 * y2 - x1 * z2 + y1 * w2 + z1 * x2,
            w1 * z2 + x1 * y2 - y1 * x2 + z1 * w2
        };
    }
    constexpr Vec<3, T> getVec() const
    {
        return Vec<3, T> {
            data[X] / data[W],
            data[Y] / data[W],
            data[Z] / data[W],
        };
    }
    constexpr bool operator==(const Quat<T>& other) const
    {
        return std::ranges::equal(this->data, other.data);
    }

    friend std::ostream& operator<<(std::ostream& os, const Quat<T>& quat)
    {
        std::cout << quat.data;
        return os;
    }
};

template <size_t R, size_t C, size_t N, typename T>
constexpr Vec<N, T> dotProduct(const Mat<R, C, T>& mat, const Vec<N, T>& vec)
{
    static_assert(N == C, "Incompatible operation");
    Vec<R, T> output;

    for (size_t row = 0; row < R; ++row) {
        T value = {};
        for (size_t col = 0; col < C; ++col) {
            value += mat[row][col] * vec[col];
        }
        output[row] = value;
    }
    return output;
}

template <size_t R, size_t C, size_t N, typename T>
constexpr Vec<N, T> dotProduct(const Vec<N, T>& vec, const Mat<R, C, T>& mat)
{
    static_assert(N == C, "Incompatible operation");
    Vec<R, T> output;

    for (size_t col = 0; col < C; ++col) {
        T value = {};
        for (size_t row = 0; row < R; ++row) {
            value += mat[row][col] * vec[row];
        }
        output[col] = value;
    }
    return output;
}

template <size_t N, typename T>
constexpr T dotProduct(const Vec<N, T>& vec1, const Vec<N, T>& vec2)
{
    return std::transform_reduce(
        vec1.cbegin(), vec1.cend(),
        vec2.cbegin(),
        T {},
        std::plus {},
        std::multiplies {});
}

template <size_t R, size_t C, typename T>
constexpr Mat<C, R, T> transpose(const Mat<R, C, T>& mat)
{
    Mat<C, R, T> transposed;
    for (size_t row = 0; row < R; row++) {
        for (size_t col = 0; col < C; col++) {
            transposed[col][row] = mat[row][col];
        }
    }
    return transposed;
}

/*
Determinant of a Matrix (only capable of 2x2 and 3x3).
*/
template <size_t N, typename T>
constexpr T determinant(const Mat<N, N, T>& mat [[maybe_unused]])
{
    assert(false);
    return 0.0f;
}

/*
Determinant of a Matrix (only capable of 2x2 and 3x3).
*/
template <typename T>
constexpr T determinant(const Mat<2, 2, T>& mat)
{
    return (mat[0][0] * mat[1][1]) - (mat[0][1] * mat[1][0]);
}

/*
Determinant of a Matrix (only capable of 2x2 and 3x3).
*/
template <typename T>
constexpr T determinant(const Mat<3, 3, T>& mat)
{
    Mat<2, 2> a({ { mat[1][1], mat[1][2] }, { mat[2][1], mat[2][2] } });
    Mat<2, 2> b({ { mat[1][0], mat[1][2] }, { mat[2][0], mat[2][2] } });
    Mat<2, 2> c({ { mat[1][0], mat[1][1] }, { mat[2][0], mat[2][1] } });
    return (mat[0][0] * determinant(a)) - (mat[0][1] * determinant(b)) + (mat[0][2] * determinant(c));
}

/*
Distance between two vectors (as if they were points).
*/
template <size_t N, typename T>
constexpr T distance(const Pos<N, T>& pos1, const Pos<N, T>& pos2)
{
    const Vec<N, T> vec1 { pos1 };
    const Vec<N, T> vec2 { pos2 };

    auto squaredDifference = [](const auto& lhs, const auto& rhs) { return (lhs - rhs) * (lhs - rhs); };

    return Math::sqrt(
        std::transform_reduce(
            vec1.begin(), vec1.end(),
            vec2.begin(),
            0.0f,
            std::plus {},
            squaredDifference));
}

/*
Generate the rotation Matrix with the inputs of yaw, pitch, and roll.
(Input units are in radians).
*/
template <typename T>
constexpr Mat<3, 3, T> getRotationMat3x3(Radians x_rotation_radians, Radians y_rotation_radians, Radians z_rotation_radians)
{
    const T x = static_cast<T>(x_rotation_radians.angle);
    const T y = static_cast<T>(y_rotation_radians.angle);
    const T z = static_cast<T>(z_rotation_radians.angle);

    namespace LA = Math::LinearAlgebra;

    Mat<3, 3, T> x_rot_mat(
        { { 1, 0, 0 },
            { 0, Math::cos(x), -Math::sin(x) },
            { 0, Math::sin(x), Math::cos(x) } });

    Mat<3, 3, T> y_rot_mat(
        { { Math::cos(y), 0, Math::sin(y) },
            { 0, 1, 0 },
            { -Math::sin(y), 0, Math::cos(y) } });

    Mat<3, 3, T> z_rot_mat(
        { { Math::cos(z), -Math::sin(z), 0 },
            { Math::sin(z), Math::cos(z), 0 },
            { 0, 0, 1 } });

    return x_rot_mat * y_rot_mat * z_rot_mat;
}

template <typename T = float>
class Sphere3D {
public:
    Pos<3, T> center;
    T radius;
    constexpr Sphere3D(const Pos<3, T>& center_point, const T radius_length)
        : center(center_point)
        , radius(radius_length)
    {
    }
};

template <typename T>
class Triangle3D {
    Pos<3, T> corners[3];
    constexpr Triangle3D(const Pos<3, T>& a, const Pos<3, T>& b, const Pos<3, T>& c)
        : corners({ a, b, c })
    {
    }
};

/*
    Returns the distance for the closet intersection IN FRONT OF ray.
*/
template <typename T>
constexpr T intersectionDist(const Ray<3, T>& ray, const Sphere3D<T>& sphere)
{
    const auto displacement = static_cast<Vec<3, T>>(ray.getOrigin()) - static_cast<Vec<3, T>>(sphere.center);
    const T A = dotProduct(ray.getDirection(), ray.getDirection());
    const T B = T { 2 } * dotProduct(displacement, ray.getDirection());
    const T C = dotProduct(displacement, displacement) - (sphere.radius * sphere.radius);
    const T D = static_cast<T>(B * B - 4.0 * A * C);

    constexpr T t_min = static_cast<T>(0.0001);

    T t = 0;
    if (D > 0) {
        T t1 = (-B + Math::sqrt(D)) / (2.0f * A);
        T t2 = (-B - Math::sqrt(D)) / (2.0f * A);
        if (t1 > t_min && t2 > t_min) {
            t = std::min(t1, t2);
        } else if (t1 > t_min && t2 < t_min) {
            t = t1;
        } else if (t1 < t_min && t2 > t_min) {
            t = t2;
        }
    }
    return t;
}

template <typename T = float>
constexpr Vec<3, T> getNormalVec(const Pos<3, T>& hit_point, const Sphere3D<T>& sphere)
{
    return (static_cast<Vec<3, T>>(hit_point) - static_cast<Vec<3, T>>(sphere.center)) / sphere.radius;
}

template <typename T = float>
constexpr Vec<3, T> getReflected(const Vec<3, T>& vec, const Vec<3, T>& normal)
{
    return vec - (normal * (2 * dotProduct(vec, normal)));
}

template <typename T = float>
constexpr Vec<3, T> getRefracted(const Vec<3, T>& vec, const Vec<3, T>& normal, T refractive_index_ratio)
{
    T cos_theta = std::min(dotProduct(-vec, normal), T { 1 });
    Vec<3> perpendicular = refractive_index_ratio * (vec + cos_theta * normal);
    Vec<3> parallel = -Math::sqrt(std::abs(1 - perpendicular.getLengthSquared())) * normal;
    return perpendicular + parallel;
}

}
