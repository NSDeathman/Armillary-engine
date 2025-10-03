///////////////////////////////////////////////////////////////
// Created: 26.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "math_half.h"
#include "math_half4.h"
#include "math_constants.h"
#include "math_functions.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
    class float2;
    class float3;
    class float4;
    ///////////////////////////////////////////////////////////////
    // Глобальные функции для float4
    inline float4 operator+(float4 lhs, const float4& rhs);
    inline float4 operator-(float4 lhs, const float4& rhs);
    inline float4 operator*(float4 lhs, const float4& rhs);
    inline float4 operator/(float4 lhs, const float4& rhs);
    inline float4 operator*(float4 vec, float scalar);
    inline float4 operator*(float scalar, float4 vec);
    inline float4 operator/(float4 vec, float scalar);
    ///////////////////////////////////////////////////////////////
    class float4
    {
    public:
        float x, y, z, w;

        // ============================================================================
        // Конструкторы
        // ============================================================================
        float4() : x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}
        float4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        float4(float scalar) : x(scalar), y(scalar), z(scalar), w(scalar) {}
        float4(const float2& vec, float z, float w) : x(vec.x), y(vec.y), z(z), w(w) {}
        float4(const float3& vec, float w) : x(vec.x), y(vec.y), z(vec.z), w(w) {}
        float4(const float4&) = default;

        // Конструкторы из D3D типов
        explicit float4(const D3DXVECTOR4& vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}
        float4(const D3DXVECTOR3& vec, float w) : x(vec.x), y(vec.y), z(vec.z), w(w) {}
        float4(const D3DXVECTOR2& vec, float z, float w) : x(vec.x), y(vec.y), z(z), w(w) {}
        explicit float4(D3DCOLOR color)
        {
            x = ((color >> 16) & 0xFF) / 255.0f; // R
            y = ((color >> 8) & 0xFF) / 255.0f;  // G
            z = (color & 0xFF) / 255.0f;         // B
            w = ((color >> 24) & 0xFF) / 255.0f; // A
        }
        explicit float4(const float* data) : x(data[0]), y(data[1]), z(data[2]), w(data[3]) {}

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        float4& operator=(const float4&) = default;
        
        float4& operator=(float scalar)
        {
            x = y = z = w = scalar;
            return *this;
        }
        
        float4& operator=(const float3& xyz)
        {
            x = xyz.x;
            y = xyz.y;
            z = xyz.z;
            // w не меняется!
            return *this;
        }
        
        float4& operator=(const D3DXVECTOR4& vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            w = vec.w;
            return *this;
        }
        
        float4& operator=(D3DCOLOR color)
        {
            x = ((color >> 16) & 0xFF) / 255.0f;
            y = ((color >> 8) & 0xFF) / 255.0f;
            z = (color & 0xFF) / 255.0f;
            w = ((color >> 24) & 0xFF) / 255.0f;
            return *this;
        }

        // ============================================================================
        // Арифметические операторы с присваиванием
        // ============================================================================
        float4& operator+=(const float4& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
        float4& operator-=(const float4& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; w -= rhs.w; return *this; }
        float4& operator*=(const float4& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; w *= rhs.w; return *this; }
        float4& operator/=(const float4& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; w /= rhs.w; return *this; }
        
        float4& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; w *= scalar; return *this; }
        float4& operator/=(float scalar) { float inv = 1.0f / scalar; x *= inv; y *= inv; z *= inv; w *= inv; return *this; }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        float4 operator+() const { return *this; }
        float4 operator-() const { return float4(-x, -y, -z, -w); }

        // ============================================================================
        // Индексация
        // ============================================================================
        float& operator[](int index) { return (&x)[index]; }
        const float& operator[](int index) const { return (&x)[index]; }

        // ============================================================================
        // Конвертация в другие типы
        // ============================================================================
        operator D3DXVECTOR4() const { return D3DXVECTOR4(x, y, z, w); }
        operator const float*() const { return &x; }
        operator float*() { return &x; }

        // ============================================================================
        // Математические функции
        // ============================================================================
        float length() const { return sqrtf(x * x + y * y + z * z + w * w); }
        float length_sq() const { return x * x + y * y + z * z + w * w; }
        
        float4 normalize() const 
        { 
            float len = length();
            return (len > 0.0f) ? (*this / len) : float4(0.0f);
        }

        static float dot(const float4& a, const float4& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
        }

        static float dot3(const float4& a, const float4& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        static float4 cross(const float4& a, const float4& b)
        {
            return float4(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x,
                0.0f  // w = 0 для векторов
            );
        }

        static float4 lerp(const float4& a, const float4& b, float t)
        {
            return float4(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t,
                a.w + (b.w - a.w) * t
            );
        }

        static float4 saturate(const float4& vec)
        {
            return float4(
                std::clamp(vec.x, 0.0f, 1.0f),
                std::clamp(vec.y, 0.0f, 1.0f),
                std::clamp(vec.z, 0.0f, 1.0f),
                std::clamp(vec.w, 0.0f, 1.0f)
            );
        }

        static float4 floor(const float4& vec)
        {
            return float4(
                std::floor(vec.x),
                std::floor(vec.y),
                std::floor(vec.z),
                std::floor(vec.w)
            );
        }

        static float4 ceil(const float4& vec)
        {
            return float4(
                std::ceil(vec.x),
                std::ceil(vec.y),
                std::ceil(vec.z),
                std::ceil(vec.w)
            );
        }

        static float4 round(const float4& vec)
        {
            return float4(
                std::round(vec.x),
                std::round(vec.y),
                std::round(vec.z),
                std::round(vec.w)
            );
        }

        // ============================================================================
        // Swizzle операции
        // ============================================================================
        float2 xy() const { return float2(x, y); }
        float2 xz() const { return float2(x, z); }
        float2 xw() const { return float2(x, w); }
        float2 yz() const { return float2(y, z); }
        float2 yw() const { return float2(y, w); }
        float2 zw() const { return float2(z, w); }

        float3 xyz() const { return float3(x, y, z); }
        float3 xyw() const { return float3(x, y, w); }
        float3 xzw() const { return float3(x, z, w); }
        float3 yzw() const { return float3(y, z, w); }

        float4 yxzw() const { return float4(y, x, z, w); }
        float4 zxyw() const { return float4(z, x, y, w); }
        float4 zyxw() const { return float4(z, y, x, w); }
        float4 wzyx() const { return float4(w, z, y, x); }

        // Цветовые swizzles
        float r() const { return x; }
        float g() const { return y; }
        float b() const { return z; }
        float a() const { return w; }
        float2 rg() const { return float2(x, y); }
        float2 rb() const { return float2(x, z); }
        float2 ra() const { return float2(x, w); }
        float2 gb() const { return float2(y, z); }
        float2 ga() const { return float2(y, w); }
        float2 ba() const { return float2(z, w); }

        float3 rgb() const { return float3(x, y, z); }
        float3 rga() const { return float3(x, y, w); }
        float3 rba() const { return float3(x, z, w); }
        float3 gba() const { return float3(y, z, w); }

        float4 grba() const { return float4(y, x, z, w); }
        float4 brga() const { return float4(z, x, y, w); }
        float4 bgra() const { return float4(z, y, x, w); }
        float4 abgr() const { return float4(w, z, y, x); }

        // ============================================================================
        // Цветовые операции
        // ============================================================================
        static float4 fromRGBA(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            return float4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }

        float luminance() const
        {
            return 0.299f * x + 0.587f * y + 0.114f * z;  // игнорируем alpha
        }

        float brightness() const
        {
            return (x + y + z) / 3.0f;
        }

        float4 premultiply_alpha() const
        {
            return float4(x * w, y * w, z * w, w);
        }

        float4 unpremultiply_alpha() const
        {
            if (w > 0.0f)
                return float4(x / w, y / w, z / w, w);
            return *this;
        }

        // ============================================================================
        // Геометрические операции
        // ============================================================================
        float3 project() const
        {
            if (w != 0.0f)
                return float3(x / w, y / w, z / w);
            return float3(x, y, z);
        }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool isValid() const 
        { 
            return std::isfinite(x) && std::isfinite(y) && 
                   std::isfinite(z) && std::isfinite(w); 
        }
        
        bool approximately(const float4& other, float epsilon = EPSILON) const
        {
            return Core::Math::approximately(x, other.x, epsilon) &&
                   Core::Math::approximately(y, other.y, epsilon) &&
                   Core::Math::approximately(z, other.z, epsilon) &&
                   Core::Math::approximately(w, other.w, epsilon);
        }

        std::string to_string() const
        {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f, %.3f)", x, y, z, w);
            return std::string(buffer);
        }

        // Установка компонентов
        void set_xyz(const float3& xyz)
        {
            x = xyz.x;
            y = xyz.y;
            z = xyz.z;
        }

        void set_xy(const float2& xy)
        {
            x = xy.x;
            y = xy.y;
        }

        void set_zw(const float2& zw)
        {
            z = zw.x;
            w = zw.y;
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(const float4& rhs) const { return approximately(rhs); }
        bool operator!=(const float4& rhs) const { return !(*this == rhs); }

        // ============================================================================
        // Статические методы конвертации
        // ============================================================================
        static float4 FromD3DXVECTOR4(const D3DXVECTOR4& vec)
        {
            return float4(vec.x, vec.y, vec.z, vec.w);
        }

        static float4 FromD3DXVECTOR3(const D3DXVECTOR3& vec, float w = 1.0f)
        {
            return float4(vec.x, vec.y, vec.z, w);
        }

        static float4 FromD3DXVECTOR2(const D3DXVECTOR2& vec, float z = 0.0f, float w = 1.0f)
        {
            return float4(vec.x, vec.y, z, w);
        }

        static float4 FromD3DCOLOR(D3DCOLOR color)
        {
            return float4(color);
        }

        static float4 FromRGB(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255)
        {
            return float4(r / 255.0f, g / 255.0f, b / 255.0f, a / 255.0f);
        }
    };

    // ============================================================================
    // Бинарные операторы
    // ============================================================================
    inline float4 operator+(float4 lhs, const float4& rhs) { return lhs += rhs; }
    inline float4 operator-(float4 lhs, const float4& rhs) { return lhs -= rhs; }
    inline float4 operator*(float4 lhs, const float4& rhs) { return lhs *= rhs; }
    inline float4 operator/(float4 lhs, const float4& rhs) { return lhs /= rhs; }

    inline float4 operator*(float4 vec, float scalar) { return vec *= scalar; }
    inline float4 operator*(float scalar, float4 vec) { return vec *= scalar; }
    inline float4 operator/(float4 vec, float scalar) { return vec /= scalar; }

    // ============================================================================
    // Глобальные функции
    // ============================================================================
    inline float distance(const float4& a, const float4& b)
    {
		float4 diff = b - a;
		return diff.length();
    }

    inline float dot(const float4& a, const float4& b)
    {
        return float4::dot(a, b);
    }

    inline float dot3(const float4& a, const float4& b)
    {
        return float4::dot3(a, b);
    }

    inline float4 cross(const float4& a, const float4& b)
    {
        return float4::cross(a, b);
    }

    inline float4 normalize(const float4& vec)
    {
        return vec.normalize();
    }

    inline float4 lerp(const float4& a, const float4& b, float t)
    {
        return float4::lerp(a, b, t);
    }

    inline float4 saturate(const float4& vec)
    {
        return float4::saturate(vec);
    }

    inline float4 floor(const float4& vec)
    {
        return float4::floor(vec);
    }

    inline float4 ceil(const float4& vec)
    {
        return float4::ceil(vec);
    }

    inline float4 round(const float4& vec)
    {
        return float4::round(vec);
    }

    inline bool approximately(const float4& a, const float4& b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }

    inline bool isNormalized(const float4& vec, float epsilon = EPSILON)
    {
        return approximately(vec.length_sq(), 1.0f, epsilon);
    }

    inline bool isValid(const float4& vec)
    {
        return vec.isValid();
    }

    // ============================================================================
    // D3D совместимость
    // ============================================================================
    inline D3DXVECTOR4 ToD3DXVECTOR4(const float4& vec)
    {
        return D3DXVECTOR4(vec.x, vec.y, vec.z, vec.w);
    }

    inline float4 FromD3DXVECTOR4(const D3DXVECTOR4& vec)
    {
        return float4(vec.x, vec.y, vec.z, vec.w);
    }

    inline D3DCOLOR ToD3DCOLOR(const float4& color)
    {
        return D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
    }

    inline void float4ArrayToD3D(const float4* source, D3DXVECTOR4* destination, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            destination[i] = ToD3DXVECTOR4(source[i]);
    }

    inline void D3DArrayToFloat4(const D3DXVECTOR4* source, float4* destination, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
            destination[i] = FromD3DXVECTOR4(source[i]);
    }

    // ============================================================================
    // Полезные константы
    // ============================================================================
    static const float4 float4_zero(0.0f, 0.0f, 0.0f, 0.0f);
    static const float4 float4_one(1.0f, 1.0f, 1.0f, 1.0f);
    static const float4 float4_unit_x(1.0f, 0.0f, 0.0f, 0.0f);
    static const float4 float4_unit_y(0.0f, 1.0f, 0.0f, 0.0f);
    static const float4 float4_unit_z(0.0f, 0.0f, 1.0f, 0.0f);
    static const float4 float4_unit_w(0.0f, 0.0f, 0.0f, 1.0f);

    // Цветовые константы
    static const float4 float4_red(1.0f, 0.0f, 0.0f, 1.0f);
    static const float4 float4_green(0.0f, 1.0f, 0.0f, 1.0f);
    static const float4 float4_blue(0.0f, 0.0f, 1.0f, 1.0f);
    static const float4 float4_white(1.0f, 1.0f, 1.0f, 1.0f);
    static const float4 float4_black(0.0f, 0.0f, 0.0f, 1.0f);
    static const float4 float4_transparent(0.0f, 0.0f, 0.0f, 0.0f);
} // namespace Core::Math
///////////////////////////////////////////////////////////////
