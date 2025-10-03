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
#include "math_float4x4.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
///////////////////////////////////////////////////////////////
class float4x4;
class float3x3;
class float3;
class float4;
///////////////////////////////////////////////////////////////
inline float3x3 operator+(float3x3 lhs, const float3x3& rhs);
inline float3x3 operator-(float3x3 lhs, const float3x3& rhs);
inline float3x3 operator*(const float3x3& lhs, const float3x3& rhs);
inline float3x3 operator*(float3x3 mat, float scalar);
inline float3x3 operator*(float scalar, float3x3 mat);
inline float3x3 operator/(float3x3 mat, float scalar);
inline float3 operator*(const float3& point, const float3x3& mat);
///////////////////////////////////////////////////////////////
    class float3x3
    {
    public:
        // ============================================================================
        // Данные матрицы (column-major как в HLSL и OpenGL)
        // ============================================================================
        union
        {
            struct
            {
                float3 col0;  // первый столбец
                float3 col1;  // второй столбец  
                float3 col2;  // третий столбец
            };
            float3 cols[3];
            float m[3][3];
            float data[9];
        };

        // ============================================================================
        // Конструкторы
        // ============================================================================
        float3x3() = default;

        // Конструктор из столбцов
        float3x3(const float3& col0, const float3& col1, const float3& col2)
            : col0(col0), col1(col1), col2(col2)
        {
        }

        // Конструктор из массива (column-major)
        explicit float3x3(const float* data)
        {
            for (int i = 0; i < 9; ++i)
                this->data[i] = data[i];
        }

        // Конструктор из скаляра (диагональная матрица)
        explicit float3x3(float scalar)
            : col0(scalar, 0, 0)
            , col1(0, scalar, 0)
            , col2(0, 0, scalar)
        {
        }

        // Конструктор из float4x4 (извлекает верхнюю левую 3x3 часть)
        explicit float3x3(const float4x4& mat4x4)
            : col0(mat4x4.col0.xyz())
            , col1(mat4x4.col1.xyz())
            , col2(mat4x4.col2.xyz())
        {
        }

        // Конструктор из D3DXMATRIX (извлекает верхнюю левую 3x3 часть)
        explicit float3x3(const D3DXMATRIX& mat)
            : col0(mat._11, mat._21, mat._31)
            , col1(mat._12, mat._22, mat._32)
            , col2(mat._13, mat._23, mat._33)
        {
        }

        // Конструктор копирования
        float3x3(const float3x3&) = default;

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        float3x3& operator=(const float3x3&) = default;

        float3x3& operator=(const float4x4& mat4x4)
        {
            col0 = mat4x4.col0.xyz();
            col1 = mat4x4.col1.xyz();
            col2 = mat4x4.col2.xyz();
            return *this;
        }

        // ============================================================================
        // Статические конструкторы
        // ============================================================================
        static float3x3 identity()
        {
            return float3x3(
                float3(1, 0, 0),
                float3(0, 1, 0),
                float3(0, 0, 1)
            );
        }

        static float3x3 zero()
        {
            return float3x3(
                float3(0, 0, 0),
                float3(0, 0, 0),
                float3(0, 0, 0)
            );
        }

        // Матрица масштабирования
        static float3x3 scaling(const float3& scale)
        {
            return float3x3(
                float3(scale.x, 0, 0),
                float3(0, scale.y, 0),
                float3(0, 0, scale.z)
            );
        }

        static float3x3 scaling(float x, float y, float z)
        {
            return scaling(float3(x, y, z));
        }

        static float3x3 scaling(float uniformScale)
        {
            return scaling(float3(uniformScale));
        }

        // Матрицы вращения
        static float3x3 rotation_x(float angle)
        {
            float s = sin(angle);
            float c = cos(angle);
            
            return float3x3(
                float3(1, 0, 0),
                float3(0, c, s),
                float3(0, -s, c)
            );
        }

        static float3x3 rotation_y(float angle)
        {
            float s = sin(angle);
            float c = cos(angle);
            
            return float3x3(
                float3(c, 0, -s),
                float3(0, 1, 0),
                float3(s, 0, c)
            );
        }

        static float3x3 rotation_z(float angle)
        {
            float s = sin(angle);
            float c = cos(angle);
            
            return float3x3(
                float3(c, s, 0),
                float3(-s, c, 0),
                float3(0, 0, 1)
            );
        }

        // Матрица вращения вокруг произвольной оси
        static float3x3 rotation_axis(const float3& axis, float angle)
        {
            float3 n = axis.normalize();
            float s = sin(angle);
            float c = cos(angle);
            float t = 1 - c;

            return float3x3(
                float3(t * n.x * n.x + c, t * n.x * n.y + s * n.z, t * n.x * n.z - s * n.y),
                float3(t * n.x * n.y - s * n.z, t * n.y * n.y + c, t * n.y * n.z + s * n.x),
                float3(t * n.x * n.z + s * n.y, t * n.y * n.z - s * n.x, t * n.z * n.z + c)
            );
        }

        // Матрица из кватерниона (будет реализована позже с классом quaternion)
        // static float3x3 from_quaternion(const quaternion& q);

        // Матрица из базисных векторов
        static float3x3 from_basis(const float3& x, const float3& y, const float3& z)
        {
            return float3x3(x, y, z);
        }

        // Матрица для преобразования нормалей (transpose(inverse(mat3x3)))
        static float3x3 normal_matrix(const float3x3& model)
        {
            return model.inverted().transposed();
        }

        // ============================================================================
        // Операторы доступа
        // ============================================================================
        float3& operator[](int colIndex)
        {
            return cols[colIndex];
        }

        const float3& operator[](int colIndex) const
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
        float3x3& operator+=(const float3x3& rhs)
        {
            col0 += rhs.col0;
            col1 += rhs.col1;
            col2 += rhs.col2;
            return *this;
        }

        float3x3& operator-=(const float3x3& rhs)
        {
            col0 -= rhs.col0;
            col1 -= rhs.col1;
            col2 -= rhs.col2;
            return *this;
        }

        float3x3& operator*=(const float3x3& rhs)
        {
            *this = *this * rhs;
            return *this;
        }

        float3x3& operator*=(float scalar)
        {
            col0 *= scalar;
            col1 *= scalar;
            col2 *= scalar;
            return *this;
        }

        float3x3& operator/=(float scalar)
        {
            float invScalar = 1.0f / scalar;
            col0 *= invScalar;
            col1 *= invScalar;
            col2 *= invScalar;
            return *this;
        }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        float3x3 operator+() const { return *this; }
        float3x3 operator-() const { return float3x3(-col0, -col1, -col2); }

        // ============================================================================
        // Математические операции
        // ============================================================================
        float3x3 transposed() const
        {
            return float3x3(
                float3(col0.x, col1.x, col2.x),
                float3(col0.y, col1.y, col2.y),
                float3(col0.z, col1.z, col2.z)
            );
        }

        float3x3 inverted() const
        {
            float det = determinant();
            float3x3 adjugate = this->adjugate();
            return adjugate * (1.0f / det);
        }

        float determinant() const
        {
            return col0.x * (col1.y * col2.z - col1.z * col2.y)
                 - col0.y * (col1.x * col2.z - col1.z * col2.x)
                 + col0.z * (col1.x * col2.y - col1.y * col2.x);
        }

        // Присоединенная матрица (adjugate)
        float3x3 adjugate() const
        {
            return float3x3(
                float3(
                    col1.y * col2.z - col1.z * col2.y,
                    col0.z * col2.y - col0.y * col2.z,
                    col0.y * col1.z - col0.z * col1.y
                ),
                float3(
                    col1.z * col2.x - col1.x * col2.z,
                    col0.x * col2.z - col0.z * col2.x,
                    col0.z * col1.x - col0.x * col1.z
                ),
                float3(
                    col1.x * col2.y - col1.y * col2.x,
                    col0.y * col2.x - col0.x * col2.y,
                    col0.x * col1.y - col0.y * col1.x
                )
            ).transposed(); // транспонируем кофактор для получения adjugate
        }

        // След матрицы (сумма диагональных элементов)
        float trace() const
        {
            return col0.x + col1.y + col2.z;
        }

        // ============================================================================
        // Преобразования векторов
        // ============================================================================
        float3 transform_vector(const float3& vec) const
        {
            return float3(
                dot(col0, vec),
                dot(col1, vec),
                dot(col2, vec)
            );
        }

        float3 transform_direction(const float3& dir) const
        {
            return transform_vector(dir).normalize();
        }

        // Преобразование нормали (использует обратную транспонированную матрицу)
        float3 transform_normal(const float3& normal) const
        {
            return inverted().transposed().transform_vector(normal).normalize();
        }

        // ============================================================================
        // Извлечение компонентов
        // ============================================================================
        float3 get_scale() const
        {
            return float3(
                col0.length(),
                col1.length(),
                col2.length()
            );
        }

        // Получение базисных векторов
        float3 right() const { return col0.normalize(); }
        float3 up() const { return col1.normalize(); }
        float3 forward() const { return col2.normalize(); }

        // Получение осей вращения
        float3 get_rotation_axis_x() const { return float3(col0.x, col1.x, col2.x).normalize(); }
        float3 get_rotation_axis_y() const { return float3(col0.y, col1.y, col2.y).normalize(); }
        float3 get_rotation_axis_z() const { return float3(col0.z, col1.z, col2.z).normalize(); }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool is_identity() const
        {
            return Core::Math::approximately(col0, float3(1,0,0)) &&
                   Core::Math::approximately(col1, float3(0,1,0)) &&
                   Core::Math::approximately(col2, float3(0,0,1));
        }

        bool isOrthogonal() const
        {
			return Core::Math::approximately(dot(col0, col1), 0.0f) &&
				   Core::Math::approximately(dot(col0, col2), 0.0f) &&
                   Core::Math::approximately(dot(col1, col2), 0.0f);
        }

        bool isOrthonormal() const
        {
            return isOrthogonal() &&
                   Core::Math::approximately(col0.length_sq(), 1.0f) &&
				   Core::Math::approximately(col1.length_sq(), 1.0f) &&
				   Core::Math::approximately(col2.length_sq(), 1.0f);
        }

        bool isValid() const
        {
            return col0.isValid() && col1.isValid() && col2.isValid();
        }

        bool approximately(const float3x3& other, float epsilon = EPSILON) const
        {
            return col0.approximately(other.col0, epsilon) &&
                   col1.approximately(other.col1, epsilon) &&
                   col2.approximately(other.col2, epsilon);
        }

        std::string to_string() const
        {
            char buffer[128];
            snprintf(buffer, sizeof(buffer), 
                "[%.3f, %.3f, %.3f]\n"
                "[%.3f, %.3f, %.3f]\n"
                "[%.3f, %.3f, %.3f]",
                col0.x, col1.x, col2.x,
                col0.y, col1.y, col2.y,
                col0.z, col1.z, col2.z);
            return std::string(buffer);
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(const float3x3& rhs) const { return approximately(rhs); }
        bool operator!=(const float3x3& rhs) const { return !(*this == rhs); }
    };

    // ============================================================================
    // Бинарные операторы
    // ============================================================================
    inline float3x3 operator+(float3x3 lhs, const float3x3& rhs) { return lhs += rhs; }
    inline float3x3 operator-(float3x3 lhs, const float3x3& rhs) { return lhs -= rhs; }

    // Умножение матриц
    inline float3x3 operator*(const float3x3& lhs, const float3x3& rhs)
    {
        float3x3 result = float3x3::zero();
        
        for (int col = 0; col < 3; ++col)
        {
            float3 rhsCol = rhs[col];
            result[col] = float3(
                dot(lhs[0], rhsCol),
                dot(lhs[1], rhsCol),
                dot(lhs[2], rhsCol)
            );
        }
        
        return result;
    }

    // Умножение на скаляр
    inline float3x3 operator*(float3x3 mat, float scalar) { return mat *= scalar; }
    inline float3x3 operator*(float scalar, float3x3 mat) { return mat *= scalar; }

    // Деление на скаляр
    inline float3x3 operator/(float3x3 mat, float scalar) { return mat /= scalar; }

    // Умножение матрицы на вектор
    inline float3 operator*(const float3& vec, const float3x3& mat)
    {
        return mat.transform_vector(vec);
    }

    // ============================================================================
    // Глобальные функции
    // ============================================================================
    inline float3x3 transpose(const float3x3& mat) { return mat.transposed(); }
    inline float3x3 inverse(const float3x3& mat) { return mat.inverted(); }
    inline float determinant(const float3x3& mat) { return mat.determinant(); }
    inline float3 mul(const float3& vec, const float3x3& mat) { return vec * mat; }
    inline float3x3 mul(const float3x3& lhs, const float3x3& rhs) { return lhs * rhs; }

    inline float trace(const float3x3& mat) { return mat.trace(); }

    inline bool approximately(const float3x3& a, const float3x3& b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }

    inline bool isValid(const float3x3& mat)
    {
        return mat.isValid();
    }

    inline bool isOrthogonal(const float3x3& mat)
    {
        return mat.isOrthogonal();
    }

    inline bool isOrthonormal(const float3x3& mat)
    {
        return mat.isOrthonormal();
    }

    // Матрица для преобразования нормалей
    inline float3x3 normal_matrix(const float3x3& model)
    {
        return float3x3::normal_matrix(model);
    }

    // ============================================================================
    // Полезные константы
    // ============================================================================
    static const float3x3 float3x3_identity = float3x3::identity();
    static const float3x3 float3x3_zero = float3x3::zero();
} // namespace Core::Math
///////////////////////////////////////////////////////////////