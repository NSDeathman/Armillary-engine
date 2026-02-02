#pragma once

#include <cmath>
#include <string>
#include <cstdio>
#include <algorithm>
#include <xmmintrin.h>
#include <pmmintrin.h>
#include <smmintrin.h>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"
#include "math_fast_functions.h"
#include "math_float3.h"
#include "math_float4.h"
#include "math_float3x3.h"
#include "math_float4x4.h"

namespace Math
{
    class quaternion;

    quaternion operator+(quaternion lhs, const quaternion& rhs) noexcept;
    quaternion operator-(quaternion lhs, const quaternion& rhs) noexcept;
    quaternion operator*(const quaternion& lhs, const quaternion& rhs) noexcept;
    quaternion operator*(quaternion q, float scalar) noexcept;
    quaternion operator*(float scalar, quaternion q) noexcept;
    quaternion operator/(quaternion q, float scalar) noexcept;
    float3 operator*(const quaternion& q, const float3& vec) noexcept;

    quaternion nlerp(const quaternion& a, const quaternion& b, float t) noexcept;
    quaternion slerp(const quaternion& a, const quaternion& b, float t) noexcept;
    quaternion lerp(const quaternion& a, const quaternion& b, float t) noexcept;

    class quaternion
    {
    public:
        union {
            struct {
                float x;
                float y;
                float z;
                float w;
            };
            float4 data_;
            __m128 simd_;
        };

    public:
        quaternion() noexcept : data_(0.0f, 0.0f, 0.0f, 1.0f) {}
        quaternion(float x, float y, float z, float w) noexcept : data_(x, y, z, w) {}
        explicit quaternion(const float4& vec) noexcept : data_(vec) {}
        explicit quaternion(__m128 simd_val) noexcept : simd_(simd_val) {}

        quaternion(const float3& axis, float angle) noexcept
        {
            float3 normalized_axis = axis.normalize();
            float half_angle = angle * 0.5f;
            float sin_half = std::sin(half_angle);
            float cos_half = std::cos(half_angle);

            data_ = float4(normalized_axis * sin_half, cos_half);
        }

        // Euler angles constructor (YXZ order / Yaw-Pitch-Roll)
        quaternion(float yaw, float pitch, float roll) noexcept
        {
            float half_yaw = yaw * 0.5f;
            float half_pitch = pitch * 0.5f;
            float half_roll = roll * 0.5f;

            float cy = std::cos(half_yaw);
            float sy = std::sin(half_yaw);
            float cp = std::cos(half_pitch);
            float sp = std::sin(half_pitch);
            float cr = std::cos(half_roll);
            float sr = std::sin(half_roll);

            w = cy * cp * cr + sy * sp * sr;
            x = cy * sp * cr + sy * cp * sr;
            y = sy * cp * cr - cy * sp * sr;
            z = cy * cp * sr - sy * sp * cr;
        }

        explicit quaternion(const float3x3& matrix) noexcept
        {
            *this = from_matrix(matrix);
        }

        explicit quaternion(const float4x4& matrix) noexcept
        {
            *this = from_matrix(matrix);
        }

        quaternion(const quaternion&) noexcept = default;
        quaternion& operator=(const quaternion&) noexcept = default;
        quaternion& operator=(const float4& vec) noexcept
        {
            data_ = vec;
            return *this;
        }

        static quaternion identity() noexcept { return quaternion(0.0f, 0.0f, 0.0f, 1.0f); }
        static quaternion zero() noexcept { return quaternion(0.0f, 0.0f, 0.0f, 0.0f); }
        static quaternion one() noexcept { return quaternion(1.0f, 1.0f, 1.0f, 1.0f); }

        bool approximately_zero(float epsilon = Constants::Constants<float>::Epsilon) const noexcept
        {
            return data_.approximately_zero(epsilon);
        }

        bool approximately(const quaternion& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept
        {
            if (data_.approximately(other.data_, epsilon)) return true;
            float dot_val = std::abs(dot(other));
            return (1.0f - dot_val) < epsilon;
        }

        bool approximately_equal(const quaternion& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept
        {
            return approximately(other, epsilon);
        }

        static quaternion from_axis_angle(const float3& axis, float angle) noexcept
        {
            return quaternion(axis, angle);
        }

        static quaternion from_euler(float yaw, float pitch, float roll) noexcept
        {
            return quaternion(yaw, pitch, roll);
        }

        static quaternion from_euler(const float3& euler_angles) noexcept
        {
            return quaternion(euler_angles.x, euler_angles.y, euler_angles.z);
        }

        static quaternion from_matrix(const float3x3& m) noexcept
        {
            // Consistent Left-Handed Conversion
            // Matches float4x4::rotation_y definition where (0,2) is -sin

            float trace = m(0, 0) + m(1, 1) + m(2, 2);
            quaternion q;

            if (trace > 0.0f) {
                float s = 0.5f / std::sqrt(trace + 1.0f);
                q.w = 0.25f / s;
                // Use (Lower - Upper) for all components to match LH matrix def
                q.x = (m(2, 1) - m(1, 2)) * s;
                q.y = (m(0, 2) - m(2, 0)) * s; // In standard RH this is swapped
                q.z = (m(1, 0) - m(0, 1)) * s;
            }
            else {
                if (m(0, 0) > m(1, 1) && m(0, 0) > m(2, 2)) {
                    float s = 2.0f * std::sqrt(1.0f + m(0, 0) - m(1, 1) - m(2, 2));
                    q.w = (m(2, 1) - m(1, 2)) / s;
                    q.x = 0.25f * s;
                    q.y = (m(0, 1) + m(1, 0)) / s;
                    q.z = (m(0, 2) + m(2, 0)) / s;
                }
                else if (m(1, 1) > m(2, 2)) {
                    float s = 2.0f * std::sqrt(1.0f + m(1, 1) - m(0, 0) - m(2, 2));
                    q.w = (m(0, 2) - m(2, 0)) / s; // Swapped to match trace logic
                    q.x = (m(0, 1) + m(1, 0)) / s;
                    q.y = 0.25f * s;
                    q.z = (m(1, 2) + m(2, 1)) / s;
                }
                else {
                    float s = 2.0f * std::sqrt(1.0f + m(2, 2) - m(0, 0) - m(1, 1));
                    q.w = (m(1, 0) - m(0, 1)) / s;
                    q.x = (m(0, 2) + m(2, 0)) / s;
                    q.y = (m(1, 2) + m(2, 1)) / s;
                    q.z = 0.25f * s;
                }
            }
            return q.normalize();
        }

        static quaternion from_matrix(const float4x4& matrix) noexcept
        {
            float3x3 rot_matrix(
                matrix.row0().xyz(),
                matrix.row1().xyz(),
                matrix.row2().xyz()
            );
            return from_matrix(rot_matrix);
        }

        static quaternion from_to_rotation(const float3& from, const float3& to) noexcept
        {
            float3 v0 = from.normalize();
            float3 v1 = to.normalize();

            if (v0.approximately_zero() || v1.approximately_zero()) return identity();

            float cos_angle = float3::dot(v0, v1);

            if (cos_angle > 0.9999f) return identity();

            if (cos_angle < -0.9999f) {
                float3 axis = cross(float3::unit_x(), v0);
                if (axis.length_sq() < 0.0001f) axis = cross(float3::unit_y(), v0);
                axis = axis.normalize();
                return quaternion(axis, Constants::PI);
            }

            float3 axis = cross(v0, v1);
            float angle = std::acos(cos_angle);
            return quaternion(axis.normalize(), angle);
        }

        static quaternion look_rotation(const float3& forward, const float3& up) noexcept
        {
            float3 f = forward.normalize();
            if (f.length_sq() < 1e-6f) return identity();

            float3 u = up.normalize();

            if (std::abs(float3::dot(f, u)) > 0.9999f) {
                u = float3::unit_y();
                if (std::abs(float3::dot(f, u)) > 0.9999f) u = float3::unit_z();
            }

            // Left-Handed: Right = Up x Forward
            float3 r = cross(u, f).normalize();
            u = cross(f, r).normalize();

            // Construct Basis Matrix (Rows are Axis vectors)
            float3x3 rot_mat(r, u, f);

            // from_matrix expects a Transform Matrix (Columns are Axis vectors), so we transpose
            return from_matrix(rot_mat.transposed());
        }

        static quaternion rotation_x(float angle) noexcept {
            float half_angle = angle * 0.5f;
            return quaternion(std::sin(half_angle), 0.0f, 0.0f, std::cos(half_angle));
        }

        static quaternion rotation_y(float angle) noexcept {
            float half_angle = angle * 0.5f;
            return quaternion(0.0f, std::sin(half_angle), 0.0f, std::cos(half_angle));
        }

        static quaternion rotation_z(float angle) noexcept {
            float half_angle = angle * 0.5f;
            return quaternion(0.0f, 0.0f, std::sin(half_angle), std::cos(half_angle));
        }

        static quaternion slerp(const quaternion& a, const quaternion& b, float t) noexcept { return Math::slerp(a, b, t); }
        static quaternion lerp(const quaternion& a, const quaternion& b, float t) noexcept { return Math::lerp(a, b, t); }

        quaternion& operator+=(const quaternion& rhs) noexcept { data_ += rhs.data_; return *this; }
        quaternion& operator-=(const quaternion& rhs) noexcept { data_ -= rhs.data_; return *this; }
        quaternion& operator*=(float scalar) noexcept { data_ *= scalar; return *this; }
        quaternion& operator/=(float scalar) noexcept { data_ /= scalar; return *this; }
        quaternion& operator*=(const quaternion& rhs) noexcept { *this = *this * rhs; return *this; }

        quaternion operator+() const noexcept { return *this; }
        quaternion operator-() const noexcept { return quaternion(-data_); }
        operator float4() const noexcept { return data_; }
        __m128 get_simd() const noexcept { return simd_; }
        void set_simd(__m128 new_simd) noexcept { simd_ = new_simd; }

        float length() const noexcept { return data_.length(); }
        float length_sq() const noexcept { return data_.length_sq(); }

        quaternion normalize() const noexcept {
            float len_sq = length_sq();
            if (len_sq > Constants::Constants<float>::Epsilon && std::isfinite(len_sq)) {
                return quaternion(data_ * (1.0f / std::sqrt(len_sq)));
            }
            return identity();
        }

        quaternion conjugate() const noexcept {
            static const __m128 SIGN_MASK = _mm_set_ps(1.0f, -1.0f, -1.0f, -1.0f);
            return quaternion(_mm_mul_ps(simd_, SIGN_MASK));
        }

        quaternion inverse() const noexcept {
            float len_sq = length_sq();
            return (len_sq > Constants::Constants<float>::Epsilon) ? (conjugate() / len_sq) : identity();
        }

        float dot(const quaternion& other) const noexcept { return float4::dot(data_, other.data_); }

        float3x3 to_matrix3x3() const noexcept
        {
            quaternion n = normalize();
            float xx = n.x * n.x, yy = n.y * n.y, zz = n.z * n.z;
            float xy = n.x * n.y, xz = n.x * n.z, yz = n.y * n.z;
            float wx = n.w * n.x, wy = n.w * n.y, wz = n.w * n.z;

            // LH Rotation Matrix to match float4x4 definitions
            // Upper triangle W terms subtracted, Lower triangle W terms added
            return float3x3(
                float3(1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy)),
                float3(2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx)),
                float3(2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy))
            );
        }

        float4x4 to_matrix4x4() const noexcept
        {
            float3x3 rot = to_matrix3x3();
            return float4x4(
                float4(rot.row0(), 0.0f),
                float4(rot.row1(), 0.0f),
                float4(rot.row2(), 0.0f),
                float4(0.0f, 0.0f, 0.0f, 1.0f)
            );
        }

        void to_axis_angle(float3& axis, float& angle) const noexcept
        {
            quaternion normalized = normalize();
            angle = 2.0f * std::acos(std::clamp(normalized.w, -1.0f, 1.0f));
            float s = std::sqrt(1.0f - normalized.w * normalized.w);
            axis = (s > Constants::Constants<float>::Epsilon) ?
                float3(normalized.x / s, normalized.y / s, normalized.z / s).normalize() :
                float3::unit_x();
        }

        float3 to_euler() const noexcept
        {
            quaternion q = normalize();
            float3 euler;

            // YXZ Order (Yaw-Pitch-Roll)
            float sinp = 2.0f * (q.w * q.x - q.y * q.z);
            if (std::abs(sinp) >= 1.0f) {
                euler.y = std::copysign(Constants::HALF_PI, sinp);
                euler.x = 2.0f * std::atan2(q.y, q.w);
                euler.z = 0.0f;
            }
            else {
                euler.y = std::asin(sinp);
                euler.x = std::atan2(2.0f * (q.w * q.y + q.x * q.z), 1.0f - 2.0f * (q.x * q.x + q.y * q.y));
                euler.z = std::atan2(2.0f * (q.w * q.z + q.x * q.y), 1.0f - 2.0f * (q.x * q.x + q.z * q.z));
            }
            return euler;
        }

        float3 transform_vector(const float3& vec) const noexcept
        {
            quaternion q = normalize();
            float3 q_xyz(q.x, q.y, q.z);
            float3 t = cross(q_xyz, vec) * 2.0f;
            return vec + q.w * t + cross(q_xyz, t);
        }

        float3 transform_direction(const float3& dir) const noexcept { return transform_vector(dir).normalize(); }

        bool is_identity(float epsilon = Constants::Constants<float>::Epsilon) const noexcept {
            return (std::abs(x) < epsilon) && (std::abs(y) < epsilon) && (std::abs(z) < epsilon) && (std::abs(w - 1.0f) < epsilon);
        }

        bool is_normalized(float epsilon = Constants::Constants<float>::Epsilon) const noexcept {
            float len_sq = length_sq();
            return std::isfinite(len_sq) && MathFunctions::approximately(len_sq, 1.0f, epsilon);
        }

        bool is_valid() const noexcept { return data_.isValid(); }

        std::string to_string() const {
            char buffer[256];
            std::snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f, %.3f)", x, y, z, w);
            return std::string(buffer);
        }

        const float* data() const noexcept { return &x; }
        float* data() noexcept { return &x; }
        const float4& get_float4() const noexcept { return data_; }
        float4& get_float4() noexcept { return data_; }

        bool operator==(const quaternion& rhs) const noexcept { return data_ == rhs.data_; }
        bool operator!=(const quaternion& rhs) const noexcept { return data_ != rhs.data_; }

        quaternion fast_normalize() const noexcept {
            float len_sq = length_sq();
            if (len_sq > 1e-12f && std::isfinite(len_sq)) {
                float inv_len = FastMath::fast_inv_sqrt(len_sq);
                inv_len = inv_len * (1.5f - 0.5f * len_sq * inv_len * inv_len);
                return quaternion(data_ * inv_len);
            }
            return identity();
        }
    };

    inline quaternion operator+(quaternion lhs, const quaternion& rhs) noexcept { return lhs += rhs; }
    inline quaternion operator-(quaternion lhs, const quaternion& rhs) noexcept { return lhs -= rhs; }

    inline quaternion operator*(const quaternion& lhs, const quaternion& rhs) noexcept {
        return quaternion(
            lhs.w * rhs.x + lhs.x * rhs.w + lhs.y * rhs.z - lhs.z * rhs.y,
            lhs.w * rhs.y - lhs.x * rhs.z + lhs.y * rhs.w + lhs.z * rhs.x,
            lhs.w * rhs.z + lhs.x * rhs.y - lhs.y * rhs.x + lhs.z * rhs.w,
            lhs.w * rhs.w - lhs.x * rhs.x - lhs.y * rhs.y - lhs.z * rhs.z
        );
    }

    inline quaternion operator*(quaternion q, float scalar) noexcept { return q *= scalar; }
    inline quaternion operator*(float scalar, quaternion q) noexcept { return q *= scalar; }
    inline quaternion operator/(quaternion q, float scalar) noexcept { return q /= scalar; }
    inline float3 operator*(const quaternion& q, const float3& vec) noexcept { return q.transform_vector(vec); }

    inline float length(const quaternion& q) noexcept { return q.length(); }
    inline float length_sq(const quaternion& q) noexcept { return q.length_sq(); }
    inline quaternion normalize(const quaternion& q) noexcept { return q.normalize(); }
    inline quaternion conjugate(const quaternion& q) noexcept { return q.conjugate(); }
    inline quaternion inverse(const quaternion& q) noexcept { return q.inverse(); }
    inline float dot(const quaternion& a, const quaternion& b) noexcept { return a.dot(b); }

    inline quaternion nlerp(const quaternion& a, const quaternion& b, float t) noexcept {
        float cos_angle = dot(a, b);
        float sign = (cos_angle < 0.0f) ? -1.0f : 1.0f;
        __m128 a_simd = a.get_simd();
        __m128 b_simd = _mm_mul_ps(b.get_simd(), _mm_set1_ps(sign));
        __m128 t_vec = _mm_set1_ps(t);
        __m128 res = _mm_add_ps(_mm_mul_ps(a_simd, _mm_set1_ps(1.0f - t)), _mm_mul_ps(b_simd, t_vec));
        return quaternion(res).normalize();
    }

    inline quaternion slerp(const quaternion& a, const quaternion& b, float t) noexcept {
        if (t <= 0.0f) return a.normalize();
        if (t >= 1.0f) return b.normalize();
        float cos_angle = dot(a, b);
        quaternion b_target = b;
        if (cos_angle < 0.0f) { b_target = -b; cos_angle = -cos_angle; }
        if (cos_angle > 0.9995f) return nlerp(a, b_target, t);

        float angle = std::acos(std::clamp(cos_angle, -1.0f, 1.0f));
        float sin_angle = std::sin(angle);
        if (std::abs(sin_angle) < 1e-6f) return nlerp(a, b_target, t);

        float inv_sin = 1.0f / sin_angle;
        float r_a = std::sin((1.0f - t) * angle) * inv_sin;
        float r_b = std::sin(t * angle) * inv_sin;
        return (a * r_a + b_target * r_b).normalize();
    }

    inline quaternion lerp(const quaternion& a, const quaternion& b, float t) noexcept { return nlerp(a, b, t); }
    inline bool approximately(const quaternion& a, const quaternion& b, float epsilon = Constants::Constants<float>::Epsilon) noexcept { return a.approximately(b, epsilon); }
    inline bool is_valid(const quaternion& q) noexcept { return q.is_valid(); }
    inline bool is_normalized(const quaternion& q, float epsilon = Constants::Constants<float>::Epsilon) noexcept { return q.is_normalized(epsilon); }

    inline const quaternion quaternion_Identity = quaternion::identity();
    inline const quaternion quaternion_Zero = quaternion::zero();
    inline const quaternion quaternion_One = quaternion::one();
}
