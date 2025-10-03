///////////////////////////////////////////////////////////////
// Created: 26.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <cmath>      // для sqrtf, atan2f и др.
#include <algorithm>  // для std::min, std::max
#include <ostream>
///////////////////////////////////////////////////////////////
#include "math_half.h"
#include "math_float2.h"
///////////////////////////////////////////////////////////////
#include "math_constants.h"
#include "math_functions.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
    class half;
    class float2;
    class half2;
    ///////////////////////////////////////////////////////////////
    inline half2 operator+(half2 lhs, const half2& rhs);
    inline half2 operator-(half2 lhs, const half2& rhs);
    inline half2 operator*(half2 lhs, const half2& rhs);
    inline half2 operator/(half2 lhs, const half2& rhs);
    inline half2 operator*(half2 vec, half scalar);
    inline half2 operator*(half scalar, half2 vec);
    inline half2 operator/(half2 vec, half scalar);
    inline half2 operator*(half2 vec, float scalar);
    inline half2 operator*(float scalar, half2 vec);
    inline half2 operator/(half2 vec, float scalar);
    inline half2 operator+(const half2& lhs, const float2& rhs);
    inline half2 operator-(const half2& lhs, const float2& rhs);
    inline half2 operator*(const half2& lhs, const float2& rhs);
    inline half2 operator/(const half2& lhs, const float2& rhs);
    inline half2 operator+(const float2& lhs, const half2& rhs);
    inline half2 operator-(const float2& lhs, const half2& rhs);
    inline half2 operator*(const float2& lhs, const half2& rhs);
    inline half2 operator/(const float2& lhs, const half2& rhs);
    ///////////////////////////////////////////////////////////////
    class half2
    {
    public:
        half x, y;

        // ============================================================================
        // Конструкторы
        // ============================================================================
        half2() : x(half_zero), y(half_zero) {}
        half2(half x, half y) : x(x), y(y) {}
        half2(half scalar) : x(scalar), y(scalar) {}
        half2(float x, float y) : x(x), y(y) {}
        half2(float scalar) : x(scalar), y(scalar) {}
        half2(const half2&) = default;

        // Конструкторы из float2
        half2(const float2& vec) : x(vec.x), y(vec.y) {}

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        half2& operator=(const half2&) = default;
        
        half2& operator=(const float2& vec)
        {
            x = vec.x;
            y = vec.y;
            return *this;
        }

        // ============================================================================
        // Арифметические операторы с присваиванием
        // ============================================================================
        half2& operator+=(const half2& rhs) { x += rhs.x; y += rhs.y; return *this; }
        half2& operator-=(const half2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
        half2& operator*=(const half2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
        half2& operator/=(const half2& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
        
        half2& operator*=(half scalar) { x *= scalar; y *= scalar; return *this; }
        half2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
        half2& operator/=(half scalar) { x /= scalar; y /= scalar; return *this; }
        half2& operator/=(float scalar) { x /= scalar; y /= scalar; return *this; }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        half2 operator+() const { return *this; }
        half2 operator-() const { return half2(-x, -y); }

        // ============================================================================
        // Индексация
        // ============================================================================
        half& operator[](int index) { return (&x)[index]; }
        const half& operator[](int index) const { return (&x)[index]; }

        // ============================================================================
        // Конвертация в другие типы
        // ============================================================================
        explicit operator float2() const { return float2(float(x), float(y)); }

        // ============================================================================
        // Математические функции
        // ============================================================================
        half length() const { return sqrt(length_sq()); }
        half length_sq() const { return x * x + y * y; }
        
        half2 normalize() const 
        { 
            half len = length();
            return (len > half_zero) ? (*this / len) : half2(half_zero);
        }

        static half dot(const half2& a, const half2& b)
        {
            return a.x * b.x + a.y * b.y;
        }

        static half2 lerp(const half2& a, const half2& b, half t)
        {
            return half2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
        }

        static half2 lerp(const half2& a, const half2& b, float t)
        {
            return half2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
        }

        static half2 saturate(const half2& vec)
        {
            return half2(
                std::clamp(float(vec.x), 0.0f, 1.0f),
                std::clamp(float(vec.y), 0.0f, 1.0f)
            );
        }

        // ============================================================================
        // Swizzle операции
        // ============================================================================
        half2 yx() const { return half2(y, x); }

        // ============================================================================
        // Текстурные координаты
        // ============================================================================
        half u() const { return x; }
        half v() const { return y; }
        static half2 uv(half u, half v) { return half2(u, v); }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool isValid() const 
        { 
            return x.isValid() && y.isValid(); 
        }
        
        bool approximately(const half2& other, float epsilon = EPSILON) const
        {
            return x.approximately(other.x, epsilon) && 
                   y.approximately(other.y, epsilon);
        }

        std::string to_string() const
        {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "(%.3f, %.3f)", float(x), float(y));
            return std::string(buffer);
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(const half2& rhs) const { return approximately(rhs); }
        bool operator!=(const half2& rhs) const { return !(*this == rhs); }
    };

    // ============================================================================
    // Бинарные операторы
    // ============================================================================
    inline half2 operator+(half2 lhs, const half2& rhs) { return lhs += rhs; }
    inline half2 operator-(half2 lhs, const half2& rhs) { return lhs -= rhs; }
    inline half2 operator*(half2 lhs, const half2& rhs) { return lhs *= rhs; }
    inline half2 operator/(half2 lhs, const half2& rhs) { return lhs /= rhs; }

    inline half2 operator*(half2 vec, half scalar) { return vec *= scalar; }
    inline half2 operator*(half scalar, half2 vec) { return vec *= scalar; }
    inline half2 operator/(half2 vec, half scalar) { return vec /= scalar; }

    inline half2 operator*(half2 vec, float scalar) { return vec *= scalar; }
    inline half2 operator*(float scalar, half2 vec) { return vec *= scalar; }
    inline half2 operator/(half2 vec, float scalar) { return vec /= scalar; }

    // ============================================================================
    // Операторы с float2
    // ============================================================================
    inline half2 operator+(const half2& lhs, const float2& rhs) 
    { 
        return half2(lhs.x + rhs.x, lhs.y + rhs.y); 
    }
    
    inline half2 operator-(const half2& lhs, const float2& rhs) 
    { 
        return half2(lhs.x - rhs.x, lhs.y - rhs.y); 
    }
    
    inline half2 operator*(const half2& lhs, const float2& rhs) 
    { 
        return half2(lhs.x * rhs.x, lhs.y * rhs.y); 
    }
    
    inline half2 operator/(const half2& lhs, const float2& rhs) 
    { 
        return half2(lhs.x / rhs.x, lhs.y / rhs.y); 
    }

    inline half2 operator+(const float2& lhs, const half2& rhs) 
    { 
        return half2(lhs.x + rhs.x, lhs.y + rhs.y); 
    }
    
    inline half2 operator-(const float2& lhs, const half2& rhs) 
    { 
        return half2(lhs.x - rhs.x, lhs.y - rhs.y); 
    }
    
    inline half2 operator*(const float2& lhs, const half2& rhs) 
    { 
        return half2(lhs.x * rhs.x, lhs.y * rhs.y); 
    }
    
    inline half2 operator/(const float2& lhs, const half2& rhs) 
    { 
        return half2(lhs.x / rhs.x, lhs.y / rhs.y); 
    }

    // ============================================================================
    // Глобальные функции
    // ============================================================================
    inline half distance(const half2& a, const half2& b)
    {
        return (b - a).length();
    }

    inline half dot(const half2& a, const half2& b)
    {
        return half2::dot(a, b);
    }

    inline half2 normalize(const half2& vec)
    {
        return vec.normalize();
    }

    inline half2 lerp(const half2& a, const half2& b, half t)
    {
        return half2::lerp(a, b, t);
    }

    inline half2 lerp(const half2& a, const half2& b, float t)
    {
        return half2::lerp(a, b, t);
    }

    inline half2 saturate(const half2& vec)
    {
        return half2::saturate(vec);
    }

    inline bool approximately(const half2& a, const half2& b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }

    inline bool isValid(const half2& vec)
    {
        return vec.isValid();
    }

    // ============================================================================
    // Конвертация между типами
    // ============================================================================
    inline float2 to_float2(const half2& vec)
    {
        return float2(float(vec.x), float(vec.y));
    }

    inline half2 to_half2(const float2& vec)
    {
        return half2(vec.x, vec.y);
    }

    // ============================================================================
    // Функции для работы с текстурами
    // ============================================================================
    inline half2 clamp(const half2& vec, const half2& min_val, const half2& max_val)
    {
        return half2(
            std::clamp(float(vec.x), float(min_val.x), float(max_val.x)),
            std::clamp(float(vec.y), float(min_val.y), float(max_val.y))
        );
    }

    inline half2 clamp(const half2& vec, float min_val, float max_val)
    {
        return half2(
            std::clamp(float(vec.x), min_val, max_val),
            std::clamp(float(vec.y), min_val, max_val)
        );
    }

    inline half2 frac(const half2& vec)
    {
        return half2(
            vec.x - floor(vec.x),
            vec.y - floor(vec.y)
        );
    }

    // ============================================================================
    // Полезные константы
    // ============================================================================
    static const half2 half2_zero(half_zero);
    static const half2 half2_one(half_one);
    static const half2 half2_unit_x(half_one, half_zero);
    static const half2 half2_unit_y(half_zero, half_one);

    // Текстурные константы
    static const half2 half2_uv_zero(half_zero, half_zero);
    static const half2 half2_uv_one(half_one, half_one);
} // namespace Core::Math
///////////////////////////////////////////////////////////////
