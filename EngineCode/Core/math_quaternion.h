///////////////////////////////////////////////////////////////
// Created: 29.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "math_float3.h"
#include "math_float4.h"
#include "math_float3x3.h"
#include "math_float4x4.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
    class quaternion;

	class quaternion
    {
    public:
        float x, y, z, w;

        // ============================================================================
        // Конструкторы
        // ============================================================================
        quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f) {}
        
        quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}
        
        quaternion(float4 vec) : x(vec.x), y(vec.y), z(vec.z), w(vec.w) {}

        // Конструктор из оси и угла
        quaternion(const float3& axis, float angle)
        {
            float half_angle = angle * 0.5f;
            float sin_half = sin(half_angle);
            float3 normalized_axis = axis.normalize();
            
            x = normalized_axis.x * sin_half;
            y = normalized_axis.y * sin_half;
            z = normalized_axis.z * sin_half;
            w = cos(half_angle);
        }
        
        // Конструктор из углов Эйлера (в радианах)
        quaternion(float pitch, float yaw, float roll)
        {
            float half_pitch = pitch * 0.5f;
            float half_yaw = yaw * 0.5f;
            float half_roll = roll * 0.5f;
            
            float sin_pitch = sin(half_pitch);
            float cos_pitch = cos(half_pitch);
            float sin_yaw = sin(half_yaw);
            float cos_yaw = cos(half_yaw);
            float sin_roll = sin(half_roll);
            float cos_roll = cos(half_roll);
            
            x = cos_roll * sin_pitch * cos_yaw + sin_roll * cos_pitch * sin_yaw;
            y = cos_roll * cos_pitch * sin_yaw - sin_roll * sin_pitch * cos_yaw;
            z = sin_roll * cos_pitch * cos_yaw - cos_roll * sin_pitch * sin_yaw;
            w = cos_roll * cos_pitch * cos_yaw + sin_roll * sin_pitch * sin_yaw;
        }
        
        // Конструктор из матрицы 3x3
        explicit quaternion(const float3x3& matrix)
        {
            float trace = matrix.trace();
            
            if (trace > 0.0f)
            {
                float s = sqrt(trace + 1.0f) * 2.0f;
                w = 0.25f * s;
                x = (matrix(2,1) - matrix(1,2)) / s;
                y = (matrix(0,2) - matrix(2,0)) / s;
                z = (matrix(1,0) - matrix(0,1)) / s;
            }
            else if (matrix(0,0) > matrix(1,1) && matrix(0,0) > matrix(2,2))
            {
                float s = sqrt(1.0f + matrix(0,0) - matrix(1,1) - matrix(2,2)) * 2.0f;
                w = (matrix(2,1) - matrix(1,2)) / s;
                x = 0.25f * s;
                y = (matrix(0,1) + matrix(1,0)) / s;
                z = (matrix(0,2) + matrix(2,0)) / s;
            }
            else if (matrix(1,1) > matrix(2,2))
            {
                float s = sqrt(1.0f + matrix(1,1) - matrix(0,0) - matrix(2,2)) * 2.0f;
                w = (matrix(0,2) - matrix(2,0)) / s;
                x = (matrix(0,1) + matrix(1,0)) / s;
                y = 0.25f * s;
                z = (matrix(1,2) + matrix(2,1)) / s;
            }
            else
            {
                float s = sqrt(1.0f + matrix(2,2) - matrix(0,0) - matrix(1,1)) * 2.0f;
                w = (matrix(1,0) - matrix(0,1)) / s;
                x = (matrix(0,2) + matrix(2,0)) / s;
                y = (matrix(1,2) + matrix(2,1)) / s;
                z = 0.25f * s;
            }
            
            normalize();
        }
        
        // Конструктор из матрицы 4x4
        explicit quaternion(const float4x4& matrix)
            : quaternion(float3x3(matrix))
        {
        }
        
        // Конструктор из D3DXQUATERNION
        quaternion(const D3DXQUATERNION& q) : x(q.x), y(q.y), z(q.z), w(q.w) {}
        
        // Конструктор копирования
        quaternion(const quaternion&) = default;

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        quaternion& operator=(const quaternion&) = default;
        
        quaternion& operator=(const D3DXQUATERNION& q)
        {
            x = q.x;
            y = q.y;
            z = q.z;
            w = q.w;
            return *this;
        }

        // ============================================================================
        // Статические конструкторы
        // ============================================================================
        static quaternion identity()
        {
            return quaternion(0.0f, 0.0f, 0.0f, 1.0f);
        }
        
        static quaternion zero()
        {
            return quaternion(0.0f, 0.0f, 0.0f, 0.0f);
        }
        
        static quaternion from_axis_angle(const float3& axis, float angle)
        {
            return quaternion(axis, angle);
        }
        
        static quaternion from_euler(float pitch, float yaw, float roll)
        {
            return quaternion(pitch, yaw, roll);
        }
        
        static quaternion from_euler(const float3& euler_angles)
        {
            return quaternion(euler_angles.x, euler_angles.y, euler_angles.z);
        }
        
        static quaternion from_matrix(const float3x3& matrix)
        {
            return quaternion(matrix);
        }
        
        static quaternion from_matrix(const float4x4& matrix)
        {
            return quaternion(matrix);
        }
        
        static quaternion from_d3dxquaternion(const D3DXQUATERNION& q)
        {
            return quaternion(q);
        }
        
        // Кватернион для вращения от from к to
        static quaternion from_to_rotation(const float3& from, const float3& to)
        {
            float3 v0 = from.normalize();
            float3 v1 = to.normalize();
            
            float cos_angle = Core::Math::dot(v0, v1);
            
            // Если векторы совпадают
            if (cos_angle > 0.9999f)
                return identity();
            
            // Если векторы противоположны
            if (cos_angle < -0.9999f)
            {
                // Находим ортогональную ось
                float3 axis = cross(float3(1, 0, 0), v0);
                if (axis.length_sq() < 0.0001f)
                    axis = cross(float3(0, 1, 0), v0);
                return quaternion(axis.normalize(), PI);
            }
            
            float3 axis = cross(v0, v1);
            float s = sqrt((1.0f + cos_angle) * 2.0f);
            float inv_s = 1.0f / s;
            
            return quaternion(axis.x * inv_s, axis.y * inv_s, axis.z * inv_s, s * 0.5f);
        }
        
        // Сферическая линейная интерполяция
        static quaternion slerp(const quaternion& a, const quaternion& b, float t)
        {
            float cos_half_angle = a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
            
            // Если кватернионы близки, используем линейную интерполяцию
            if (fabs(cos_half_angle) >= 1.0f)
                return a;
            
            float half_angle = acos(cos_half_angle);
            float sin_half_angle = sqrt(1.0f - cos_half_angle * cos_half_angle);
            
            // Учитываем кратчайший путь
            if (fabs(sin_half_angle) < 0.001f)
                return quaternion(
                    a.x * 0.5f + b.x * 0.5f,
                    a.y * 0.5f + b.y * 0.5f,
                    a.z * 0.5f + b.z * 0.5f,
                    a.w * 0.5f + b.w * 0.5f
                ).normalize();
            
            float ratio_a = sin((1 - t) * half_angle) / sin_half_angle;
            float ratio_b = sin(t * half_angle) / sin_half_angle;
            
            return quaternion(
                a.x * ratio_a + b.x * ratio_b,
                a.y * ratio_a + b.y * ratio_b,
                a.z * ratio_a + b.z * ratio_b,
                a.w * ratio_a + b.w * ratio_b
            ).normalize();
        }

        // ============================================================================
        // Арифметические операторы с присваиванием
        // ============================================================================
        quaternion& operator+=(const quaternion& rhs)
        {
            x += rhs.x;
            y += rhs.y;
            z += rhs.z;
            w += rhs.w;
            return *this;
        }
        
        quaternion& operator-=(const quaternion& rhs)
        {
            x -= rhs.x;
            y -= rhs.y;
            z -= rhs.z;
            w -= rhs.w;
            return *this;
        }
        
        quaternion& operator*=(const quaternion& rhs)
        {
            *this = *this * rhs;
            return *this;
        }
        
        quaternion& operator*=(float scalar)
        {
            x *= scalar;
            y *= scalar;
            z *= scalar;
            w *= scalar;
            return *this;
        }
        
        quaternion& operator/=(float scalar)
        {
            float inv_scalar = 1.0f / scalar;
            x *= inv_scalar;
            y *= inv_scalar;
            z *= inv_scalar;
            w *= inv_scalar;
            return *this;
        }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        quaternion operator+() const { return *this; }
        quaternion operator-() const { return quaternion(-x, -y, -z, -w); }

        // ============================================================================
        // Конвертация в другие типы
        // ============================================================================
        operator D3DXQUATERNION() const
        {
            return D3DXQUATERNION(x, y, z, w);
        }
        
        operator float4() const
        {
            return float4(x, y, z, w);
        }

        // ============================================================================
        // Математические операции
        // ============================================================================
        float length() const
        {
            return sqrt(x * x + y * y + z * z + w * w);
        }
        
        float length_sq() const
        {
            return x * x + y * y + z * z + w * w;
        }
        
        quaternion normalize() const
        {
            float len = length();
            if (len > 0.0f)
                return *this / len;
            return identity();
        }
        
        quaternion conjugate() const
        {
            return quaternion(-x, -y, -z, w);
        }
        
        quaternion inverse() const
        {
            float len_sq = length_sq();
            if (len_sq > 0.0f)
				return Core::Math::quaternion::conjugate() / len_sq;
            return identity();
        }
        
        float dot(const quaternion& other) const
        {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }
        
        // Преобразование в матрицу 3x3
        float3x3 to_matrix3x3() const
        {
            float xx = x * x;
            float yy = y * y;
            float zz = z * z;
            float xy = x * y;
            float xz = x * z;
            float xw = x * w;
            float yz = y * z;
            float yw = y * w;
            float zw = z * w;
            
            return float3x3(
                float3(1 - 2 * (yy + zz), 2 * (xy + zw), 2 * (xz - yw)),
                float3(2 * (xy - zw), 1 - 2 * (xx + zz), 2 * (yz + xw)),
                float3(2 * (xz + yw), 2 * (yz - xw), 1 - 2 * (xx + yy))
            );
        }
        
        // Преобразование в матрицу 4x4
        float4x4 to_matrix4x4() const
        {
            float3x3 rot = to_matrix3x3();
            return float4x4(
                float4(rot.col0, 0.0f),
                float4(rot.col1, 0.0f),
                float4(rot.col2, 0.0f),
                float4(0.0f, 0.0f, 0.0f, 1.0f)
            );
        }
        
        // Получение оси и угла вращения
        void to_axis_angle(float3& axis, float& angle) const
        {
            quaternion normalized = normalize();
            angle = 2.0f * acos(normalized.w);
            float sin_half_angle = sqrt(1.0f - normalized.w * normalized.w);
            
            if (sin_half_angle > 0.001f)
            {
                axis.x = normalized.x / sin_half_angle;
                axis.y = normalized.y / sin_half_angle;
                axis.z = normalized.z / sin_half_angle;
            }
            else
            {
                axis = float3(1, 0, 0);
            }
        }
        
        // Получение углов Эйлера (в радианах)
        float3 to_euler() const
        {
            float3 euler;
            
            // pitch (x-axis rotation)
            float sin_pitch = 2.0f * (w * x + y * z);
            float cos_pitch = 1.0f - 2.0f * (x * x + y * y);
            euler.x = atan2(sin_pitch, cos_pitch);
            
            // yaw (y-axis rotation)
            float sin_yaw = 2.0f * (w * y - z * x);
            if (fabs(sin_yaw) >= 1.0f)
                euler.y = copysign(PI / 2.0f, sin_yaw);
            else
                euler.y = asin(sin_yaw);
            
            // roll (z-axis rotation)
            float sin_roll = 2.0f * (w * z + x * y);
            float cos_roll = 1.0f - 2.0f * (y * y + z * z);
            euler.z = atan2(sin_roll, cos_roll);
            
            return euler;
        }

        // ============================================================================
        // Преобразования векторов
        // ============================================================================
        float3 transform_vector(const float3& vec) const
        {
            quaternion vec_quat(vec.x, vec.y, vec.z, 0.0f);
            quaternion result = *this * vec_quat * conjugate();
            return float3(result.x, result.y, result.z);
        }
        
        float3 transform_direction(const float3& dir) const
        {
            return transform_vector(dir).normalize();
        }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool is_identity() const
        {
			return Core::Math::approximately(x, 0.0f) && Core::Math::approximately(y, 0.0f) && 
                   Core::Math::approximately(z, 0.0f) && Core::Math::approximately(w, 1.0f);
        }
        
        bool is_normalized() const
        {
			return Core::Math::approximately(length_sq(), 1.0f);
        }
        
        bool isValid() const
        {
            return std::isfinite(x) && std::isfinite(y) && 
                   std::isfinite(z) && std::isfinite(w);
        }
        
        bool approximately(const quaternion& other, float epsilon = EPSILON) const
        {
            return dot(other) > (1.0f - epsilon);
        }
        
        std::string to_string() const
        {
            char buffer[64];
            snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f, %.3f)", x, y, z, w);
            return std::string(buffer);
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(const quaternion& rhs) const { return approximately(rhs); }
        bool operator!=(const quaternion& rhs) const { return !(*this == rhs); }
    };

    // ============================================================================
    // Бинарные операторы
    // ============================================================================
    inline quaternion operator+(quaternion lhs, const quaternion& rhs) { return lhs += rhs; }
    inline quaternion operator-(quaternion lhs, const quaternion& rhs) { return lhs -= rhs; }
    
    // Умножение кватернионов
    inline quaternion operator*(const quaternion& lhs, const quaternion& rhs)
    {
        return quaternion(
            lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
            lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
            lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
        );
    }
    
    // Умножение на скаляр
    inline quaternion operator*(quaternion q, float scalar) { return q *= scalar; }
    inline quaternion operator*(float scalar, quaternion q) { return q *= scalar; }
    
    // Деление на скаляр
    inline quaternion operator/(quaternion q, float scalar) { return q /= scalar; }
    
    // Умножение кватерниона на вектор
    inline float3 operator*(const quaternion& q, const float3& vec)
    {
        return q.transform_vector(vec);
    }

    // ============================================================================
    // Глобальные функции
    // ============================================================================
    inline float length(const quaternion& q) { return q.length(); }
    inline float length_sq(const quaternion& q) { return q.length_sq(); }
    inline quaternion normalize(const quaternion& q) { return q.normalize(); }
    inline quaternion conjugate(const quaternion& q) { return q.conjugate(); }
    inline quaternion inverse(const quaternion& q) { return q.inverse(); }
    inline float dot(const quaternion& a, const quaternion& b) { return a.dot(b); }
    
    inline float3x3 to_matrix3x3(const quaternion& q) { return q.to_matrix3x3(); }
    inline float4x4 to_matrix4x4(const quaternion& q) { return q.to_matrix4x4(); }
    
    inline quaternion slerp(const quaternion& a, const quaternion& b, float t)
    {
        return quaternion::slerp(a, b, t);
    }
    
    inline quaternion lerp(const quaternion& a, const quaternion& b, float t)
    {
        return (a * (1.0f - t) + b * t).normalize();
    }
    
    inline bool approximately(const quaternion& a, const quaternion& b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }
    
    inline bool isValid(const quaternion& q)
    {
        return q.isValid();
    }
    
    inline bool is_normalized(const quaternion& q)
    {
        return q.is_normalized();
    }

    // ============================================================================
    // Полезные константы
    // ============================================================================
    static const quaternion quaternion_identity = quaternion::identity();
    static const quaternion quaternion_zero = quaternion::zero();
} // namespace Core::Math
///////////////////////////////////////////////////////////////