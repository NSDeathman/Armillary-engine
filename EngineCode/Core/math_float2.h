///////////////////////////////////////////////////////////////
// Created: 26.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <cmath>      // для sqrtf, atan2f и др.
#include <algorithm>  // для std::min, std::max
#include <d3dx9.h>
///////////////////////////////////////////////////////////////
#include "math_half2.h"
#include "math_constants.h"
#include "math_functions.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
class float2;
class float4;
///////////////////////////////////////////////////////////////
// Глобальные функции для float4
inline float2 operator+(float2 lhs, const float2& rhs);
inline float2 operator-(float2 lhs, const float2& rhs);
inline float2 operator*(float2 lhs, const float2& rhs);
inline float2 operator/(float2 lhs, const float2& rhs);
inline float2 operator*(float2 vec, float scalar);
inline float2 operator*(float scalar, float2 vec);
inline float2 operator/(float2 vec, float scalar);
///////////////////////////////////////////////////////////////
class float2
    {
    public:
        float x, y;

        // ============================================================================
        // Конструкторы
        // ============================================================================
        float2() : x(0.0f), y(0.0f) {}
        float2(float x, float y) : x(x), y(y) {}
        float2(float scalar) : x(scalar), y(scalar) {}
        float2(const float2&) = default;

        // Конструкторы из D3D типов
        float2(const D3DXVECTOR2& vec) : x(vec.x), y(vec.y) {}
        float2(const D3DXVECTOR4& vec) : x(vec.x), y(vec.y) {}
        explicit float2(D3DCOLOR color) 
        {
            x = ((color >> 16) & 0xFF) / 255.0f; // R
            y = ((color >> 8) & 0xFF) / 255.0f;  // G
        }
        explicit float2(const float* data) : x(data[0]), y(data[1]) {}

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        float2& operator=(const float2&) = default;
        
        float2& operator=(float scalar)
        {
            x = y = scalar;
            return *this;
        }
        
        float2& operator=(const D3DXVECTOR2& vec)
        {
            x = vec.x;
            y = vec.y;
            return *this;
        }
        
        float2& operator=(D3DCOLOR color)
        {
            x = ((color >> 16) & 0xFF) / 255.0f;
            y = ((color >> 8) & 0xFF) / 255.0f;
            return *this;
        }

        // ============================================================================
        // Арифметические операторы с присваиванием
        // ============================================================================
        float2& operator+=(const float2& rhs) { x += rhs.x; y += rhs.y; return *this; }
        float2& operator-=(const float2& rhs) { x -= rhs.x; y -= rhs.y; return *this; }
        float2& operator*=(const float2& rhs) { x *= rhs.x; y *= rhs.y; return *this; }
        float2& operator/=(const float2& rhs) { x /= rhs.x; y /= rhs.y; return *this; }
        
        float2& operator*=(float scalar) { x *= scalar; y *= scalar; return *this; }
        float2& operator/=(float scalar) { float inv = 1.0f / scalar; x *= inv; y *= inv; return *this; }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        float2 operator+() const { return *this; }
        float2 operator-() const { return float2(-x, -y); }

        // ============================================================================
        // Индексация
        // ============================================================================
        float& operator[](int index) { return (&x)[index]; }
        const float& operator[](int index) const { return (&x)[index]; }

        // ============================================================================
        // Конвертация в другие типы
        // ============================================================================
        operator D3DXVECTOR2() const { return D3DXVECTOR2(x, y); }
        operator const float*() const { return &x; }
        operator float*() { return &x; }

        // ============================================================================
        // Математические функции
        // ============================================================================
        float length() const { return sqrtf(x * x + y * y); }
        float length_sq() const { return x * x + y * y; }
        
        float2 normalized() const 
        { 
            float len = length();
            return (len > 0.0f) ? (*this / len) : float2(0.0f);
        }

        static float dot(const float2& a, const float2& b)
        {
            return a.x * b.x + a.y * b.y;
        }

        static float2 lerp(const float2& a, const float2& b, float t)
        {
            return float2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
        }

        // ============================================================================
        // Swizzle операции
        // ============================================================================
        float2 yx() const { return float2(y, x); }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool isValid() const { return std::isfinite(x) && std::isfinite(y); }
        
        bool approximately(const float2& other, float epsilon = EPSILON) const
        {
            return Core::Math::approximately(x, other.x, epsilon) &&
                   Core::Math::approximately(y, other.y, epsilon);
        }

        std::string to_string() const
        {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "(%.3f, %.3f)", x, y);
            return std::string(buffer);
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(const float2& rhs) const { return approximately(rhs); }
        bool operator!=(const float2& rhs) const { return !(*this == rhs); }
    };

    // ============================================================================
    // Бинарные операторы
    // ============================================================================
    inline float2 operator+(float2 lhs, const float2& rhs) { return lhs += rhs; }
    inline float2 operator-(float2 lhs, const float2& rhs) { return lhs -= rhs; }
    inline float2 operator*(float2 lhs, const float2& rhs) { return lhs *= rhs; }
    inline float2 operator/(float2 lhs, const float2& rhs) { return lhs /= rhs; }

    inline float2 operator*(float2 vec, float scalar) { return vec *= scalar; }
    inline float2 operator*(float scalar, float2 vec) { return vec *= scalar; }
    inline float2 operator/(float2 vec, float scalar) { return vec /= scalar; }

    // ============================================================================
    // Глобальные функции
    // ============================================================================
    inline float distance(const float2& a, const float2& b)
    {
        return (b - a).length();
    }

    inline float dot(const float2& a, const float2& b)
    {
        return float2::dot(a, b);
    }

    inline bool approximately(const float2& a, const float2& b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }

    inline bool isValid(const float2& vec)
    {
        return vec.isValid();
    }

    // ============================================================================
    // D3D совместимость
    // ============================================================================
    inline D3DXVECTOR2 ToD3DXVECTOR2(const float2& vec)
    {
        return D3DXVECTOR2(vec.x, vec.y);
    }

    inline float2 FromD3DXVECTOR2(const D3DXVECTOR2& vec)
    {
        return float2(vec.x, vec.y);
    }

    inline D3DCOLOR ToD3DCOLOR(const float2& color)
    {
        return D3DCOLOR_COLORVALUE(color.x, color.y, 0.0f, 1.0f);
    }

    inline void float2ArrayToD3D(const float2* source, D3DXVECTOR2* destination, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            destination[i] = ToD3DXVECTOR2(source[i]);
    }

    inline void D3DArrayTofloat2(const D3DXVECTOR2* source, float2* destination, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            destination[i] = FromD3DXVECTOR2(source[i]);
    }

    // ============================================================================
    // Полезные константы
    // ============================================================================
    static const float2 float2_Zero(0.0f, 0.0f);
    static const float2 float2_One(1.0f, 1.0f);
    static const float2 float2_UnitX(1.0f, 0.0f);
    static const float2 float2_UnitY(0.0f, 1.0f);
} // namespace Core::Math
///////////////////////////////////////////////////////////////
