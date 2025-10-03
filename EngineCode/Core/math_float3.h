///////////////////////////////////////////////////////////////
// Created: 26.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "math_constants.h"
#include "math_functions.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
class float3;
///////////////////////////////////////////////////////////////
inline float3 operator+(float3 lhs, const float3& rhs);
inline float3 operator-(float3 lhs, const float3& rhs);
inline float3 operator*(float3 lhs, const float3& rhs);
inline float3 operator/(float3 lhs, const float3& rhs);
inline float3 operator*(float3 vec, float scalar);
inline float3 operator*(float scalar, float3 vec);
inline float3 operator/(float3 vec, float scalar);
///////////////////////////////////////////////////////////////
    class float3
    {
    public:
        float x, y, z;

        // ============================================================================
        // Конструкторы
        // ============================================================================
        float3() : x(0.0f), y(0.0f), z(0.0f) {}
        float3(float x, float y, float z) : x(x), y(y), z(z) {}
        float3(float scalar) : x(scalar), y(scalar), z(scalar) {}
        float3(const float2& vec, float z) : x(vec.x), y(vec.y), z(z) {}
        float3(const float3&) = default;

        // Конструкторы из D3D типов
        float3(const D3DXVECTOR3& vec) : x(vec.x), y(vec.y), z(vec.z) {}
        float3(const D3DXVECTOR4& vec) : x(vec.x), y(vec.y), z(vec.z) {}
        float3(const D3DXVECTOR2& vec, float z) : x(vec.x), y(vec.y), z(z) {}
        explicit float3(D3DCOLOR color)
        {
            x = ((color >> 16) & 0xFF) / 255.0f; // R
            y = ((color >> 8) & 0xFF) / 255.0f;  // G
            z = (color & 0xFF) / 255.0f;         // B
        }
        explicit float3(const float* data) : x(data[0]), y(data[1]), z(data[2]) {}

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        float3& operator=(const float3&) = default;
        
        float3& operator=(float scalar)
        {
            x = y = z = scalar;
            return *this;
        }
        
        float3& operator=(const D3DXVECTOR3& vec)
        {
            x = vec.x;
            y = vec.y;
            z = vec.z;
            return *this;
        }
        
        float3& operator=(D3DCOLOR color)
        {
            x = ((color >> 16) & 0xFF) / 255.0f;
            y = ((color >> 8) & 0xFF) / 255.0f;
            z = (color & 0xFF) / 255.0f;
            return *this;
        }

        // ============================================================================
        // Арифметические операторы с присваиванием
        // ============================================================================
        float3& operator+=(const float3& rhs) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
        float3& operator-=(const float3& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
        float3& operator*=(const float3& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
        float3& operator/=(const float3& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
        
        float3& operator*=(float scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
        float3& operator/=(float scalar) { float inv = 1.0f / scalar; x *= inv; y *= inv; z *= inv; return *this; }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        float3 operator+() const { return *this; }
        float3 operator-() const { return float3(-x, -y, -z); }

        // ============================================================================
        // Индексация
        // ============================================================================
        float& operator[](int index) { return (&x)[index]; }
        const float& operator[](int index) const { return (&x)[index]; }

        // ============================================================================
        // Конвертация в другие типы
        // ============================================================================
        operator D3DXVECTOR3() const { return D3DXVECTOR3(x, y, z); }
        operator const float*() const { return &x; }
        operator float*() { return &x; }

        // ============================================================================
        // Математические функции (HLSL style)
        // ============================================================================
        float length() const { return sqrtf(x * x + y * y + z * z); }
        float length_sq() const { return x * x + y * y + z * z; }
        
        float3 normalize() const 
        { 
            float len = length();
            return (len > 0.0f) ? (*this / len) : float3(0.0f);
        }

        static float dot(const float3& a, const float3& b)
        {
            return a.x * b.x + a.y * b.y + a.z * b.z;
        }

        static float3 cross(const float3& a, const float3& b)
        {
            return float3(
                a.y * b.z - a.z * b.y,
                a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x
            );
        }

        static float3 lerp(const float3& a, const float3& b, float t)
        {
            return float3(
                a.x + (b.x - a.x) * t,
                a.y + (b.y - a.y) * t,
                a.z + (b.z - a.z) * t
            );
        }

        static float3 saturate(const float3& vec)
        {
            return float3(
                std::clamp(vec.x, 0.0f, 1.0f),
                std::clamp(vec.y, 0.0f, 1.0f),
                std::clamp(vec.z, 0.0f, 1.0f)
            );
        }

        static float3 reflect(const float3& incident, const float3& normal)
        {
            float dot_val = dot(incident, normal);
            return incident - 2.0f * dot_val * normal;
        }

        static float3 refract(const float3& incident, const float3& normal, float eta)
        {
            float dot_ni = dot(normal, incident);
            float k = 1.0f - eta * eta * (1.0f - dot_ni * dot_ni);

            if (k < 0.0f)
                return float3(0.0f); // полное внутреннее отражение

            return eta * incident - (eta * dot_ni + sqrtf(k)) * normal;
        }

        // ============================================================================
        // Swizzle операции (HLSL style)
        // ============================================================================
        float2 xy() const { return float2(x, y); }
        float2 xz() const { return float2(x, z); }
        float2 yz() const { return float2(y, z); }

        float3 yxz() const { return float3(y, x, z); }
        float3 zxy() const { return float3(z, x, y); }
        float3 zyx() const { return float3(z, y, x); }
        float3 xzy() const { return float3(x, z, y); }

        // Цветовые swizzles
        float r() const { return x; }
        float g() const { return y; }
        float b() const { return z; }
        float2 rg() const { return float2(x, y); }
        float2 rb() const { return float2(x, z); }
        float2 gb() const { return float2(y, z); }
        float3 rgb() const { return *this; }
        float3 bgr() const { return float3(z, y, x); }
        float3 gbr() const { return float3(y, z, x); }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool isValid() const 
        { 
            return std::isfinite(x) && std::isfinite(y) && std::isfinite(z); 
        }
        
        bool approximately(const float3& other, float epsilon = EPSILON) const
        {
            return Core::Math::approximately(x, other.x, epsilon) &&
                   Core::Math::approximately(y, other.y, epsilon) &&
                   Core::Math::approximately(z, other.z, epsilon);
        }

        std::string to_string() const
        {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f)", x, y, z);
            return std::string(buffer);
        }

        // Установка компонентов
        void set_xy(const float2& xy)
        {
            x = xy.x;
            y = xy.y;
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(const float3& rhs) const { return approximately(rhs); }
        bool operator!=(const float3& rhs) const { return !(*this == rhs); }

        // ============================================================================
        // Статические методы конвертации
        // ============================================================================
		static float3 fromD3DXVECTOR3(const D3DXVECTOR3& vec)
        {
            return float3(vec.x, vec.y, vec.z);
        }

        static float3 fromD3DXVECTOR2(const D3DXVECTOR2& vec, float z = 0.0f)
        {
            return float3(vec.x, vec.y, z);
        }

        static float3 fromD3DCOLOR(D3DCOLOR color)
        {
            return float3(color);
        }

        static float3 fromRGB(uint8_t r, uint8_t g, uint8_t b)
        {
            return float3(r / 255.0f, g / 255.0f, b / 255.0f);
        }
    };

    // ============================================================================
    // Бинарные операторы
    // ============================================================================
    inline float3 operator+(float3 lhs, const float3& rhs) { return lhs += rhs; }
    inline float3 operator-(float3 lhs, const float3& rhs) { return lhs -= rhs; }
    inline float3 operator*(float3 lhs, const float3& rhs) { return lhs *= rhs; }
    inline float3 operator/(float3 lhs, const float3& rhs) { return lhs /= rhs; }

    inline float3 operator*(float3 vec, float scalar) { return vec *= scalar; }
    inline float3 operator*(float scalar, float3 vec) { return vec *= scalar; }
    inline float3 operator/(float3 vec, float scalar) { return vec /= scalar; }

    // ============================================================================
    // Глобальные функции (HLSL style)
    // ============================================================================
    inline float distance(const float3& a, const float3& b)
    {
        return (b - a).length();
    }

    inline float dot(const float3& a, const float3& b)
    {
        return float3::dot(a, b);
    }

    inline float3 cross(const float3& a, const float3& b)
    {
        return float3::cross(a, b);
    }

    inline float3 normalize(const float3& vec)
    {
        return vec.normalize();
    }

    inline float3 lerp(const float3& a, const float3& b, float t)
    {
        return float3::lerp(a, b, t);
    }

    inline float3 saturate(const float3& vec)
    {
        return float3::saturate(vec);
    }

    inline float3 reflect(const float3& incident, const float3& normal)
    {
        return float3::reflect(incident, normal);
    }

    inline float3 refract(const float3& incident, const float3& normal, float eta)
    {
        return float3::refract(incident, normal, eta);
    }

    inline bool approximately(const float3& a, const float3& b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }

    inline bool isNormalized(const float3& vec, float epsilon = EPSILON)
    {
        return approximately(vec.length_sq(), 1.0f, epsilon);
    }

    inline bool areOrthogonal(const float3& a, const float3& b, float epsilon = EPSILON)
    {
        return approximately(dot(a, b), 0.0f, epsilon);
    }

    inline bool isOrthonormalBasis(const float3& x, const float3& y, const float3& z, float epsilon = EPSILON)
    {
        return isNormalized(x, epsilon) &&
               isNormalized(y, epsilon) &&
               isNormalized(z, epsilon) &&
               areOrthogonal(x, y, epsilon) &&
               areOrthogonal(x, z, epsilon) &&
               areOrthogonal(y, z, epsilon);
    }

    inline bool isValid(const float3& vec)
    {
        return vec.isValid();
    }

    // ============================================================================
    // D3D совместимость
    // ============================================================================
	inline D3DXVECTOR3 ToD3DXVECTOR3(const float3& vec)
    {
        return D3DXVECTOR3(vec.x, vec.y, vec.z);
    }

    inline float3 FromD3DXVECTOR3(const D3DXVECTOR3& vec)
    {
        return float3(vec.x, vec.y, vec.z);
    }

    inline D3DCOLOR ToD3DCOLOR(const float3& color)
    {
        return D3DCOLOR_COLORVALUE(color.x, color.y, color.z, 1.0f);
    }

    inline void float3ArrayToD3D(const float3* source, D3DXVECTOR3* destination, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
			destination[i] = ToD3DXVECTOR3(source[i]);
    }

    inline void D3DArrayTofloat3(const D3DXVECTOR3* source, float3* destination, size_t count)
    {
        for (size_t i = 0; i < count; ++i)
			destination[i] = FromD3DXVECTOR3(source[i]);
    }

    // ============================================================================
    // Полезные константы
    // ============================================================================
    static const float3 float3_zero(0.0f, 0.0f, 0.0f);
    static const float3 float3_one(1.0f, 1.0f, 1.0f);
    static const float3 float3_unit_x(1.0f, 0.0f, 0.0f);
    static const float3 float3_unit_y(0.0f, 1.0f, 0.0f);
    static const float3 float3_unit_z(0.0f, 0.0f, 1.0f);
    static const float3 float3_forward(0.0f, 0.0f, 1.0f);
    static const float3 float3_up(0.0f, 1.0f, 0.0f);
    static const float3 float3_right(1.0f, 0.0f, 0.0f);
} // namespace Core::Math
///////////////////////////////////////////////////////////////
