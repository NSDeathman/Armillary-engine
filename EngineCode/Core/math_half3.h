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
#include "math_float3.h"
#include "math_half2.h"
///////////////////////////////////////////////////////////////
#include "math_constants.h"
#include "math_functions.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
	class half3;
	///////////////////////////////////////////////////////////////
	inline half3 operator+(half3 lhs, const half3& rhs);
	inline half3 operator-(half3 lhs, const half3& rhs);
	inline half3 operator*(half3 lhs, const half3& rhs);
	inline half3 operator/(half3 lhs, const half3& rhs);
	inline half3 operator*(half3 vec, half scalar);
	inline half3 operator*(half scalar, half3 vec);
	inline half3 operator/(half3 vec, half scalar);
	inline half3 operator*(half3 vec, float scalar);
	inline half3 operator*(float scalar, half3 vec);
	inline half3 operator/(half3 vec, float scalar);
	inline half3 operator+(const half3& lhs, const float3& rhs);
	inline half3 operator-(const half3& lhs, const float3& rhs);
	inline half3 operator*(const half3& lhs, const float3& rhs);
	inline half3 operator/(const half3& lhs, const float3& rhs);
	inline half3 operator+(const float3& lhs, const half3& rhs);
	inline half3 operator-(const float3& lhs, const half3& rhs);
	inline half3 operator*(const float3& lhs, const half3& rhs);
	inline half3 operator/(const float3& lhs, const half3& rhs);
	///////////////////////////////////////////////////////////////
    class half3
    {
    public:
        half x, y, z;

        // ============================================================================
        // Конструкторы
        // ============================================================================
        half3() : x(half_zero), y(half_zero), z(half_zero) {}
        half3(half x, half y, half z) : x(x), y(y), z(z) {}
        half3(half scalar) : x(scalar), y(scalar), z(scalar) {}
        half3(float x, float y, float z) : x(x), y(y), z(z) {}
        half3(float scalar) : x(scalar), y(scalar), z(scalar) {}
        half3(const half3&) = default;

        // Конструкторы из других типов
        half3(const half2& vec, half z) : x(vec.x), y(vec.y), z(z) {}
        half3(const float3& vec) : x(vec.x), y(vec.y), z(vec.z) {}
        half3(const float2& vec, float z) : x(vec.x), y(vec.y), z(z) {}

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        half3& operator=(const half3&) = default;
        
        half3& operator=(const float3& vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            return *this;
        }

        // ============================================================================
        // Арифметические операторы с присваиванием
        // ============================================================================
        half3& operator+=(const half3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
        half3& operator-=(const half3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
        half3& operator*=(const half3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
        half3& operator/=(const half3& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
        
        half3& operator*=(half scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
        half3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
        half3& operator/=(half scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }
        half3& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        half3 operator+() const { return *this; }
        half3 operator-() const { return half3(-x, -y, -z); }

        // ============================================================================
        // Индексация
        // ============================================================================
        half& operator[](int index) { return (&x)[index]; }
        const half& operator[](int index) const { return (&x)[index]; }

        // ============================================================================
        // Конвертация в другие типы
        // ============================================================================
        explicit operator float3() const { return float3(float(x), float(y), float(z)); }

        // ============================================================================
        // Математические функции
        // ============================================================================
        half length() const { return sqrt(length_sq()); }
        half length_sq() const { return x * x + y * y + z * z; }
        
        half3 normalize() const 
        { 
            half len = length();
            return (len > half_zero) ? (*this / len) : half3(half_zero);
        }

        static half dot(const half3& a, const half3& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        static half3 cross(const half3& a, const half3& b)
        {
            return half3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            );
        }

        static half3 lerp(const half3& a, const half3& b, half t)
        {
            return half3(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t
            );
        }

        static half3 lerp(const half3& a, const half3& b, float t)
        {
            return half3(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t
            );
        }

        static half3 saturate(const half3& vec)
        {
            return half3(
                std::clamp(float(vec.x), 0.0f, 1.0f),
                std::clamp(float(vec.y), 0.0f, 1.0f),
                std::clamp(float(vec.z), 0.0f, 1.0f)
            );
        }

        static half3 reflect(const half3& incident, const half3& normal)
        {
            half dot_val = dot(incident, normal);
            return incident - half(2.0f) * dot_val * normal;
        }

        // ============================================================================
        // Swizzle операции
        // ============================================================================
        half2 xy() const { return half2(x, y); }
        half2 xz() const { return half2(x, z); }
        half2 yz() const { return half2(y, z); }

        half3 yxz() const { return half3(y, x, z); }
        half3 zxy() const { return half3(z, x, y); }
        half3 zyx() const { return half3(z, y, x); }
        half3 xzy() const { return half3(x, z, y); }

        // ============================================================================
        // Цветовые операции
        // ============================================================================
        half luminance() const
        {
            return 0.299f * x + 0.587f * y + 0.114f * z;
        }

        half3 rgb_to_grayscale() const
        {
            half luma = luminance();
            return half3(luma, luma, luma);
        }

        half3 gamma_correct(half gamma) const
        {
            return half3(pow(x, gamma), pow(y, gamma), pow(z, gamma));
        }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool isValid() const 
        { 
            return x.isValid() && y.isValid() && z.isValid(); 
        }
        
        bool approximately(const half3& other, float epsilon = EPSILON) const
        {
            return x.approximately(other.x, epsilon) && 
                   y.approximately(other.y, epsilon) && 
                   z.approximately(other.z, epsilon);
        }

        std::string to_string() const
        {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f)", float(x), float(y), float(z));
            return std::string(buffer);
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(const half3& rhs) const { return approximately(rhs); }
        bool operator!=(const half3& rhs) const { return !(*this == rhs); }
    };

    // ============================================================================
    // Бинарные операторы
    // ============================================================================
    inline half3 operator+(half3 lhs, const half3& rhs) { return lhs += rhs; }
    inline half3 operator-(half3 lhs, const half3& rhs) { return lhs -= rhs; }
    inline half3 operator*(half3 lhs, const half3& rhs) { return lhs *= rhs; }
    inline half3 operator/(half3 lhs, const half3& rhs) { return lhs /= rhs; }

    inline half3 operator*(half3 vec, half scalar) { return vec *= scalar; }
    inline half3 operator*(half scalar, half3 vec) { return vec *= scalar; }
    inline half3 operator/(half3 vec, half scalar) { return vec /= scalar; }

    inline half3 operator*(half3 vec, float scalar) { return vec *= scalar; }
    inline half3 operator*(float scalar, half3 vec) { return vec *= scalar; }
    inline half3 operator/(half3 vec, float scalar) { return vec /= scalar; }

    // ============================================================================
    // Операторы с float3
    // ============================================================================
    inline half3 operator+(const half3& lhs, const float3& rhs) 
    { 
        return half3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); 
    }
    
    inline half3 operator-(const half3& lhs, const float3& rhs) 
    { 
        return half3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); 
    }
    
    inline half3 operator*(const half3& lhs, const float3& rhs) 
    { 
        return half3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); 
    }
    
    inline half3 operator/(const half3& lhs, const float3& rhs) 
    { 
        return half3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); 
    }

    inline half3 operator+(const float3& lhs, const half3& rhs) 
    { 
        return half3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z); 
    }
    
    inline half3 operator-(const float3& lhs, const half3& rhs) 
    { 
        return half3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z); 
    }
    
    inline half3 operator*(const float3& lhs, const half3& rhs) 
    { 
        return half3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z); 
    }
    
    inline half3 operator/(const float3& lhs, const half3& rhs) 
    { 
        return half3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z); 
    }

    // ============================================================================
    // Глобальные функции
    // ============================================================================
    inline half distance(const half3& a, const half3& b)
    {
        return (b - a).length();
    }

    inline half dot(const half3& a, const half3& b)
    {
        return half3::dot(a, b);
    }

    inline half3 cross(const half3& a, const half3& b)
    {
        return half3::cross(a, b);
    }

    inline half3 normalize(const half3& vec)
    {
        return vec.normalize();
    }

    inline half3 lerp(const half3& a, const half3& b, half t)
    {
        return half3::lerp(a, b, t);
    }

    inline half3 lerp(const half3& a, const half3& b, float t)
    {
        return half3::lerp(a, b, t);
    }

    inline half3 saturate(const half3& vec)
    {
        return half3::saturate(vec);
    }

    inline half3 reflect(const half3& incident, const half3& normal)
    {
        return half3::reflect(incident, normal);
    }

    inline bool approximately(const half3& a, const half3& b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }

    inline bool isValid(const half3& vec)
    {
        return vec.isValid();
    }

    inline bool isNormalized(const half3& vec, float epsilon = EPSILON)
    {
		return approximately(vec.length_sq(), half_one, epsilon);
    }

    // ============================================================================
    // Конвертация между типами
    // ============================================================================
    inline float3 to_float3(const half3& vec)
    {
        return float3(float(vec.x), float(vec.y), float(vec.z));
    }

    inline half3 to_half3(const float3& vec)
    {
        return half3(vec.x, vec.y, vec.z);
    }

    // ============================================================================
    // Цветовые функции
    // ============================================================================
    inline half3 rgb_to_grayscale(const half3& rgb)
    {
        return rgb.rgb_to_grayscale();
    }

    inline half luminance(const half3& rgb)
    {
        return rgb.luminance();
    }

    inline half3 gamma_correct(const half3& color, half gamma)
    {
        return color.gamma_correct(gamma);
    }

    // ============================================================================
    // Геометрические функции
    // ============================================================================
    inline half3 clamp(const half3& vec, const half3& min_val, const half3& max_val)
    {
        return half3(
            std::clamp(float(vec.x), float(min_val.x), float(max_val.x)),
            std::clamp(float(vec.y), float(min_val.y), float(max_val.y)),
            std::clamp(float(vec.z), float(min_val.z), float(max_val.z))
        );
    }

    inline half3 clamp(const half3& vec, float min_val, float max_val)
    {
        return half3(
            std::clamp(float(vec.x), min_val, max_val),
            std::clamp(float(vec.y), min_val, max_val),
            std::clamp(float(vec.z), min_val, max_val)
        );
    }

    inline half3 frac(const half3& vec)
    {
        return half3(
            vec.x - floor(vec.x),
            vec.y - floor(vec.y),
            vec.z - floor(vec.z)
        );
    }

    inline half3 ensure_normalized(const half3& normal)
    {
        half len_sq = normal.length_sq();
        if (len_sq > half_zero)
        {
            half len = sqrt(len_sq);
            if (abs(len - half_one) > half(0.01f))
            {
                return normal / len;
            }
        }
        return normal;
    }

    // ============================================================================
    // Полезные константы
    // ============================================================================
    static const half3 half3_zero(half_zero);
    static const half3 half3_one(half_one);
    static const half3 half3_unit_x(half_one, half_zero, half_zero);
    static const half3 half3_unit_y(half_zero, half_one, half_zero);
    static const half3 half3_unit_z(half_zero, half_zero, half_one);

    // Цветовые константы
    static const half3 half3_red(half_one, half_zero, half_zero);
    static const half3 half3_green(half_zero, half_one, half_zero);
    static const half3 half3_blue(half_zero, half_zero, half_one);
    static const half3 half3_white(half_one);
    static const half3 half3_black(half_zero);
} // namespace Core::Math
///////////////////////////////////////////////////////////////
