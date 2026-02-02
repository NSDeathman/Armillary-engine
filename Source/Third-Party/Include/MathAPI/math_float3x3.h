#pragma once

#include <cmath>
#include <string>
#include <cstdio>
#include <xmmintrin.h>
#include <pmmintrin.h>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"
#include "math_float3.h"
#include "math_float4.h"

namespace Math
{
    class float4x4;
    class quaternion;

    class float3x3
    {
    public:
        alignas(16) float4 row0_, row1_, row2_;

    public:
        float3x3() noexcept;
        float3x3(const float3& row0, const float3& row1, const float3& row2) noexcept;
        float3x3(float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22) noexcept;
        explicit float3x3(const float* data) noexcept;
        explicit float3x3(float scalar) noexcept;
        explicit float3x3(const float3& diagonal) noexcept;
        explicit float3x3(const float4x4& mat4x4) noexcept;
        explicit float3x3(const quaternion& q) noexcept;
        float3x3(const float3x3&) noexcept = default;

        float3x3& operator=(const float3x3&) noexcept = default;
        float3x3& operator=(const float4x4& mat4x4) noexcept;

        float3& operator[](int rowIndex) noexcept;
        const float3& operator[](int rowIndex) const noexcept;
        float& operator()(int row, int col) noexcept;
        const float& operator()(int row, int col) const noexcept;

        float3 row0() const noexcept;
        float3 row1() const noexcept;
        float3 row2() const noexcept;
        float3 col0() const noexcept;
        float3 col1() const noexcept;
        float3 col2() const noexcept;
        void set_row0(const float3& row) noexcept;
        void set_row1(const float3& row) noexcept;
        void set_row2(const float3& row) noexcept;
        void set_col0(const float3& col) noexcept;
        void set_col1(const float3& col) noexcept;
        void set_col2(const float3& col) noexcept;

        static float3x3 identity() noexcept;
        static float3x3 zero() noexcept;
        static float3x3 scaling(const float3& scale) noexcept;
        static float3x3 scaling(float scaleX, float scaleY, float scaleZ) noexcept;
        static float3x3 scaling(float scale) noexcept;
        static float3x3 rotation_x(float angle) noexcept;
        static float3x3 rotation_y(float angle) noexcept;
        static float3x3 rotation_z(float angle) noexcept;
        static float3x3 rotation_axis(const float3& axis, float angle) noexcept;
        static float3x3 rotation_euler(const float3& angles) noexcept;
        static float3x3 skew_symmetric(const float3& vec) noexcept;
        static float3x3 outer_product(const float3& u, const float3& v) noexcept;

        bool approximately_zero(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        float3x3& operator+=(const float3x3& rhs) noexcept;
        float3x3& operator-=(const float3x3& rhs) noexcept;
        float3x3& operator*=(float scalar) noexcept;
        float3x3& operator/=(float scalar) noexcept;
        float3x3& operator*=(const float3x3& rhs) noexcept;

        float3x3 operator+() const noexcept;
        float3x3 operator-() const noexcept;

        float3x3 transposed() const noexcept;
        float determinant() const noexcept;
        float3x3 inverted() const noexcept;
        static float3x3 normal_matrix(const float3x3& model) noexcept;
        float trace() const noexcept;
        float3 diagonal() const noexcept;
        float frobenius_norm() const noexcept;
        float3x3 symmetric_part() const noexcept;
        float3x3 skew_symmetric_part() const noexcept;

        float3 transform_vector(const float3& vec) const noexcept;
        float3 transform_point(const float3& point) const noexcept;
        float3 transform_normal(const float3& normal) const noexcept;

        float3 extract_scale() const noexcept;
        float3x3 extract_rotation() const noexcept;

        bool is_identity(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool is_orthogonal(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool is_orthonormal(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool approximately(const float3x3& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        std::string to_string() const;
        void to_row_major(float* data) const noexcept;
        void to_column_major(float* data) const noexcept;
        bool isValid() const noexcept;

        bool operator==(const float3x3& rhs) const noexcept;
        bool operator!=(const float3x3& rhs) const noexcept;
    };

    float3x3 operator+(float3x3 lhs, const float3x3& rhs) noexcept;
    float3x3 operator-(float3x3 lhs, const float3x3& rhs) noexcept;
    float3x3 operator*(const float3x3& lhs, const float3x3& rhs) noexcept;
    float3x3 operator*(float3x3 mat, float scalar) noexcept;
    float3x3 operator*(float scalar, float3x3 mat) noexcept;
    float3x3 operator/(float3x3 mat, float scalar) noexcept;
    float3 operator*(const float3& vec, const float3x3& mat) noexcept;
    float3 operator*(const float3x3& mat, const float3& vec) noexcept;

    float3x3 transpose(const float3x3& mat) noexcept;
    float3x3 inverse(const float3x3& mat) noexcept;
    float determinant(const float3x3& mat) noexcept;
    float3 mul(const float3& vec, const float3x3& mat) noexcept;
    float3x3 mul(const float3x3& lhs, const float3x3& rhs) noexcept;
    float trace(const float3x3& mat) noexcept;
    float3 diagonal(const float3x3& mat) noexcept;
    float frobenius_norm(const float3x3& mat) noexcept;
    bool approximately(const float3x3& a, const float3x3& b, float epsilon) noexcept;
    bool is_orthogonal(const float3x3& mat, float epsilon) noexcept;
    bool is_orthonormal(const float3x3& mat, float epsilon) noexcept;
    float3x3 normal_matrix(const float3x3& model) noexcept;
    float3 extract_scale(const float3x3& mat) noexcept;
    float3x3 extract_rotation(const float3x3& mat) noexcept;
    float3x3 skew_symmetric(const float3& vec) noexcept;
    float3x3 outer_product(const float3& u, const float3& v) noexcept;

    extern const float3x3 float3x3_Identity;
    extern const float3x3 float3x3_Zero;

}

#include "math_float4x4.h"
#include "math_quaternion.h"
#include "math_float3x3.inl"
