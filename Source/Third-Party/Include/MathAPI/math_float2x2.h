#pragma once

#include <cmath>
#include <string>
#include <cstdio>
#include <xmmintrin.h>
#include <pmmintrin.h>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"
#include "math_float2.h"

namespace Math
{
    class float2x2
    {
    private:
        alignas(16) float2 row0_;
        alignas(16) float2 row1_;

    public:
        float2x2() noexcept;
        float2x2(const float2& row0, const float2& row1) noexcept;
        float2x2(float m00, float m01, float m10, float m11) noexcept;
        explicit float2x2(const float* data) noexcept;
        explicit float2x2(float scalar) noexcept;
        explicit float2x2(const float2& diagonal) noexcept;
        explicit float2x2(__m128 sse_data) noexcept;
        float2x2(const float2x2&) noexcept = default;

        float2x2& operator=(const float2x2&) noexcept = default;

        static float2x2 identity() noexcept;
        static float2x2 zero() noexcept;
        static float2x2 rotation(float angle) noexcept;
        static float2x2 scaling(const float2& scale) noexcept;
        static float2x2 scaling(float x, float y) noexcept;
        static float2x2 scaling(float uniformScale) noexcept;
        static float2x2 shear(const float2& shear) noexcept;
        static float2x2 shear(float x, float y) noexcept;

        float2& operator[](int rowIndex) noexcept;
        const float2& operator[](int rowIndex) const noexcept;
        float& operator()(int row, int col) noexcept;
        const float& operator()(int row, int col) const noexcept;

        float2 row0() const noexcept;
        float2 row1() const noexcept;
        float2 col0() const noexcept;
        float2 col1() const noexcept;
        void set_row0(const float2& row) noexcept;
        void set_row1(const float2& row) noexcept;
        void set_col0(const float2& col) noexcept;
        void set_col1(const float2& col) noexcept;

        __m128 sse_data() const noexcept;
        void set_sse_data(__m128 sse_data) noexcept;

        float2x2& operator+=(const float2x2& rhs) noexcept;
        float2x2& operator-=(const float2x2& rhs) noexcept;
        float2x2& operator*=(float scalar) noexcept;
        float2x2& operator/=(float scalar) noexcept;
        float2x2& operator*=(const float2x2& rhs) noexcept;

        float2x2 operator+() const noexcept;
        float2x2 operator-() const noexcept;

        float2x2 transposed() const noexcept;
        float determinant() const noexcept;
        float2x2 inverted() const noexcept;
        float2x2 adjugate() const noexcept;
        float trace() const noexcept;
        float2 diagonal() const noexcept;
        float frobenius_norm() const noexcept;

        float2 transform_vector(const float2& vec) const noexcept;
        float2 transform_point(const float2& point) const noexcept;

        float get_rotation() const noexcept;
        float2 get_scale() const noexcept;
        void set_rotation(float angle) noexcept;
        void set_scale(const float2& scale) noexcept;

        bool is_identity(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool is_orthogonal(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool is_rotation(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool approximately(const float2x2& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool approximately_zero(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        std::string to_string() const;
        void to_row_major(float* data) const noexcept;
        void to_column_major(float* data) const noexcept;

        bool operator==(const float2x2& rhs) const noexcept;
        bool operator!=(const float2x2& rhs) const noexcept;
    };

    float2x2 operator+(float2x2 lhs, const float2x2& rhs) noexcept;
    float2x2 operator-(float2x2 lhs, const float2x2& rhs) noexcept;
    float2x2 operator*(const float2x2& lhs, const float2x2& rhs) noexcept;
    float2x2 operator*(float2x2 mat, float scalar) noexcept;
    float2x2 operator*(float scalar, float2x2 mat) noexcept;
    float2x2 operator/(float2x2 mat, float scalar) noexcept;
    float2 operator*(const float2& vec, const float2x2& mat) noexcept;

    float2x2 transpose(const float2x2& mat) noexcept;
    float2x2 inverse(const float2x2& mat) noexcept;
    float determinant(const float2x2& mat) noexcept;
    float2 mul(const float2& vec, const float2x2& mat) noexcept;
    float2x2 mul(const float2x2& lhs, const float2x2& rhs) noexcept;
    float trace(const float2x2& mat) noexcept;
    float2 diagonal(const float2x2& mat) noexcept;
    float frobenius_norm(const float2x2& mat) noexcept;

    bool approximately(const float2x2& a, const float2x2& b,
        float epsilon = Constants::Constants<float>::Epsilon) noexcept;
    bool is_orthogonal(const float2x2& mat,
        float epsilon = Constants::Constants<float>::Epsilon) noexcept;
    bool is_rotation(const float2x2& mat,
        float epsilon = Constants::Constants<float>::Epsilon) noexcept;

    extern const float2x2 float2x2_Identity;
    extern const float2x2 float2x2_Zero;

}

#include "math_float2x2.inl"
