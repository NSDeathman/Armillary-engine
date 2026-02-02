#pragma once

#include <cmath>
#include <string>
#include <cstdio>
#include <algorithm>
#include <xmmintrin.h>
#include <pmmintrin.h>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"
#include "math_float3.h"
#include "math_float4.h"

namespace Math
{
    class float3x3;
    class quaternion;

    class float4x4
    {
    public:
        alignas(16) float4 row0_;
        alignas(16) float4 row1_;
        alignas(16) float4 row2_;
        alignas(16) float4 row3_;

    public:
        float4x4() noexcept;
        float4x4(const float4& r0, const float4& r1, const float4& r2, const float4& r3) noexcept;
        float4x4(float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33) noexcept;
        explicit float4x4(const float* data) noexcept;
        explicit float4x4(float scalar) noexcept;
        explicit float4x4(const float4& diagonal) noexcept;
        explicit float4x4(const float3x3& mat3x3) noexcept;
        explicit float4x4(const quaternion& q) noexcept;
        float4x4(const float4x4&) noexcept = default;

        float4x4& operator=(const float4x4&) noexcept = default;

        static float4x4 identity() noexcept;
        static float4x4 zero() noexcept;

        static float4x4 translation(float x, float y, float z) noexcept;
        static float4x4 translation(const float3& translation) noexcept;
        static float4x4 scaling(float x, float y, float z) noexcept;
        static float4x4 scaling(const float3& scale) noexcept;
        static float4x4 scaling(float uniformScale) noexcept;
        static float4x4 rotation_x(float angle) noexcept;
        static float4x4 rotation_y(float angle) noexcept;
        static float4x4 rotation_z(float angle) noexcept;
        static float4x4 rotation_axis(const float3& axis, float angle) noexcept;
        static float4x4 rotation_euler(const float3& angles) noexcept;
        static float4x4 TRS(const float3& translation, const quaternion& rotation, const float3& scale) noexcept;

        static float4x4 perspective_lh_zo(float fovY, float aspect, float zNear, float zFar) noexcept;
        static float4x4 perspective_rh_zo(float fovY, float aspect, float zNear, float zFar) noexcept;
        static float4x4 perspective_lh_no(float fovY, float aspect, float zNear, float zFar) noexcept;
        static float4x4 perspective_rh_no(float fovY, float aspect, float zNear, float zFar) noexcept;
        static float4x4 perspective(float fovY, float aspect, float zNear, float zFar) noexcept;
        static float4x4 orthographic_lh_zo(float width, float height, float zNear, float zFar) noexcept;
        static float4x4 orthographic_off_center_lh_zo(float left, float right, float bottom, float top, float zNear, float zFar) noexcept;
        static float4x4 orthographic(float width, float height, float zNear, float zFar) noexcept;

        static float4x4 look_at_lh(const float3& eye, const float3& target, const float3& up) noexcept;
        static float4x4 look_at_rh(const float3& eye, const float3& target, const float3& up) noexcept;
        static float4x4 look_at(const float3& eye, const float3& target, const float3& up) noexcept;

        float4& operator[](int rowIndex) noexcept;
        const float4& operator[](int rowIndex) const noexcept;
        float& operator()(int row, int col) noexcept;
        const float& operator()(int row, int col) const noexcept;

        float4 row0() const noexcept;
        float4 row1() const noexcept;
        float4 row2() const noexcept;
        float4 row3() const noexcept;
        void set_row0(const float4& r) noexcept;
        void set_row1(const float4& r) noexcept;
        void set_row2(const float4& r) noexcept;
        void set_row3(const float4& r) noexcept;
        float4 col0() const noexcept;
        float4 col1() const noexcept;
        float4 col2() const noexcept;
        float4 col3() const noexcept;

        float4x4& operator+=(const float4x4& rhs) noexcept;
        float4x4& operator-=(const float4x4& rhs) noexcept;
        float4x4& operator*=(float scalar) noexcept;
        float4x4& operator/=(float scalar) noexcept;
        float4x4& operator*=(const float4x4& rhs) noexcept;

        float4x4 operator+() const noexcept;
        float4x4 operator-() const noexcept;

        float4x4 transposed() const noexcept;
        float determinant() const noexcept;
        float4x4 inverted_affine() const noexcept;
        float4x4 inverted() const noexcept;
        float4x4 adjugate() const noexcept;
        float3x3 normal_matrix() const noexcept;
        float trace() const noexcept;
        float4 diagonal() const noexcept;
        float frobenius_norm() const noexcept;

        float4 transform_vector(const float4& vec) const noexcept;
        float3 transform_point(const float3& point) const noexcept;
        float3 transform_vector(const float3& vec) const noexcept;
        float3 transform_direction(const float3& dir) const noexcept;

        float3 get_translation() const noexcept;
        float3 get_scale() const noexcept;
        quaternion get_rotation() const noexcept;
        void set_translation(const float3& translation) noexcept;
        void set_scale(const float3& scale) noexcept;

        bool is_identity(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool is_affine(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool is_orthogonal(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool approximately(const float4x4& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool approximately_zero(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        std::string to_string() const;
        void to_column_major(float* data) const noexcept;
        void to_row_major(float* data) const noexcept;

        bool operator==(const float4x4& rhs) const noexcept;
        bool operator!=(const float4x4& rhs) const noexcept;
    };

    inline float4x4 operator+(const float4x4& lhs, const float4x4& rhs) noexcept;
    inline float4x4 operator-(const float4x4& lhs, const float4x4& rhs) noexcept;
    float4x4 operator*(const float4x4& lhs, const float4x4& rhs) noexcept;
    inline float4x4 operator*(const float4x4& mat, float scalar) noexcept;
    inline float4x4 operator*(float scalar, const float4x4& mat) noexcept;
    inline float4x4 operator/(const float4x4& mat, float scalar) noexcept;
    inline float4 operator*(const float4& vec, const float4x4& mat) noexcept;
    inline float3 operator*(const float3& point, const float4x4& mat) noexcept;

    inline float4x4 transpose(const float4x4& mat) noexcept;
    inline float4x4 inverse(const float4x4& mat) noexcept;
    inline float determinant(const float4x4& mat) noexcept;
    inline float4 mul(const float4& vec, const float4x4& mat) noexcept;
    inline float3 mul(const float3& point, const float4x4& mat) noexcept;
    inline float4x4 mul(const float4x4& lhs, const float4x4& rhs) noexcept;

    extern const float4x4 float4x4_Identity;
    extern const float4x4 float4x4_Zero;

}

#include "math_float4x4.inl"
