///////////////////////////////////////////////////////////////
// Created: 26.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <ostream>
///////////////////////////////////////////////////////////////
#include "math_half.h"
#include "math_half2.h"
#include "math_half3.h"
#include "math_float4.h"
///////////////////////////////////////////////////////////////
#include "math_constants.h"
#include "math_functions.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
	class half;
	class half2;
	class half3;
	class half4;
	class float2;
	class float3;
	class float4;
	///////////////////////////////////////////////////////////////
	inline half4 operator+(half4 lhs, const half4& rhs);
	inline half4 operator-(half4 lhs, const half4& rhs);
	inline half4 operator*(half4 lhs, const half4& rhs);
	inline half4 operator/(half4 lhs, const half4& rhs);
	inline half4 operator*(half4 vec, half scalar);
	inline half4 operator*(half scalar, half4 vec);
	inline half4 operator/(half4 vec, half scalar);
	inline half4 operator*(half4 vec, float scalar);
	inline half4 operator*(float scalar, half4 vec);
	inline half4 operator/(half4 vec, float scalar);
	inline half4 operator+(const half4& lhs, const float4& rhs);
	inline half4 operator-(const half4& lhs, const float4& rhs);
	inline half4 operator*(const half4& lhs, const float4& rhs);
	inline half4 operator/(const half4& lhs, const float4& rhs);
	inline half4 operator+(const float4& lhs, const half4& rhs);
	inline half4 operator-(const float4& lhs, const half4& rhs);
	inline half4 operator*(const float4& lhs, const half4& rhs);
	inline half4 operator/(const float4& lhs, const half4& rhs);
	///////////////////////////////////////////////////////////////
    class half4
    {
    public:
        half x, y, z, w;

        // ============================================================================
        // Конструкторы
        // ============================================================================
        half4() : x(half_zero), y(half_zero), z(half_zero), w(half_zero) {}
        half4(half x, half y, half z, half w) : x(x), y(y), z(z), w(w) {}
        half4(half scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
        half4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        half4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
        half4(const half4&) = default;

        // Конструкторы из других типов
        half4(const half2& vec, half z, half w) : x(vec.x), y(vec.y), z(z), w(w) {}
        half4(const half3& vec, half w) : x(vec.x), y(vec.y), z(vec.z), w(w) {}
        half4(const float4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
        half4(const float3& vec, float w) : x(vec.x), y(vec.y), z(vec.z), w(w) {}
        half4(const float2& vec, float z, float w) : x(vec.x), y(vec.y), z(z), w(w) {}

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        half4& operator=(const half4&) = default;
        
        half4& operator=(const float4& vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            w = vec.w;
            return *this;
        }

        // ============================================================================
        // Арифметические операторы с присваиванием
        // ============================================================================
        half4& operator+=(const half4& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
        half4& operator-=(const half4& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
        half4& operator*=(const half4& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
        half4& operator/=(const half4& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
        
        half4& operator*=(half scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
        half4& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
        half4& operator/=(half scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }
        half4& operator/=(float scalar) { x /= scalar; y /= scalar; z /= scalar; w /= scalar; return *this; }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        half4 operator+() const { return *this; }
        half4 operator-() const { return half4(-x, -y, -z, -w); }

        // ============================================================================
        // Индексация
        // ============================================================================
        half& operator[](int index) { return (&x)[index]; }
        const half& operator[](int index) const { return (&x)[index]; }

        // ============================================================================
        // Конвертация в другие типы
        // ============================================================================
        explicit operator float4() const { return float4(float(x), float(y), float(z), float(w)); }

        // ============================================================================
        // Математические функции
        // ============================================================================
        half length() const { return sqrt(length_sq()); }
        half length_sq() const { return x * x + y * y + z * z + w * w; }
        
        half4 normalize() const 
        { 
            half len = length();
            return (len > half_zero) ? (*this / len) : half4(half_zero);
        }

        static half dot(const half4& a, const half4& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        static half dot3(const half4& a, const half4& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        static half4 cross(const half4& a, const half4& b)
        {
            return half4(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x,
                half_zero  // w = 0 для векторов
            );
        }

        static half4 lerp(const half4& a, const half4& b, half t)
        {
            return half4(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t,
                a.w + (b.w - a.w) * t
            );
        }

        static half4 lerp(const half4& a, const half4& b, float t)
        {
            return half4(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t,
                a.w + (b.w - a.w) * t
            );
        }

        static half4 saturate(const half4& vec)
        {
            return half4(
                std::clamp(float(vec.x), 0.0f, 1.0f),
                std::clamp(float(vec.y), 0.0f, 1.0f),
                std::clamp(float(vec.z), 0.0f, 1.0f),
                std::clamp(float(vec.w), 0.0f, 1.0f)
            );
        }

        static half4 floor(const half4& vec)
        {
            return half4(
                ::floor(vec.x),
                ::floor(vec.y),
                ::floor(vec.z),
                ::floor(vec.w)
            );
        }

        static half4 ceil(const half4& vec)
        {
            return half4(
                ::ceil(vec.x),
                ::ceil(vec.y),
                ::ceil(vec.z),
                ::ceil(vec.w)
            );
        }

        static half4 round(const half4& vec)
        {
            return half4(
                ::round(vec.x),
                ::round(vec.y),
                ::round(vec.z),
                ::round(vec.w)
            );
        }

        // ============================================================================
        // Swizzle операции
        // ============================================================================
        half2 xy() const { return half2(x, y); }
        half2 xz() const { return half2(x, z); }
        half2 xw() const { return half2(x, w); }
        half2 yz() const { return half2(y, z); }
        half2 yw() const { return half2(y, w); }
        half2 zw() const { return half2(z, w); }

        half3 xyz() const { return half3(x, y, z); }
        half3 xyw() const { return half3(x, y, w); }
        half3 xzw() const { return half3(x, z, w); }
        half3 yzw() const { return half3(y, z, w); }

        half4 yxzw() const { return half4(y, x, z, w); }
        half4 zxyw() const { return half4(z, x, y, w); }
        half4 zyxw() const { return half4(z, y, x, w); }
        half4 wzyx() const { return half4(w, z, y, x); }

        // ============================================================================
        // Цветовые операции
        // ============================================================================
        static half4 from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            return half4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }

        half luminance() const
        {
            return 0.299f * x + 0.587f * y + 0.114f * z;  // игнорируем alpha
        }

        half4 rgb_to_grayscale() const
        {
            half luma = luminance();
            return half4(luma, luma, luma, w);  // сохраняем alpha
        }

        half4 premultiply_alpha() const
        {
            return half4(x * w, y * w, z * w, w);
        }

        half4 unpremultiply_alpha() const
        {
            if (w > half_zero)
                return half4(x / w, y / w, z / w, w);
            return *this;
        }

        half4 gamma_correct(half gamma) const
        {
            return half4(
                pow(x, gamma),
                pow(y, gamma),
                pow(z, gamma),
                w  // alpha не гамма-корректируется
            );
        }

        half4 alpha_blend(const half4& background) const
        {
            half alpha = w;
            half inv_alpha = half_one - alpha;
            return half4(
                x * alpha + background.x * inv_alpha,
                y * alpha + background.y * inv_alpha,
                z * alpha + background.z * inv_alpha,
                half_one  // результирующий alpha
            );
        }

        // ============================================================================
        // Геометрические операции
        // ============================================================================
        half3 project() const
        {
            if (w != half_zero)
                return half3(x / w, y / w, z / w);
            return half3(x, y, z);
        }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool isValid() const 
        { 
            return x.isValid() && y.isValid() && z.isValid() && w.isValid(); 
        }
        
        bool approximately(const half4& other, float epsilon = EPSILON) const
        {
            return x.approximately(other.x, epsilon) && 
                   y.approximately(other.y, epsilon) && 
                   z.approximately(other.z, epsilon) && 
                   w.approximately(other.w, epsilon);
        }

        std::string to_string() const
        {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f, %.3f)", float(x), float(y), float(z), float(w));
            return std::string(buffer);
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(const half4& rhs) const { return approximately(rhs); }
        bool operator!=(const half4& rhs) const { return !(*this == rhs); }
    };

    // ============================================================================
    // Бинарные операторы
    // ============================================================================
    inline half4 operator+(half4 lhs, const half4& rhs) { return lhs += rhs; }
    inline half4 operator-(half4 lhs, const half4& rhs) { return lhs -= rhs; }
    inline half4 operator*(half4 lhs, const half4& rhs) { return lhs *= rhs; }
    inline half4 operator/(half4 lhs, const half4& rhs) { return lhs /= rhs; }

    inline half4 operator*(half4 vec, half scalar) { return vec *= scalar; }
    inline half4 operator*(half scalar, half4 vec) { return vec *= scalar; }
    inline half4 operator/(half4 vec, half scalar) { return vec /= scalar; }

    inline half4 operator*(half4 vec, float scalar) { return vec *= scalar; }
    inline half4 operator*(float scalar, half4 vec) { return vec *= scalar; }
    inline half4 operator/(half4 vec, float scalar) { return vec /= scalar; }

    // ============================================================================
    // Операторы с float4
    // ============================================================================
    inline half4 operator+(const half4& lhs, const float4& rhs) 
    { 
        return half4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); 
    }
    
    inline half4 operator-(const half4& lhs, const float4& rhs) 
    { 
        return half4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); 
    }
    
    inline half4 operator*(const half4& lhs, const float4& rhs) 
    { 
        return half4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); 
    }
    
    inline half4 operator/(const half4& lhs, const float4& rhs) 
    { 
        return half4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); 
    }

    inline half4 operator+(const float4& lhs, const half4& rhs) 
    { 
        return half4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w); 
    }
    
    inline half4 operator-(const float4& lhs, const half4& rhs) 
    { 
        return half4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w); 
    }
    
    inline half4 operator*(const float4& lhs, const half4& rhs) 
    { 
        return half4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w); 
    }
    
    inline half4 operator/(const float4& lhs, const half4& rhs) 
    { 
        return half4(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z, lhs.w / rhs.w); 
    }

    // ============================================================================
    // Глобальные функции
    // ============================================================================
    inline half distance(const half4& a, const half4& b)
    {
        return (b - a).length();
    }

    inline half dot(const half4& a, const half4& b)
    {
        return half4::dot(a, b);
    }

    inline half dot3(const half4& a, const half4& b)
    {
        return half4::dot3(a, b);
    }

    inline half4 cross(const half4& a, const half4& b)
    {
        return half4::cross(a, b);
    }

    inline half4 normalize(const half4& vec)
    {
        return vec.normalize();
    }

    inline half4 lerp(const half4& a, const half4& b, half t)
    {
        return half4::lerp(a, b, t);
    }

    inline half4 lerp(const half4& a, const half4& b, float t)
    {
        return half4::lerp(a, b, t);
    }

    inline half4 saturate(const half4& vec)
    {
        return half4::saturate(vec);
    }

    inline half4 floor(const half4& vec)
    {
        return half4::floor(vec);
    }

    inline half4 ceil(const half4& vec)
    {
        return half4::ceil(vec);
    }

    inline half4 round(const half4& vec)
    {
        return half4::round(vec);
    }

    inline bool approximately(const half4& a, const half4& b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }

    inline bool isValid(const half4& vec)
    {
        return vec.isValid();
    }

    inline bool is_normalized(const half4& vec, float epsilon = EPSILON)
    {
        return approximately(vec.length_sq(), half_one, epsilon);
    }

    // ============================================================================
    // Конвертация между типами
    // ============================================================================
    inline float4 to_float4(const half4& vec)
    {
        return float4(float(vec.x), float(vec.y), float(vec.z), float(vec.w));
    }

    inline half4 to_half4(const float4& vec)
    {
        return half4(vec.x, vec.y, vec.z, vec.w);
    }

    // ============================================================================
    // Цветовые функции
    // ============================================================================
    inline half4 rgb_to_grayscale(const half4& rgba)
    {
        return rgba.rgb_to_grayscale();
    }

    inline half luminance(const half4& rgba)
    {
        return rgba.luminance();
    }

    inline half4 gamma_correct(const half4& color, half gamma)
    {
        return color.gamma_correct(gamma);
    }

    inline half4 alpha_blend(const half4& foreground, const half4& background)
    {
        return foreground.alpha_blend(background);
    }

    inline half4 premultiply_alpha(const half4& color)
    {
        return color.premultiply_alpha();
    }

    inline half4 unpremultiply_alpha(const half4& color)
    {
        return color.unpremultiply_alpha();
    }

    // ============================================================================
    // Геометрические функции
    // ============================================================================
    inline half4 clamp(const half4& vec, const half4& min_val, const half4& max_val)
    {
        return half4(
            std::clamp(float(vec.x), float(min_val.x), float(max_val.x)),
            std::clamp(float(vec.y), float(min_val.y), float(max_val.y)),
            std::clamp(float(vec.z), float(min_val.z), float(max_val.z)),
            std::clamp(float(vec.w), float(min_val.w), float(max_val.w))
        );
    }

    inline half4 clamp(const half4& vec, float min_val, float max_val)
    {
        return half4(
            std::clamp(float(vec.x), min_val, max_val),
            std::clamp(float(vec.y), min_val, max_val),
            std::clamp(float(vec.z), min_val, max_val),
            std::clamp(float(vec.w), min_val, max_val)
        );
    }

    inline half4 frac(const half4& vec)
    {
        return half4(
            vec.x - floor(vec.x),
            vec.y - floor(vec.y),
            vec.z - floor(vec.z),
            vec.w - floor(vec.w)
        );
    }

    // ============================================================================
    // Утилиты для вершинных данных
    // ============================================================================
    inline half4 make_vertex_position(const half3& position, half w = half_one)
    {
        return half4(position, w);
    }

    inline half4 make_color_rgba(const half3& rgb, half a = half_one)
    {
        return half4(rgb, a);
    }

    // ============================================================================
    // Полезные константы
    // ============================================================================
    static const half4 half4_zero(half_zero);
    static const half4 half4_one(half_one);
    static const half4 half4_unit_x(half_one, half_zero, half_zero, half_zero);
    static const half4 half4_unit_y(half_zero, half_one, half_zero, half_zero);
    static const half4 half4_unit_z(half_zero, half_zero, half_one, half_zero);
    static const half4 half4_unit_w(half_zero, half_zero, half_zero, half_one);

    // Цветовые константы
    static const half4 half4_red(half_one, half_zero, half_zero, half_one);
    static const half4 half4_green(half_zero, half_one, half_zero, half_one);
    static const half4 half4_blue(half_zero, half_zero, half_one, half_one);
    static const half4 half4_white(half_one);
    static const half4 half4_black(half_zero, half_zero, half_zero, half_one);
    static const half4 half4_transparent(half_zero, half_zero, half_zero, half_zero);
} // namespace Core::Math
///////////////////////////////////////////////////////////////
