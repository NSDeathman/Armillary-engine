// Description: 3-dimensional half-precision vector class with 
//              comprehensive mathematical operations, SSE optimization,
//              and HLSL compatibility
// Author: NSDeathman, DeepSeek
#pragma once

/**
 * @file math_half3.h
 * @brief 3-dimensional half-precision vector class
 * @note Optimized for 3D graphics, normals, colors, and memory-constrained applications
 * @note Features SSE optimization and comprehensive HLSL compatibility
 */

#include <cmath>
#include <algorithm>
#include <string>
#include <cstdio>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"
#include "math_half.h"
#include "math_half2.h"
#include "math_float3.h"

#include <xmmintrin.h>
#include <pmmintrin.h>

namespace Math
{
    /**
     * @class half3
     * @brief 3-dimensional half-precision vector with comprehensive mathematical operations
     *
     * Represents a 3D vector (x, y, z) using 16-bit half-precision floating point format.
     * Features SSE optimization for performance-critical operations and comprehensive
     * HLSL compatibility. Perfect for 3D graphics, normals, colors, and memory-constrained
     * applications where full 32-bit precision is not required.
     *
     * @note Optimized for memory bandwidth and GPU data formats
     * @note Provides seamless interoperability with float3 and comprehensive mathematical operations
     * @note Includes advanced color operations and geometric functions
     */
    class half3
    {
    public:
        // ============================================================================
        // Data Members (Public for Direct Access)
        // ============================================================================

        half x; ///< X component of the vector
        half y; ///< Y component of the vector
        half z; ///< Z component of the vector

        // ============================================================================
        // Constructors
        // ============================================================================

        /**
         * @brief Default constructor (initializes to zero vector)
         */
        half3() noexcept;

        /**
         * @brief Construct from half components
         * @param x X component
         * @param y Y component
         * @param z Z component
         */
        half3(half x, half y, half z) noexcept;

        /**
         * @brief Construct from scalar (all components set to same value)
         * @param scalar Value for all components
         */
        explicit half3(half scalar) noexcept;

        /**
         * @brief Construct from float components
         * @param x X component as float
         * @param y Y component as float
         * @param z Z component as float
         */
        half3(float x, float y, float z) noexcept;

        /**
         * @brief Construct from float scalar (all components set to same value)
         * @param scalar Value for all components as float
         */
        explicit half3(float scalar) noexcept;

        /**
         * @brief Copy constructor
         */
        half3(const half3&) noexcept = default;

        /**
         * @brief Construct from half2 and z component
         * @param vec 2D vector for x and y components
         * @param z Z component
         */
        half3(const half2& vec, half z = half::from_bits(0)) noexcept;

        /**
         * @brief Construct from float3 (converts components to half precision)
         * @param vec 32-bit floating point vector
         */
        half3(const float3& vec) noexcept;

        /**
         * @brief Construct from float2 and z component
         * @param vec 2D vector for x and y components
         * @param z Z component as float
         */
        half3(const float2& vec, float z = 0.0f) noexcept;

        // ============================================================================
        // Assignment Operators
        // ============================================================================

        /**
         * @brief Copy assignment operator
         */
        half3& operator=(const half3&) noexcept = default;

        /**
         * @brief Assignment from float3 (converts components to half precision)
         * @param vec 32-bit floating point vector
         */
        half3& operator=(const float3& vec) noexcept;

        /**
         * @brief Assignment from half scalar (sets all components to same value)
         * @param scalar Value for all components
         */
        half3& operator=(half scalar) noexcept;

        /**
         * @brief Assignment from float scalar (sets all components to same value)
         * @param scalar Value for all components as float
         */
        half3& operator=(float scalar) noexcept;

        // ============================================================================
        // Compound Assignment Operators
        // ============================================================================

        /**
         * @brief Compound addition assignment
         * @param rhs Vector to add
         * @return Reference to this object
         */
        half3& operator+=(const half3& rhs) noexcept;

        /**
         * @brief Compound subtraction assignment
         * @param rhs Vector to subtract
         * @return Reference to this object
         */
        half3& operator-=(const half3& rhs) noexcept;

        /**
         * @brief Compound multiplication assignment
         * @param rhs Vector to multiply by
         * @return Reference to this object
         */
        half3& operator*=(const half3& rhs) noexcept;

        /**
         * @brief Compound division assignment
         * @param rhs Vector to divide by
         * @return Reference to this object
         */
        half3& operator/=(const half3& rhs) noexcept;

        /**
         * @brief Compound scalar multiplication assignment (half)
         * @param scalar Scalar to multiply by
         * @return Reference to this object
         */
        half3& operator*=(half scalar) noexcept;

        /**
         * @brief Compound scalar multiplication assignment (float)
         * @param scalar Scalar to multiply by
         * @return Reference to this object
         */
        half3& operator*=(float scalar) noexcept;

        /**
         * @brief Compound scalar division assignment (half)
         * @param scalar Scalar to divide by
         * @return Reference to this object
         */
        half3& operator/=(half scalar) noexcept;

        /**
         * @brief Compound scalar division assignment (float)
         * @param scalar Scalar to divide by
         * @return Reference to this object
         */
        half3& operator/=(float scalar) noexcept;

        // ============================================================================
        // Unary Operators
        // ============================================================================

        /**
         * @brief Unary plus operator
         * @return Positive vector
         */
        half3 operator+() const noexcept;

        /**
         * @brief Unary minus operator
         * @return Negated vector
         */
        half3 operator-() const noexcept;

        // ============================================================================
        // Access Operators
        // ============================================================================

        /**
         * @brief Access component by index
         * @param index Component index (0 = x, 1 = y, 2 = z)
         * @return Reference to component
         */
        half& operator[](int index) noexcept;

        /**
         * @brief Access component by index (const)
         * @param index Component index (0 = x, 1 = y, 2 = z)
         * @return Const reference to component
         */
        const half& operator[](int index) const noexcept;

        // ============================================================================
        // Conversion Operators
        // ============================================================================

        /**
         * @brief Convert to float3 (promotes components to full precision)
         * @return 32-bit floating point vector
         */
        explicit operator float3() const noexcept;

        // ============================================================================
        // Static Constructors
        // ============================================================================

        /**
         * @brief Zero vector (0, 0, 0)
         * @return Zero vector
         */
        static half3 zero() noexcept;

        /**
         * @brief One vector (1, 1, 1)
         * @return One vector
         */
        static half3 one() noexcept;

        /**
         * @brief Unit X vector (1, 0, 0)
         * @return Unit X vector
         */
        static half3 unit_x() noexcept;

        /**
         * @brief Unit Y vector (0, 1, 0)
         * @return Unit Y vector
         */
        static half3 unit_y() noexcept;

        /**
         * @brief Unit Z vector (0, 0, 1)
         * @return Unit Z vector
         */
        static half3 unit_z() noexcept;

        /**
         * @brief Forward vector (0, 0, 1) - common in 3D graphics
         * @return Forward vector
         */
        static half3 forward() noexcept;

        /**
         * @brief Up vector (0, 1, 0) - common in 3D graphics
         * @return Up vector
         */
        static half3 up() noexcept;

        /**
         * @brief Right vector (1, 0, 0) - common in 3D graphics
         * @return Right vector
         */
        static half3 right() noexcept;

        // ============================================================================
        // Mathematical Functions
        // ============================================================================

        /**
         * @brief Compute Euclidean length (magnitude)
         * @return Length of the vector
         */
        half length() const noexcept;

        /**
         * @brief Compute squared length (faster, useful for comparisons)
         * @return Squared length of the vector
         */
        half length_sq() const noexcept;

        /**
         * @brief Normalize vector to unit length
         * @return Normalized vector
         * @note Returns zero vector if length is zero
         */
        half3 normalize() const noexcept;

        /**
         * @brief Compute dot product with another vector
         * @param other Other vector
         * @return Dot product result
         */
        half dot(const half3& other) const noexcept;

        /**
         * @brief Compute cross product with another vector
         * @param other Other vector
         * @return Cross product result
         */
        half3 cross(const half3& other) const noexcept;

        /**
         * @brief Compute distance to another point
         * @param other Other point
         * @return Euclidean distance
         */
        half distance(const half3& other) const noexcept;

        /**
         * @brief Compute squared distance to another point (faster)
         * @param other Other point
         * @return Squared Euclidean distance
         */
        half distance_sq(const half3& other) const noexcept;

        // ============================================================================
        // HLSL-like Functions
        // ============================================================================

        /**
         * @brief HLSL-like abs function (component-wise absolute value)
         * @return Vector with absolute values of components
         */
        half3 abs() const noexcept;

        /**
         * @brief HLSL-like sign function (component-wise sign)
         * @return Vector with signs of components (-1, 0, or 1)
         */
        half3 sign() const noexcept;

        /**
         * @brief HLSL-like floor function (component-wise floor)
         * @return Vector with floored components
         */
        half3 floor() const noexcept;

        /**
         * @brief HLSL-like ceil function (component-wise ceiling)
         * @return Vector with ceiling components
         */
        half3 ceil() const noexcept;

        /**
         * @brief HLSL-like round function (component-wise rounding)
         * @return Vector with rounded components
         */
        half3 round() const noexcept;

        /**
         * @brief HLSL-like frac function (component-wise fractional part)
         * @return Vector with fractional parts of components
         */
        half3 frac() const noexcept;

        /**
         * @brief HLSL-like saturate function (clamp components to [0, 1])
         * @return Saturated vector
         */
        half3 saturate() const noexcept;

        /**
         * @brief HLSL-like step function (component-wise step)
         * @param edge Edge value
         * @return 1.0 if component >= edge, else 0.0
         */
        half3 step(half edge) const noexcept;

        // ============================================================================
        // Geometric Operations
        // ============================================================================

        /**
         * @brief Compute reflection vector
         * @param normal Surface normal (must be normalized)
         * @return Reflected vector
         */
        half3 reflect(const half3& normal) const noexcept;

        /**
         * @brief Compute refraction vector
         * @param normal Surface normal (must be normalized)
         * @param eta Ratio of indices of refraction
         * @return Refracted vector
         */
        half3 refract(const half3& normal, half eta) const noexcept;

        /**
         * @brief Project vector onto another vector
         * @param onto Vector to project onto
         * @return Projected vector
         */
        half3 project(const half3& onto) const noexcept;

        /**
         * @brief Reject vector from another vector (component perpendicular)
         * @param from Vector to reject from
         * @return Rejected vector
         */
        half3 reject(const half3& from) const noexcept;

        // ============================================================================
        // Static Mathematical Functions (SSE Optimized where possible)
        // ============================================================================

        /**
         * @brief Compute dot product of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Dot product result
         */
        static half dot(const half3& a, const half3& b) noexcept;

        /**
         * @brief Compute cross product of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Cross product result
         */
        static half3 cross(const half3& a, const half3& b) noexcept;

        /**
         * @brief Linear interpolation between two vectors
         * @param a Start vector
         * @param b End vector
         * @param t Interpolation factor [0, 1]
         * @return Interpolated vector
         */
        static half3 lerp(const half3& a, const half3& b, half t) noexcept;

        /**
         * @brief Linear interpolation between two vectors (float factor)
         * @param a Start vector
         * @param b End vector
         * @param t Interpolation factor [0, 1] as float
         * @return Interpolated vector
         */
        static half3 lerp(const half3& a, const half3& b, float t) noexcept;

        /**
         * @brief HLSL-like saturate function (clamp components to [0, 1])
         * @param vec Vector to saturate
         * @return Saturated vector
         */
        static half3 saturate(const half3& vec) noexcept;

        /**
         * @brief Component-wise minimum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise minimum
         */
        static half3 min(const half3& a, const half3& b) noexcept;

        /**
         * @brief Component-wise maximum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise maximum
         */
        static half3 max(const half3& a, const half3& b) noexcept;

        /**
         * @brief Compute reflection vector
         * @param incident Incident vector
         * @param normal Surface normal (must be normalized)
         * @return Reflected vector
         */
        static half3 reflect(const half3& incident, const half3& normal) noexcept;

        /**
         * @brief Compute refraction vector
         * @param incident Incident vector
         * @param normal Surface normal (must be normalized)
         * @param eta Ratio of indices of refraction
         * @return Refracted vector
         */
        static half3 refract(const half3& incident, const half3& normal, half eta) noexcept;

        // ============================================================================
        // Color Operations
        // ============================================================================

        /**
         * @brief Compute luminance using Rec. 709 weights
         * @return Luminance value
         * @note Uses weights: 0.2126*R + 0.7152*G + 0.0722*B
         */
        half luminance() const noexcept;

        /**
         * @brief Convert RGB to grayscale using luminance
         * @return Grayscale color (RGB = luminance)
         */
        half3 rgb_to_grayscale() const noexcept;

        /**
         * @brief Apply gamma correction
         * @param gamma Gamma value
         * @return Gamma-corrected color
         */
        half3 gamma_correct(half gamma) const noexcept;

        /**
         * @brief Apply sRGB to linear conversion
         * @return Linear color values
         */
        half3 srgb_to_linear() const noexcept;

        /**
         * @brief Apply linear to sRGB conversion
         * @return sRGB color values
         */
        half3 linear_to_srgb() const noexcept;

        // ============================================================================
        // Swizzle Operations (HLSL style)
        // ============================================================================

        /**
         * @brief Swizzle to (x, y)
         * @return 2D vector with x and y components
         */
        half2 xy() const noexcept;

        /**
         * @brief Swizzle to (x, z)
         * @return 2D vector with x and z components
         */
        half2 xz() const noexcept;

        /**
         * @brief Swizzle to (y, z)
         * @return 2D vector with y and z components
         */
        half2 yz() const noexcept;

        /**
         * @brief Swizzle to (y, x)
         * @return 2D vector with y and x components
         */
        half2 yx() const noexcept;

        /**
         * @brief Swizzle to (z, x)
         * @return 2D vector with z and x components
         */
        half2 zx() const noexcept;

        /**
         * @brief Swizzle to (z, y)
         * @return 2D vector with z and y components
         */
        half2 zy() const noexcept;

        /**
         * @brief Swizzle to (y, x, z)
         * @return 3D vector with components rearranged
         */
        half3 yxz() const noexcept;

        /**
         * @brief Swizzle to (z, x, y)
         * @return 3D vector with components rearranged
         */
        half3 zxy() const noexcept;

        /**
         * @brief Swizzle to (z, y, x)
         * @return 3D vector with components rearranged
         */
        half3 zyx() const noexcept;

        /**
         * @brief Swizzle to (x, z, y)
         * @return 3D vector with components rearranged
         */
        half3 xzy() const noexcept;

        // Color swizzles
        /**
         * @brief Get red component (alias for x)
         * @return Red component
         */
        half r() const noexcept;

        /**
         * @brief Get green component (alias for y)
         * @return Green component
         */
        half g() const noexcept;

        /**
         * @brief Get blue component (alias for z)
         * @return Blue component
         */
        half b() const noexcept;

        /**
         * @brief Get red and green components
         * @return 2D vector with red and green components
         */
        half2 rg() const noexcept;

        /**
         * @brief Get red and blue components
         * @return 2D vector with red and blue components
         */
        half2 rb() const noexcept;

        /**
         * @brief Get green and blue components
         * @return 2D vector with green and blue components
         */
        half2 gb() const noexcept;

        /**
         * @brief Get RGB components (alias for this vector)
         * @return RGB vector
         */
        half3 rgb() const noexcept;

        /**
         * @brief Get BGR components (components reversed)
         * @return BGR vector
         */
        half3 bgr() const noexcept;

        /**
         * @brief Get GBR components (components rearranged)
         * @return GBR vector
         */
        half3 gbr() const noexcept;

        // ============================================================================
        // Utility Methods
        // ============================================================================

        /**
         * @brief Check if vector contains valid finite values
         * @return True if all components are finite (not NaN or infinity)
         */
        bool is_valid() const noexcept;

        /**
         * @brief Check if vector is approximately equal to another
         * @param other Vector to compare with
         * @param epsilon Comparison tolerance
         * @return True if vectors are approximately equal
         */
        bool approximately(const half3& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Check if vector is approximately zero
         * @param epsilon Comparison tolerance
         * @return True if vector length is approximately zero
         */
        bool approximately_zero(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Check if vector is normalized
         * @param epsilon Comparison tolerance
         * @return True if vector length is approximately 1.0
         */
        bool is_normalized(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Convert to string representation
         * @return String in format "(x, y, z)"
         */
        std::string to_string() const;

        /**
         * @brief Get pointer to raw data
         * @return Pointer to first component
         */
        const half* data() const noexcept;

        /**
         * @brief Get pointer to raw data (mutable)
         * @return Pointer to first component
         */
        half* data() noexcept;

        /**
         * @brief Set x and y components from half2
         * @param xy 2D vector for x and y components
         */
        void set_xy(const half2& xy) noexcept;

        // ============================================================================
        // Comparison Operators
        // ============================================================================

        /**
         * @brief Equality comparison
         * @param rhs Vector to compare with
         * @return True if vectors are approximately equal
         */
        bool operator==(const half3& rhs) const noexcept;

        /**
         * @brief Inequality comparison
         * @param rhs Vector to compare with
         * @return True if vectors are not approximately equal
         */
        bool operator!=(const half3& rhs) const noexcept;

        /**
         * @brief Check if any component is infinity
         * @return True if any component is positive or negative infinity
         */
        bool is_inf() const noexcept
        {
            return x.is_inf() || y.is_inf() || z.is_inf();
        }

        /**
         * @brief Check if any component is negative infinity
         * @return True if any component is negative infinity
         */
        bool is_negative_inf() const noexcept
        {
            return x.is_negative_inf() || y.is_negative_inf() || z.is_negative_inf();
        }

        /**
         * @brief Check if any component is positive infinity
         * @return True if any component is positive infinity
         */
        bool is_positive_inf() const noexcept
        {
            return x.is_positive_inf() || y.is_positive_inf() || z.is_positive_inf();
        }

        /**
         * @brief Check if any component is negative (including negative zero)
         * @return True if any component is negative
         */
        bool is_negative() const noexcept
        {
            return x.is_negative() || y.is_negative() || z.is_negative();
        }

        /**
         * @brief Check if all components are negative (including negative zero)
         * @return True if all components are negative
         */
        bool is_all_negative() const noexcept
        {
            return x.is_negative() && y.is_negative() && z.is_negative();
        }

        /**
         * @brief Check if any component is positive (excluding negative zero)
         * @return True if any component is positive
         */
        bool is_positive() const noexcept
        {
            return x.is_positive() || y.is_positive() || z.is_positive();
        }

        /**
         * @brief Check if all components are positive (excluding negative zero)
         * @return True if all components are positive
         */
        bool is_all_positive() const noexcept
        {
            return x.is_positive() && y.is_positive() && z.is_positive();
        }

        /**
         * @brief Check if any component is NaN (Not a Number)
         * @return True if any component is NaN
         */
        bool is_nan() const noexcept
        {
            return x.is_nan() || y.is_nan() || z.is_nan();
        }

        /**
         * @brief Check if all components are NaN
         * @return True if all components are NaN
         */
        bool is_all_nan() const noexcept
        {
            return x.is_nan() && y.is_nan() && z.is_nan();
        }

        /**
         * @brief Check if any component is finite (not NaN and not infinity)
         * @return True if any component is finite
         */
        bool is_finite() const noexcept
        {
            return x.is_finite() || y.is_finite() || z.is_finite();
        }

        /**
         * @brief Check if all components are finite (not NaN and not infinity)
         * @return True if all components are finite
         */
        bool is_all_finite() const noexcept
        {
            return x.is_finite() && y.is_finite() && z.is_finite();
        }

        /**
         * @brief Check if any component is zero (positive or negative)
         * @return True if any component is zero
         */
        bool is_zero() const noexcept
        {
            return x.is_zero() || y.is_zero() || z.is_zero();
        }

        /**
         * @brief Check if all components are zero (positive or negative)
         * @return True if all components are zero
         */
        bool is_all_zero() const noexcept
        {
            return x.is_zero() && y.is_zero() && z.is_zero();
        }

        /**
         * @brief Check if any component is positive zero
         * @return True if any component is positive zero
         */
        bool is_positive_zero() const noexcept
        {
            return x.is_positive_zero() || y.is_positive_zero() || z.is_positive_zero();
        }

        /**
         * @brief Check if any component is negative zero
         * @return True if any component is negative zero
         */
        bool is_negative_zero() const noexcept
        {
            return x.is_negative_zero() || y.is_negative_zero() || z.is_negative_zero();
        }
    };

    // ============================================================================
    // Binary Operators
    // ============================================================================

    /**
     * @brief Vector addition
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of addition
     */
    half3 operator+(half3 lhs, const half3& rhs) noexcept;

    /**
     * @brief Vector subtraction
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of subtraction
     */
    half3 operator-(half3 lhs, const half3& rhs) noexcept;

    /**
     * @brief Component-wise vector multiplication
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of multiplication
     */
    half3 operator*(half3 lhs, const half3& rhs) noexcept;

    /**
     * @brief Component-wise vector division
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of division
     */
    half3 operator/(half3 lhs, const half3& rhs) noexcept;

    /**
     * @brief Vector-scalar multiplication (half)
     * @param vec Vector to multiply
     * @param scalar Scalar multiplier
     * @return Scaled vector
     */
    half3 operator*(half3 vec, half scalar) noexcept;

    /**
     * @brief Scalar-vector multiplication (half)
     * @param scalar Scalar multiplier
     * @param vec Vector to multiply
     * @return Scaled vector
     */
    half3 operator*(half scalar, half3 vec) noexcept;

    /**
     * @brief Vector-scalar division (half)
     * @param vec Vector to divide
     * @param scalar Scalar divisor
     * @return Scaled vector
     */
    half3 operator/(half3 vec, half scalar) noexcept;

    /**
     * @brief Vector-scalar multiplication (float)
     * @param vec Vector to multiply
     * @param scalar Scalar multiplier
     * @return Scaled vector
     */
    half3 operator*(half3 vec, float scalar) noexcept;

    /**
     * @brief Scalar-vector multiplication (float)
     * @param scalar Scalar multiplier
     * @param vec Vector to multiply
     * @return Scaled vector
     */
    half3 operator*(float scalar, half3 vec) noexcept;

    /**
     * @brief Vector-scalar division (float)
     * @param vec Vector to divide
     * @param scalar Scalar divisor
     * @return Scaled vector
     */
    half3 operator/(half3 vec, float scalar) noexcept;

    // ============================================================================
    // Mixed Type Operators (half3 <-> float3)
    // ============================================================================

    /**
     * @brief Addition between half3 and float3
     * @param lhs half3 vector
     * @param rhs float3 vector
     * @return Result of addition
     */
    half3 operator+(const half3& lhs, const float3& rhs) noexcept;

    /**
     * @brief Subtraction between half3 and float3
     * @param lhs half3 vector
     * @param rhs float3 vector
     * @return Result of subtraction
     */
    half3 operator-(const half3& lhs, const float3& rhs) noexcept;

    /**
     * @brief Multiplication between half3 and float3
     * @param lhs half3 vector
     * @param rhs float3 vector
     * @return Result of multiplication
     */
    half3 operator*(const half3& lhs, const float3& rhs) noexcept;

    /**
     * @brief Division between half3 and float3
     * @param lhs half3 vector
     * @param rhs float3 vector
     * @return Result of division
     */
    half3 operator/(const half3& lhs, const float3& rhs) noexcept;

    /**
     * @brief Addition between float3 and half3
     * @param lhs float3 vector
     * @param rhs half3 vector
     * @return Result of addition
     */
    half3 operator+(const float3& lhs, const half3& rhs) noexcept;

    /**
     * @brief Subtraction between float3 and half3
     * @param lhs float3 vector
     * @param rhs half3 vector
     * @return Result of subtraction
     */
    half3 operator-(const float3& lhs, const half3& rhs) noexcept;

    /**
     * @brief Multiplication between float3 and half3
     * @param lhs float3 vector
     * @param rhs half3 vector
     * @return Result of multiplication
     */
    half3 operator*(const float3& lhs, const half3& rhs) noexcept;

    /**
     * @brief Division between float3 and half3
     * @param lhs float3 vector
     * @param rhs half3 vector
     * @return Result of division
     */
    half3 operator/(const float3& lhs, const half3& rhs) noexcept;

    // ============================================================================
    // Global Mathematical Functions
    // ============================================================================

    /**
     * @brief Compute distance between two points
     * @param a First point
     * @param b Second point
     * @return Euclidean distance between points
     */
    half distance(const half3& a, const half3& b) noexcept;

    /**
     * @brief Compute squared distance between two points (faster)
     * @param a First point
     * @param b Second point
     * @return Squared Euclidean distance
     */
    half distance_sq(const half3& a, const half3& b) noexcept;

    /**
     * @brief Compute dot product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return Dot product result
     */
    half dot(const half3& a, const half3& b) noexcept;

    /**
     * @brief Compute cross product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return Cross product result
     */
    half3 cross(const half3& a, const half3& b) noexcept;

    /**
     * @brief Normalize vector to unit length
     * @param vec Vector to normalize
     * @return Normalized vector
     */
    half3 normalize(const half3& vec) noexcept;

    /**
     * @brief Linear interpolation between two vectors
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation factor [0, 1]
     * @return Interpolated vector
     */
    half3 lerp(const half3& a, const half3& b, half t) noexcept;

    /**
     * @brief Linear interpolation between two vectors (float factor)
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation factor [0, 1] as float
     * @return Interpolated vector
     */
    half3 lerp(const half3& a, const half3& b, float t) noexcept;

    /**
     * @brief HLSL-like saturate function (clamp components to [0, 1])
     * @param vec Vector to saturate
     * @return Saturated vector
     */
    half3 saturate(const half3& vec) noexcept;

    /**
     * @brief Compute reflection vector
     * @param incident Incident vector
     * @param normal Surface normal (must be normalized)
     * @return Reflected vector
     */
    half3 reflect(const half3& incident, const half3& normal) noexcept;

    /**
     * @brief Compute refraction vector
     * @param incident Incident vector
     * @param normal Surface normal (must be normalized)
     * @param eta Ratio of indices of refraction
     * @return Refracted vector
     */
    half3 refract(const half3& incident, const half3& normal, half eta) noexcept;

    /**
     * @brief Check if two vectors are approximately equal
     * @param a First vector
     * @param b Second vector
     * @param epsilon Comparison tolerance
     * @return True if vectors are approximately equal
     */
    bool approximately(const half3& a, const half3& b, float epsilon = Constants::Constants<float>::Epsilon) noexcept;

    /**
     * @brief Check if vector contains valid finite values
     * @param vec Vector to check
     * @return True if vector is valid (finite values)
     */
    bool is_valid(const half3& vec) noexcept;

    /**
     * @brief Check if vector is normalized
     * @param vec Vector to check
     * @param epsilon Comparison tolerance
     * @return True if vector is normalized
     */
    bool is_normalized(const half3& vec, float epsilon = Constants::Constants<float>::Epsilon) noexcept;

    // ============================================================================
    // HLSL-like Global Functions
    // ============================================================================

    /**
     * @brief HLSL-like abs function (component-wise absolute value)
     * @param vec Input vector
     * @return Vector with absolute values of components
     */
    half3 abs(const half3& vec) noexcept;

    /**
     * @brief HLSL-like sign function (component-wise sign)
     * @param vec Input vector
     * @return Vector with signs of components
     */
    half3 sign(const half3& vec) noexcept;

    /**
     * @brief HLSL-like floor function (component-wise floor)
     * @param vec Input vector
     * @return Vector with floored components
     */
    half3 floor(const half3& vec) noexcept;

    /**
     * @brief HLSL-like ceil function (component-wise ceiling)
     * @param vec Input vector
     * @return Vector with ceiling components
     */
    half3 ceil(const half3& vec) noexcept;

    /**
     * @brief HLSL-like round function (component-wise rounding)
     * @param vec Input vector
     * @return Vector with rounded components
     */
    half3 round(const half3& vec) noexcept;

    /**
     * @brief HLSL-like frac function (component-wise fractional part)
     * @param vec Input vector
     * @return Vector with fractional parts of components
     */
    half3 frac(const half3& vec) noexcept;

    /**
     * @brief HLSL-like step function (component-wise step)
     * @param edge Edge value
     * @param vec Input vector
     * @return Step result vector
     */
    half3 step(half edge, const half3& vec) noexcept;

    /**
     * @brief HLSL-like min function (component-wise minimum)
     * @param a First vector
     * @param b Second vector
     * @return Component-wise minimum
     */
    half3 min(const half3& a, const half3& b) noexcept;

    /**
     * @brief HLSL-like max function (component-wise maximum)
     * @param a First vector
     * @param b Second vector
     * @return Component-wise maximum
     */
    half3 max(const half3& a, const half3& b) noexcept;

    /**
     * @brief HLSL-like clamp function (component-wise clamping)
     * @param vec Vector to clamp
     * @param min_val Minimum values
     * @param max_val Maximum values
     * @return Clamped vector
     */
    half3 clamp(const half3& vec, const half3& min_val, const half3& max_val) noexcept;

    /**
     * @brief HLSL-like clamp function (scalar boundaries)
     * @param vec Vector to clamp
     * @param min_val Minimum value
     * @param max_val Maximum value
     * @return Clamped vector
     */
    half3 clamp(const half3& vec, float min_val, float max_val) noexcept;

    /**
     * @brief HLSL-like smoothstep function (component-wise smooth interpolation)
     * @param edge0 Lower edge
     * @param edge1 Upper edge
     * @param vec Input vector
     * @return Smoothly interpolated vector
     */
    half3 smoothstep(half edge0, half edge1, const half3& vec) noexcept;

    // ============================================================================
    // Geometric Operations
    // ============================================================================

    /**
     * @brief Project vector onto another vector
     * @param vec Vector to project
     * @param onto Vector to project onto
     * @return Projected vector
     */
    half3 project(const half3& vec, const half3& onto) noexcept;

    /**
     * @brief Reject vector from another vector (component perpendicular)
     * @param vec Vector to reject
     * @param from Vector to reject from
     * @return Rejected vector
     */
    half3 reject(const half3& vec, const half3& from) noexcept;

    /**
     * @brief Compute angle between two vectors in radians
     * @param a First vector
     * @param b Second vector
     * @return Angle in radians between [0, π]
     */
    half angle_between(const half3& a, const half3& b) noexcept;

    // ============================================================================
    // Color Operations
    // ============================================================================

    /**
     * @brief Convert RGB to grayscale using luminance
     * @param rgb RGB color vector
     * @return Grayscale color (RGB = luminance)
     */
    half3 rgb_to_grayscale(const half3& rgb) noexcept;

    /**
     * @brief Compute luminance of RGB color
     * @param rgb RGB color vector
     * @return Luminance value
     */
    half luminance(const half3& rgb) noexcept;

    /**
     * @brief Apply gamma correction to color
     * @param color Input color
     * @param gamma Gamma value
     * @return Gamma-corrected color
     */
    half3 gamma_correct(const half3& color, half gamma) noexcept;

    /**
     * @brief Convert sRGB color to linear space
     * @param srgb sRGB color vector
     * @return Linear color values
     */
    half3 srgb_to_linear(const half3& srgb) noexcept;

    /**
     * @brief Convert linear color to sRGB space
     * @param linear Linear color vector
     * @return sRGB color values
     */
    half3 linear_to_srgb(const half3& linear) noexcept;

    // ============================================================================
    // Type Conversion Functions
    // ============================================================================

    /**
     * @brief Convert half3 to float3 (promotes components to full precision)
     * @param vec half-precision vector
     * @return full-precision vector
     */
    float3 to_float3(const half3& vec) noexcept;

    /**
     * @brief Convert float3 to half3 (demotes components to half precision)
     * @param vec full-precision vector
     * @return half-precision vector
     */
    half3 to_half3(const float3& vec) noexcept;

    // ============================================================================
    // Utility Functions
    // ============================================================================

    /**
     * @brief Ensure vector is normalized, with fallback to safe value
     * @param normal Vector to normalize
     * @param fallback Fallback vector if normalization fails
     * @return Normalized vector or fallback if normalization fails
     */
    half3 ensure_normalized(const half3& normal, const half3& fallback = half3::unit_z()) noexcept;

    // ============================================================================
    // Useful Constants
    // ============================================================================

    /**
     * @brief Zero vector constant (0, 0, 0)
     */
    extern const half3 half3_Zero;

    /**
     * @brief One vector constant (1, 1, 1)
     */
    extern const half3 half3_One;

    /**
     * @brief Unit X vector constant (1, 0, 0)
     */
    extern const half3 half3_UnitX;

    /**
     * @brief Unit Y vector constant (0, 1, 0)
     */
    extern const half3 half3_UnitY;

    /**
     * @brief Unit Z vector constant (0, 0, 1)
     */
    extern const half3 half3_UnitZ;

    /**
     * @brief Forward vector constant (0, 0, 1)
     */
    extern const half3 half3_Forward;

    /**
     * @brief Up vector constant (0, 1, 0)
     */
    extern const half3 half3_Up;

    /**
     * @brief Right vector constant (1, 0, 0)
     */
    extern const half3 half3_Right;

    /**
     * @brief Red color constant (1, 0, 0)
     */
    extern const half3 half3_Red;

    /**
     * @brief Green color constant (0, 1, 0)
     */
    extern const half3 half3_Green;

    /**
     * @brief Blue color constant (0, 0, 1)
     */
    extern const half3 half3_Blue;

    /**
     * @brief White color constant (1, 1, 1)
     */
    extern const half3 half3_White;

    /**
     * @brief Black color constant (0, 0, 0)
     */
    extern const half3 half3_Black;

    /**
     * @brief Yellow color constant (1, 1, 0)
     */
    extern const half3 half3_Yellow;

    /**
     * @brief Cyan color constant (0, 1, 1)
     */
    extern const half3 half3_Cyan;

    /**
     * @brief Magenta color constant (1, 0, 1)
     */
    extern const half3 half3_Magenta;

} // namespace Math

#include "math_half3.inl"
