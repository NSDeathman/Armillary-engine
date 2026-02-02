/**
 * @file math_half4.h
 * @brief 4-dimensional half-precision vector class
 * @note Optimized for 4D graphics, homogeneous coordinates, RGBA colors with SSE optimization
 * @note Features comprehensive HLSL compatibility and color space operations
 */

#pragma once

 // Description: 4-dimensional half-precision vector class with 
 //              comprehensive mathematical operations, SSE optimization,
 //              and full HLSL compatibility
 // Author: NSDeathman, DeepSeek

#include <xmmintrin.h>
#include <pmmintrin.h> // SSE3
#include <smmintrin.h> // SSE4.1
#include <cmath>
#include <algorithm>
#include <string>
#include <cstdio>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"
#include "math_half.h"
#include "math_half2.h"
#include "math_half3.h"
#include "math_float4.h"

namespace Math
{
    /**
     * @class half4
     * @brief 4-dimensional half-precision vector with comprehensive mathematical operations
     *
     * Represents a 4D vector (x, y, z, w) using 16-bit half-precision floating point format.
     * Features SSE optimization for performance-critical operations and comprehensive
     * HLSL compatibility. Perfect for 4D graphics, homogeneous coordinates, RGBA colors,
     * and memory-constrained applications where full 32-bit precision is not required.
     *
     * @note Optimized for memory bandwidth and GPU data formats
     * @note Provides seamless interoperability with float4 and comprehensive mathematical operations
     * @note Includes advanced color operations, geometric functions, and homogeneous coordinate support
     */
    class half4
    {
    public:
        // ============================================================================
        // Data Members (Public for Direct Access)
        // ============================================================================

        half x; ///< X component of the vector
        half y; ///< Y component of the vector
        half z; ///< Z component of the vector
        half w; ///< W component of the vector (homogeneous coordinate or alpha)

        // ============================================================================
        // Constructors
        // ============================================================================

        half4() noexcept;
        half4(half x, half y, half z, half w) noexcept;
        explicit half4(half scalar) noexcept;
        half4(float x, float y, float z, float w) noexcept;
        explicit half4(float scalar) noexcept;
        half4(const half4&) noexcept = default;
        half4(const half2& vec, half z = half::from_bits(0), half w = half::from_bits(0)) noexcept;
        half4(const half3& vec, half w = half::from_bits(0)) noexcept;
        half4(const float4& vec) noexcept;
        half4(const float2& vec, float z = 0.0f, float w = 0.0f) noexcept;
        half4(const float3& vec, float w = 0.0f) noexcept;

        // ============================================================================
        // Assignment Operators
        // ============================================================================

        half4& operator=(const half4&) noexcept = default;
        half4& operator=(const float4& vec) noexcept;
        half4& operator=(const half3& xyz) noexcept;
        half4& operator=(half scalar) noexcept;
        half4& operator=(float scalar) noexcept;

        // ============================================================================
        // Compound Assignment Operators
        // ============================================================================

        half4& operator+=(const half4& rhs) noexcept;
        half4& operator-=(const half4& rhs) noexcept;
        half4& operator*=(const half4& rhs) noexcept;
        half4& operator/=(const half4& rhs) noexcept;
        half4& operator*=(half scalar) noexcept;
        half4& operator*=(float scalar) noexcept;
        half4& operator/=(half scalar) noexcept;
        half4& operator/=(float scalar) noexcept;

        // ============================================================================
        // Unary Operators
        // ============================================================================

        half4 operator+() const noexcept;
        half4 operator-() const noexcept;

        // ============================================================================
        // Access Operators
        // ============================================================================

        half& operator[](int index) noexcept;
        const half& operator[](int index) const noexcept;

        // ============================================================================
        // Conversion Operators
        // ============================================================================

        explicit operator float4() const noexcept;

        // ============================================================================
        // Static Constructors
        // ============================================================================

        static half4 zero() noexcept;
        static half4 one() noexcept;
        static half4 unit_x() noexcept;
        static half4 unit_y() noexcept;
        static half4 unit_z() noexcept;
        static half4 unit_w() noexcept;
        static half4 from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept;

        // ============================================================================
        // Mathematical Functions (SSE Optimized)
        // ============================================================================

        half length() const noexcept;
        half length_sq() const noexcept;
        half4 normalize() const noexcept;
        half dot(const half4& other) const noexcept;
        half dot3(const half4& other) const noexcept;
        half4 cross(const half4& other) const noexcept;
        half distance(const half4& other) const noexcept;
        half distance_sq(const half4& other) const noexcept;

        // ============================================================================
        // HLSL-like Functions
        // ============================================================================

        half4 abs() const noexcept;
        half4 sign() const noexcept;
        half4 floor() const noexcept;
        half4 ceil() const noexcept;
        half4 round() const noexcept;
        half4 frac() const noexcept;
        half4 saturate() const noexcept;
        half4 step(half edge) const noexcept;

        // ============================================================================
        // Color Operations
        // ============================================================================

        half luminance() const noexcept;
        half brightness() const noexcept;
        half4 premultiply_alpha() const noexcept;
        half4 unpremultiply_alpha() const noexcept;
        half4 grayscale() const noexcept;
        half4 srgb_to_linear() const noexcept;
        half4 linear_to_srgb() const noexcept;

        // ============================================================================
        // Geometric Operations
        // ============================================================================

        half3 project() const noexcept;
        half4 to_homogeneous() const noexcept;

        // ============================================================================
        // Static Mathematical Functions (SSE Optimized)
        // ============================================================================

        static half dot(const half4& a, const half4& b) noexcept;
        static half dot3(const half4& a, const half4& b) noexcept;
        static half4 cross(const half4& a, const half4& b) noexcept;
        static half4 lerp(const half4& a, const half4& b, half t) noexcept;
        static half4 lerp(const half4& a, const half4& b, float t) noexcept;
        static half4 min(const half4& a, const half4& b) noexcept;
        static half4 max(const half4& a, const half4& b) noexcept;
        static half4 saturate(const half4& vec) noexcept;

        // ============================================================================
        // Swizzle Operations (HLSL style)
        // ============================================================================

        half2 xy() const noexcept;
        half2 xz() const noexcept;
        half2 xw() const noexcept;
        half2 yz() const noexcept;
        half2 yw() const noexcept;
        half2 zw() const noexcept;

        half3 xyz() const noexcept;
        half3 xyw() const noexcept;
        half3 xzw() const noexcept;
        half3 yzw() const noexcept;

        half4 yxzw() const noexcept;
        half4 zxyw() const noexcept;
        half4 zyxw() const noexcept;
        half4 wzyx() const noexcept;

        // Color swizzles
        half r() const noexcept;
        half g() const noexcept;
        half b() const noexcept;
        half a() const noexcept;
        half2 rg() const noexcept;
        half2 rb() const noexcept;
        half2 ra() const noexcept;
        half2 gb() const noexcept;
        half2 ga() const noexcept;
        half2 ba() const noexcept;

        half3 rgb() const noexcept;
        half3 rga() const noexcept;
        half3 rba() const noexcept;
        half3 gba() const noexcept;

        half4 grba() const noexcept;
        half4 brga() const noexcept;
        half4 bgra() const noexcept;
        half4 abgr() const noexcept;

        // ============================================================================
        // Utility Methods
        // ============================================================================

        bool is_valid() const noexcept;
        bool approximately(const half4& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool approximately_zero(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        bool is_normalized(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;
        std::string to_string() const;
        const half* data() const noexcept;
        half* data() noexcept;
        void set_xyz(const half3& xyz) noexcept;
        void set_xy(const half2& xy) noexcept;
        void set_zw(const half2& zw) noexcept;

        // ============================================================================
        // Comparison Operators
        // ============================================================================

        bool operator==(const half4& rhs) const noexcept;
        bool operator!=(const half4& rhs) const noexcept;

        // ============================================================================
        // Special Value Checks
        // ============================================================================

        bool is_inf() const noexcept;
        bool is_all_inf() const noexcept;
        bool is_negative_inf() const noexcept;
        bool is_all_negative_inf() const noexcept;
        bool is_positive_inf() const noexcept;
        bool is_all_positive_inf() const noexcept;
        bool is_negative() const noexcept;
        bool is_all_negative() const noexcept;
        bool is_positive() const noexcept;
        bool is_all_positive() const noexcept;
        bool is_nan() const noexcept;
        bool is_all_nan() const noexcept;
        bool is_finite() const noexcept;
        bool is_all_finite() const noexcept;
        bool is_zero() const noexcept;
        bool is_all_zero() const noexcept;
        bool is_positive_zero() const noexcept;
        bool is_all_positive_zero() const noexcept;
        bool is_negative_zero() const noexcept;
        bool is_all_negative_zero() const noexcept;
        bool is_normal() const noexcept;
        bool is_all_normal() const noexcept;
    };

    // ============================================================================
    // Binary Operators
    // ============================================================================

    half4 operator+(half4 lhs, const half4& rhs) noexcept;
    half4 operator-(half4 lhs, const half4& rhs) noexcept;
    half4 operator*(half4 lhs, const half4& rhs) noexcept;
    half4 operator/(half4 lhs, const half4& rhs) noexcept;

    half4 operator*(half4 vec, half scalar) noexcept;
    half4 operator*(half scalar, half4 vec) noexcept;
    half4 operator/(half4 vec, half scalar) noexcept;

    half4 operator*(half4 vec, float scalar) noexcept;
    half4 operator*(float scalar, half4 vec) noexcept;
    half4 operator/(half4 vec, float scalar) noexcept;

    // ============================================================================
    // Mixed Type Operators (half4 <-> float4)
    // ============================================================================

    half4 operator+(const half4& lhs, const float4& rhs) noexcept;
    half4 operator-(const half4& lhs, const float4& rhs) noexcept;
    half4 operator*(const half4& lhs, const float4& rhs) noexcept;
    half4 operator/(const half4& lhs, const float4& rhs) noexcept;

    half4 operator+(const float4& lhs, const half4& rhs) noexcept;
    half4 operator-(const float4& lhs, const half4& rhs) noexcept;
    half4 operator*(const float4& lhs, const half4& rhs) noexcept;
    half4 operator/(const float4& lhs, const half4& rhs) noexcept;

    // ============================================================================
    // Global Mathematical Functions
    // ============================================================================

    half distance(const half4& a, const half4& b) noexcept;
    half distance_sq(const half4& a, const half4& b) noexcept;
    half dot(const half4& a, const half4& b) noexcept;
    half dot3(const half4& a, const half4& b) noexcept;
    half4 cross(const half4& a, const half4& b) noexcept;
    half4 normalize(const half4& vec) noexcept;
    half4 lerp(const half4& a, const half4& b, half t) noexcept;
    half4 lerp(const half4& a, const half4& b, float t) noexcept;
    half4 saturate(const half4& vec) noexcept;
    bool approximately(const half4& a, const half4& b, float epsilon = Constants::Constants<float>::Epsilon) noexcept;
    bool is_valid(const half4& vec) noexcept;
    bool is_normalized(const half4& vec, float epsilon = Constants::Constants<float>::Epsilon) noexcept;

    // ============================================================================
    // HLSL-like Global Functions
    // ============================================================================

    half4 abs(const half4& vec) noexcept;
    half4 sign(const half4& vec) noexcept;
    half4 floor(const half4& vec) noexcept;
    half4 ceil(const half4& vec) noexcept;
    half4 round(const half4& vec) noexcept;
    half4 frac(const half4& vec) noexcept;
    half4 step(half edge, const half4& vec) noexcept;
    half4 min(const half4& a, const half4& b) noexcept;
    half4 max(const half4& a, const half4& b) noexcept;
    half4 clamp(const half4& vec, const half4& min_val, const half4& max_val) noexcept;
    half4 clamp(const half4& vec, float min_val, float max_val) noexcept;
    half4 smoothstep(half edge0, half edge1, const half4& vec) noexcept;

    // ============================================================================
    // Color Operations
    // ============================================================================

    half4 rgb_to_grayscale(const half4& rgb) noexcept;
    half luminance(const half4& rgb) noexcept;
    half brightness(const half4& rgb) noexcept;
    half4 premultiply_alpha(const half4& color) noexcept;
    half4 unpremultiply_alpha(const half4& color) noexcept;
    half4 srgb_to_linear(const half4& srgb) noexcept;
    half4 linear_to_srgb(const half4& linear) noexcept;

    // ============================================================================
    // Geometric Operations
    // ============================================================================

    half3 project(const half4& vec) noexcept;
    half4 to_homogeneous(const half4& vec) noexcept;

    // ============================================================================
    // Type Conversion Functions
    // ============================================================================

    float4 to_float4(const half4& vec) noexcept;
    half4 to_half4(const float4& vec) noexcept;

    // ============================================================================
    // Useful Constants
    // ============================================================================

    extern const half4 half4_Zero;
    extern const half4 half4_One;
    extern const half4 half4_UnitX;
    extern const half4 half4_UnitY;
    extern const half4 half4_UnitZ;
    extern const half4 half4_UnitW;
    extern const half4 half4_Red;
    extern const half4 half4_Green;
    extern const half4 half4_Blue;
    extern const half4 half4_White;
    extern const half4 half4_Black;
    extern const half4 half4_Transparent;
    extern const half4 half4_Yellow;
    extern const half4 half4_Cyan;
    extern const half4 half4_Magenta;

} // namespace Math

// Include inline implementation
#include "math_half4.inl"
