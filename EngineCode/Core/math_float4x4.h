///////////////////////////////////////////////////////////////
// Created: 29.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <cmath>
#include <string>
#include <iomanip>
///////////////////////////////////////////////////////////////
#include "math_defines.h"
#include "math_constants.h"
#include "math_float4.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
///////////////////////////////////////////////////////////////
class float4x4;
class float3;
class float4;
///////////////////////////////////////////////////////////////
inline float4x4 operator+(float4x4 lhs, const float4x4& rhs);
inline float4x4 operator-(float4x4 lhs, const float4x4& rhs);
inline float4x4 operator*(const float4x4& lhs, const float4x4& rhs);
inline float4x4 operator*(float4x4 mat, float scalar);
inline float4x4 operator*(float scalar, float4x4 mat);
inline float4x4 operator/(float4x4 mat, float scalar);
inline float4 operator*(const float4& vec, const float4x4& mat);
inline float3 operator*(const float3& point, const float4x4& mat);
///////////////////////////////////////////////////////////////
    class float4x4
    {
    public:
        // ============================================================================
        // Данные матрицы (column-major как в HLSL и OpenGL)
        // ============================================================================
        union
        {
            struct
            {
                float4 col0;  // первый столбец
                float4 col1;  // второй столбец  
                float4 col2;  // третий столбец
                float4 col3;  // четвертый столбец
            };
            float4 cols[4];
            float m[4][4];
            float data[16];
        };

        // ============================================================================
        // Конструкторы
        // ============================================================================
        float4x4() = default;

        // Конструктор из столбцов
        float4x4(const float4& col0, const float4& col1, const float4& col2, const float4& col3)
            : col0(col0), col1(col1), col2(col2), col3(col3)
        {
        }

        // Конструктор из массива (column-major)
        explicit float4x4(const float* data)
        {
            for (int i = 0; i < 16; ++i)
                this->data[i] = data[i];
        }

        // Конструктор из скаляра (диагональная матрица)
        explicit float4x4(float scalar)
            : col0(scalar, 0, 0, 0)
            , col1(0, scalar, 0, 0)
            , col2(0, 0, scalar, 0)
            , col3(0, 0, 0, scalar)
        {
        }

        // Конструктор из D3DXMATRIX
        float4x4(const D3DXMATRIX& mat)
        {
            col0 = float4(mat._11, mat._21, mat._31, mat._41);
            col1 = float4(mat._12, mat._22, mat._32, mat._42);
            col2 = float4(mat._13, mat._23, mat._33, mat._43);
            col3 = float4(mat._14, mat._24, mat._34, mat._44);
        }

        // Конструктор копирования
        float4x4(const float4x4&) = default;

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        float4x4& operator=(const float4x4&) = default;

        float4x4& operator=(const D3DXMATRIX& mat)
        {
            col0 = float4(mat._11, mat._21, mat._31, mat._41);
            col1 = float4(mat._12, mat._22, mat._32, mat._42);
            col2 = float4(mat._13, mat._23, mat._33, mat._43);
            col3 = float4(mat._14, mat._24, mat._34, mat._44);
            return *this;
        }

        // ============================================================================
        // Статические конструкторы
        // ============================================================================
        static float4x4 identity()
        {
            return float4x4(
                float4(1, 0, 0, 0),
                float4(0, 1, 0, 0),
                float4(0, 0, 1, 0),
                float4(0, 0, 0, 1)
            );
        }

        static float4x4 zero()
        {
            return float4x4(
                float4(0, 0, 0, 0),
                float4(0, 0, 0, 0),
                float4(0, 0, 0, 0),
                float4(0, 0, 0, 0)
            );
        }

        // Матрица переноса
        static float4x4 translation(const float3& translation)
        {
            return float4x4(
                float4(1, 0, 0, 0),
                float4(0, 1, 0, 0),
                float4(0, 0, 1, 0),
                float4(translation.x, translation.y, translation.z, 1)
            );
        }

        static float4x4 translation(float x, float y, float z)
        {
            return translation(float3(x, y, z));
        }

        // Матрица масштабирования
        static float4x4 scaling(const float3& scale)
        {
            return float4x4(
                float4(scale.x, 0, 0, 0),
                float4(0, scale.y, 0, 0),
                float4(0, 0, scale.z, 0),
                float4(0, 0, 0, 1)
            );
        }

        static float4x4 scaling(float x, float y, float z)
        {
            return scaling(float3(x, y, z));
        }

        static float4x4 scaling(float uniformScale)
        {
            return scaling(float3(uniformScale));
        }

        // Матрицы вращения
        static float4x4 rotation_x(float angle)
        {
            float s = sin(angle);
            float c = cos(angle);
            
            return float4x4(
                float4(1, 0, 0, 0),
                float4(0, c, s, 0),
                float4(0, -s, c, 0),
                float4(0, 0, 0, 1)
            );
        }

        static float4x4 rotation_y(float angle)
        {
            float s = sin(angle);
            float c = cos(angle);
            
            return float4x4(
                float4(c, 0, -s, 0),
                float4(0, 1, 0, 0),
                float4(s, 0, c, 0),
                float4(0, 0, 0, 1)
            );
        }

        static float4x4 rotation_z(float angle)
        {
            float s = sin(angle);
            float c = cos(angle);
            
            return float4x4(
                float4(c, s, 0, 0),
                float4(-s, c, 0, 0),
                float4(0, 0, 1, 0),
                float4(0, 0, 0, 1)
            );
        }

        // Матрица вращения вокруг произвольной оси
        static float4x4 rotation_axis(const float3& axis, float angle)
        {
            float3 n = axis.normalize();
            float s = sin(angle);
            float c = cos(angle);
            float t = 1 - c;

            return float4x4(
                float4(t * n.x * n.x + c, t * n.x * n.y + s * n.z, t * n.x * n.z - s * n.y, 0),
                float4(t * n.x * n.y - s * n.z, t * n.y * n.y + c, t * n.y * n.z + s * n.x, 0),
                float4(t * n.x * n.z + s * n.y, t * n.y * n.z - s * n.x, t * n.z * n.z + c, 0),
                float4(0, 0, 0, 1)
            );
        }

        // Матрица ортографической проекции
        static float4x4 orthographic(float left, float right, float bottom, float top, float zNear, float zFar)
        {
            float rcpWidth = 1.0f / (right - left);
            float rcpHeight = 1.0f / (top - bottom);
            float rcpDepth = 1.0f / (zFar - zNear);

            return float4x4(
                float4(2.0f * rcpWidth, 0, 0, 0),
                float4(0, 2.0f * rcpHeight, 0, 0),
                float4(0, 0, rcpDepth, 0),
                float4(-(right + left) * rcpWidth, -(top + bottom) * rcpHeight, -zNear * rcpDepth, 1)
            );
        }

        // Матрица перспективной проекции
        static float4x4 perspective(float fovY, float aspect, float zNear, float zFar)
        {
            float tanHalfFov = tan(fovY * 0.5f);
            float f = 1.0f / tanHalfFov;
            float rcpDepth = 1.0f / (zFar - zNear);

            return float4x4(
                float4(f / aspect, 0, 0, 0),
                float4(0, f, 0, 0),
                float4(0, 0, zFar * rcpDepth, 1),
                float4(0, 0, -zNear * zFar * rcpDepth, 0)
            );
        }

        // Матрица вида (look-at)
        static float4x4 look_at(const float3& eye, const float3& target, const float3& up)
        {
            float3 z = (target - eye).normalize();
            float3 x = cross(up, z).normalize();
            float3 y = cross(z, x);

            return float4x4(
                float4(x.x, y.x, z.x, 0),
                float4(x.y, y.y, z.y, 0),
                float4(x.z, y.z, z.z, 0),
                float4(-dot(x, eye), -dot(y, eye), -dot(z, eye), 1)
            );
        }

        // ============================================================================
        // Статические методы конвертации
        // ============================================================================
        static float4x4 from_d3dxmatrix(const D3DXMATRIX& mat)
        {
            return float4x4(mat);
        }

        // ============================================================================
        // Операторы доступа
        // ============================================================================
        float4& operator[](int colIndex)
        {
            return cols[colIndex];
        }

        const float4& operator[](int colIndex) const
        {
            return cols[colIndex];
        }

        float& operator()(int row, int col)
        {
            return m[col][row];  // column-major: [col][row]
        }

        const float& operator()(int row, int col) const
        {
            return m[col][row];  // column-major: [col][row]
        }

        // ============================================================================
        // Арифметические операторы с присваиванием
        // ============================================================================
        float4x4& operator+=(const float4x4& rhs)
        {
            col0 += rhs.col0;
            col1 += rhs.col1;
            col2 += rhs.col2;
            col3 += rhs.col3;
            return *this;
        }

        float4x4& operator-=(const float4x4& rhs)
        {
            col0 -= rhs.col0;
            col1 -= rhs.col1;
            col2 -= rhs.col2;
            col3 -= rhs.col3;
            return *this;
        }

        float4x4& operator*=(const float4x4& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        float4x4& operator*=(float scalar)
        {
            col0 *= scalar;
            col1 *= scalar;
            col2 *= scalar;
            col3 *= scalar;
            return *this;
        }

        float4x4& operator/=(float scalar)
        {
            float invScalar = 1.0f / scalar;
            col0 *= invScalar;
            col1 *= invScalar;
            col2 *= invScalar;
            col3 *= invScalar;
            return *this;
        }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        float4x4 operator+() const { return *this; }
        float4x4 operator-() const { return float4x4(-col0, -col1, -col2, -col3); }

        // ============================================================================
        // Конвертация в другие типы
        // ============================================================================
        operator D3DXMATRIX() const
        {
            D3DXMATRIX result;
            
            result._11 = col0.x; result._12 = col1.x; result._13 = col2.x; result._14 = col3.x;
            result._21 = col0.y; result._22 = col1.y; result._23 = col2.y; result._24 = col3.y;
            result._31 = col0.z; result._32 = col1.z; result._33 = col2.z; result._34 = col3.z;
            result._41 = col0.w; result._42 = col1.w; result._43 = col2.w; result._44 = col3.w;
            
            return result;
        }

        // ============================================================================
        // Математические операции
        // ============================================================================
        float4x4 transposed() const
        {
            return float4x4(
                float4(col0.x, col1.x, col2.x, col3.x),
                float4(col0.y, col1.y, col2.y, col3.y),
                float4(col0.z, col1.z, col2.z, col3.z),
                float4(col0.w, col1.w, col2.w, col3.w)
            );
        }

        float4x4 inverted() const
        {
            float det = determinant();
            float4x4 adjugate = this->adjugate();
            return adjugate * (1.0f / det);
        }

        float determinant() const
        {
            float det = col0.x * (
                col1.y * (col2.z * col3.w - col2.w * col3.z) -
                col1.z * (col2.y * col3.w - col2.w * col3.y) +
                col1.w * (col2.y * col3.z - col2.z * col3.y)
            ) - col0.y * (
                col1.x * (col2.z * col3.w - col2.w * col3.z) -
                col1.z * (col2.x * col3.w - col2.w * col3.x) +
                col1.w * (col2.x * col3.z - col2.z * col3.x)
            ) + col0.z * (
                col1.x * (col2.y * col3.w - col2.w * col3.y) -
                col1.y * (col2.x * col3.w - col2.w * col3.x) +
                col1.w * (col2.x * col3.y - col2.y * col3.x)
            ) - col0.w * (
                col1.x * (col2.y * col3.z - col2.z * col3.y) -
                col1.y * (col2.x * col3.z - col2.z * col3.x) +
                col1.z * (col2.x * col3.y - col2.y * col3.x)
            );

            return det;
        }

        // Присоединенная матрица (adjugate)
        float4x4 adjugate() const
        {
			float4x4 result = zero();

            result(0,0) =  (col1.y * (col2.z * col3.w - col2.w * col3.z) - col1.z * (col2.y * col3.w - col2.w * col3.y) + col1.w * (col2.y * col3.z - col2.z * col3.y));
            result(0,1) = -(col1.x * (col2.z * col3.w - col2.w * col3.z) - col1.z * (col2.x * col3.w - col2.w * col3.x) + col1.w * (col2.x * col3.z - col2.z * col3.x));
            result(0,2) =  (col1.x * (col2.y * col3.w - col2.w * col3.y) - col1.y * (col2.x * col3.w - col2.w * col3.x) + col1.w * (col2.x * col3.y - col2.y * col3.x));
            result(0,3) = -(col1.x * (col2.y * col3.z - col2.z * col3.y) - col1.y * (col2.x * col3.z - col2.z * col3.x) + col1.z * (col2.x * col3.y - col2.y * col3.x));

            result(1,0) = -(col0.y * (col2.z * col3.w - col2.w * col3.z) - col0.z * (col2.y * col3.w - col2.w * col3.y) + col0.w * (col2.y * col3.z - col2.z * col3.y));
            result(1,1) =  (col0.x * (col2.z * col3.w - col2.w * col3.z) - col0.z * (col2.x * col3.w - col2.w * col3.x) + col0.w * (col2.x * col3.z - col2.z * col3.x));
            result(1,2) = -(col0.x * (col2.y * col3.w - col2.w * col3.y) - col0.y * (col2.x * col3.w - col2.w * col3.x) + col0.w * (col2.x * col3.y - col2.y * col3.x));
            result(1,3) =  (col0.x * (col2.y * col3.z - col2.z * col3.y) - col0.y * (col2.x * col3.z - col2.z * col3.x) + col0.z * (col2.x * col3.y - col2.y * col3.x));

            result(2,0) =  (col0.y * (col1.z * col3.w - col1.w * col3.z) - col0.z * (col1.y * col3.w - col1.w * col3.y) + col0.w * (col1.y * col3.z - col1.z * col3.y));
            result(2,1) = -(col0.x * (col1.z * col3.w - col1.w * col3.z) - col0.z * (col1.x * col3.w - col1.w * col3.x) + col0.w * (col1.x * col3.z - col1.z * col3.x));
            result(2,2) =  (col0.x * (col1.y * col3.w - col1.w * col3.y) - col0.y * (col1.x * col3.w - col1.w * col3.x) + col0.w * (col1.x * col3.y - col1.y * col3.x));
            result(2,3) = -(col0.x * (col1.y * col3.z - col1.z * col3.y) - col0.y * (col1.x * col3.z - col1.z * col3.x) + col0.z * (col1.x * col3.y - col1.y * col3.x));

            result(3,0) = -(col0.y * (col1.z * col2.w - col1.w * col2.z) - col0.z * (col1.y * col2.w - col1.w * col2.y) + col0.w * (col1.y * col2.z - col1.z * col2.y));
            result(3,1) =  (col0.x * (col1.z * col2.w - col1.w * col2.z) - col0.z * (col1.x * col2.w - col1.w * col2.x) + col0.w * (col1.x * col2.z - col1.z * col2.x));
            result(3,2) = -(col0.x * (col1.y * col2.w - col1.w * col2.y) - col0.y * (col1.x * col2.w - col1.w * col2.x) + col0.w * (col1.x * col2.y - col1.y * col2.x));
            result(3,3) =  (col0.x * (col1.y * col2.z - col1.z * col2.y) - col0.y * (col1.x * col2.z - col1.z * col2.x) + col0.z * (col1.x * col2.y - col1.y * col2.x));

            return result;
        }

        // ============================================================================
        // Преобразования векторов и точек
        // ============================================================================
        float4 transform_vector(const float4& vec) const
        {
            return float4(
                dot(col0, vec),
                dot(col1, vec),
                dot(col2, vec),
                dot(col3, vec)
            );
        }

        float3 transform_point(const float3& point) const
        {
            float4 result = transform_vector(float4(point, 1.0f));
            return result.xyz() / result.w;
        }

        float3 transform_vector(const float3& vec) const
        {
            float4 result = transform_vector(float4(vec, 0.0f));
            return result.xyz();
        }

        float3 transform_direction(const float3& dir) const
        {
            return transform_vector(dir).normalize();
        }

        // ============================================================================
        // Извлечение компонентов трансформации
        // ============================================================================
        float3 get_translation() const
        {
            return float3(col3.x, col3.y, col3.z);
        }

        float3 get_scale() const
        {
            return float3(
                float3(col0.x, col0.y, col0.z).length(),
                float3(col1.x, col1.y, col1.z).length(),
                float3(col2.x, col2.y, col2.z).length()
            );
        }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool is_identity() const
        {
			return Core::Math::approximately(col0, float4(1, 0, 0, 0)) &&
				   Core::Math::approximately(col1, float4(0, 1, 0, 0)) &&
				   Core::Math::approximately(col2, float4(0, 0, 1, 0)) &&
				   Core::Math::approximately(col3, float4(0, 0, 0, 1));
        }

        bool is_affine() const
        {
            return Core::Math::approximately(col0.w, 0.0f) &&
                   Core::Math::approximately(col1.w, 0.0f) &&
				   Core::Math::approximately(col2.w, 0.0f) &&
                   Core::Math::approximately(col3.w, 1.0f);
        }

        bool isValid() const
        {
            return col0.isValid() && col1.isValid() && col2.isValid() && col3.isValid();
        }

        bool approximately(const float4x4& other, float epsilon = EPSILON) const
        {
            return col0.approximately(other.col0, epsilon) &&
                   col1.approximately(other.col1, epsilon) &&
                   col2.approximately(other.col2, epsilon) &&
                   col3.approximately(other.col3, epsilon);
        }

        std::string to_string() const
        {
            char buffer[256];
            snprintf(buffer, sizeof(buffer), 
                "[%.3f, %.3f, %.3f, %.3f]\n"
                "[%.3f, %.3f, %.3f, %.3f]\n"
                "[%.3f, %.3f, %.3f, %.3f]\n"
                "[%.3f, %.3f, %.3f, %.3f]",
                col0.x, col1.x, col2.x, col3.x,
                col0.y, col1.y, col2.y, col3.y,
                col0.z, col1.z, col2.z, col3.z,
                col0.w, col1.w, col2.w, col3.w);
            return std::string(buffer);
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(const float4x4& rhs) const { return approximately(rhs); }
        bool operator!=(const float4x4& rhs) const { return !(*this == rhs); }
    };
	// ============================================================================
	// Полезные константы
	// ============================================================================
	static const float4x4 float4x4_identity = float4x4::identity();
	static const float4x4 float4x4_zero = float4x4::zero();

    // ============================================================================
    // Бинарные операторы
    // ============================================================================
    inline float4x4 operator+(float4x4 lhs, const float4x4& rhs) { return lhs += rhs; }
    inline float4x4 operator-(float4x4 lhs, const float4x4& rhs) { return lhs -= rhs; }

    // Умножение матриц
    inline float4x4 operator*(const float4x4& lhs, const float4x4& rhs)
    {
		float4x4 result = float4x4_zero;
        
        for (int col = 0; col < 4; ++col)
        {
            float4 rhsCol = rhs[col];
            result[col] = float4(
                dot(lhs[0], rhsCol),
                dot(lhs[1], rhsCol),
                dot(lhs[2], rhsCol),
                dot(lhs[3], rhsCol)
            );
        }
        
        return result;
    }

    // Умножение на скаляр
    inline float4x4 operator*(float4x4 mat, float scalar) { return mat *= scalar; }
    inline float4x4 operator*(float scalar, float4x4 mat) { return mat *= scalar; }

    // Деление на скаляр
    inline float4x4 operator/(float4x4 mat, float scalar) { return mat /= scalar; }

    // Умножение матрицы на вектор
    inline float4 operator*(const float4& vec, const float4x4& mat)
    {
        return mat.transform_vector(vec);
    }

    inline float3 operator*(const float3& point, const float4x4& mat)
    {
        return mat.transform_point(point);
    }

    // ============================================================================
    // Глобальные функции
    // ============================================================================
    inline float4x4 transpose(const float4x4& mat) { return mat.transposed(); }
    inline float4x4 inverse(const float4x4& mat) { return mat.inverted(); }
    inline float determinant(const float4x4& mat) { return mat.determinant(); }
    inline float4 mul(const float4& vec, const float4x4& mat) { return vec * mat; }
    inline float3 mul(const float3& point, const float4x4& mat) { return point * mat; }
    inline float4x4 mul(const float4x4& lhs, const float4x4& rhs) { return lhs * rhs; }

    inline bool approximately(const float4x4& a, const float4x4& b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }

    inline bool isValid(const float4x4& mat)
    {
        return mat.isValid();
    }
} // namespace Core::Math
///////////////////////////////////////////////////////////////