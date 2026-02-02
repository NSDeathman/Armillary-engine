// Description: 2-dimensional half-precision vector class with 
//              comprehensive mathematical operations and HLSL compatibility
//              Optimized for memory bandwidth and GPU data formats
// Author: NSDeathman, DeepSeek
#pragma once

/**
 * @file math_half2.h
 * @brief 2-dimensional half-precision vector class
 * @note Optimized for memory bandwidth, GPU data formats, and texture coordinates
 * @note Provides seamless interoperability with float2 and comprehensive HLSL-like functions
 */

#include <cmath>
#include <algorithm>
#include <string>
#include <cstdio>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"
#include "math_half.h"
#include "math_float2.h"

namespace Math
{
    /**
     * @class half2
     * @brief 2-dimensional half-precision vector with comprehensive mathematical operations
     *
     * Represents a 2D vector (x, y) using 16-bit half-precision floating point format.
     * Optimized for memory bandwidth, GPU data formats, and scenarios where full 32-bit
     * precision is not required (texture coordinates, normals, colors, etc.).
     *
     * @note Perfect for texture coordinates, 2D graphics, UI systems, and memory-constrained applications
     * @note Provides seamless interoperability with float2 and comprehensive HLSL-like functions
     * @note All operations maintain half-precision accuracy while providing performance benefits
     */
    class half2
    {
    public:
        // ============================================================================
        // Data Members (Public for Direct Access)
        // ============================================================================

        half x; ///< X component of the vector
        half y; ///< Y component of the vector

        // ============================================================================
        // Constructors
        // ============================================================================

        /**
         * @brief Default constructor (initializes to zero vector)
         */
        half2() noexcept;

        /**
         * @brief Construct from half components
         * @param x X component
         * @param y Y component
         */
        half2(half x, half y) noexcept;

        /**
         * @brief Construct from scalar (all components set to same value)
         * @param scalar Value for all components
         */
        explicit half2(half scalar) noexcept;

        /**
         * @brief Construct from float components
         * @param x X component as float
         * @param y Y component as float
         */
        half2(float x, float y) noexcept;

        /**
         * @brief Construct from float scalar (all components set to same value)
         * @param scalar Value for all components as float
         */
        explicit half2(float scalar) noexcept;

        /**
         * @brief Copy constructor
         */
        half2(const half2&) noexcept = default;

        /**
         * @brief Construct from float2 (converts components to half precision)
         * @param vec 32-bit floating point vector
         */
        half2(const float2& vec) noexcept;

        // ============================================================================
        // Assignment Operators
        // ============================================================================

        /**
         * @brief Copy assignment operator
         */
        half2& operator=(const half2&) noexcept = default;

        /**
         * @brief Assignment from float2 (converts components to half precision)
         * @param vec 32-bit floating point vector
         */
        half2& operator=(const float2& vec) noexcept;

        /**
         * @brief Assignment from half scalar (sets all components to same value)
         * @param scalar Value for all components
         */
        half2& operator=(half scalar) noexcept;

        /**
         * @brief Assignment from float scalar (sets all components to same value)
         * @param scalar Value for all components as float
         */
        half2& operator=(float scalar) noexcept;

        // ============================================================================
        // Compound Assignment Operators
        // ============================================================================

        /**
         * @brief Compound addition assignment
         * @param rhs Vector to add
         * @return Reference to this object
         */
        half2& operator+=(const half2& rhs) noexcept;

        /**
         * @brief Compound subtraction assignment
         * @param rhs Vector to subtract
         * @return Reference to this object
         */
        half2& operator-=(const half2& rhs) noexcept;

        /**
         * @brief Compound multiplication assignment
         * @param rhs Vector to multiply by
         * @return Reference to this object
         */
        half2& operator*=(const half2& rhs) noexcept;

        /**
         * @brief Compound division assignment
         * @param rhs Vector to divide by
         * @return Reference to this object
         */
        half2& operator/=(const half2& rhs) noexcept;

        /**
         * @brief Compound scalar multiplication assignment (half)
         * @param scalar Scalar to multiply by
         * @return Reference to this object
         */
        half2& operator*=(half scalar) noexcept;

        /**
         * @brief Compound scalar multiplication assignment (float)
         * @param scalar Scalar to multiply by
         * @return Reference to this object
         */
        half2& operator*=(float scalar) noexcept;

        /**
         * @brief Compound scalar division assignment (half)
         * @param scalar Scalar to divide by
         * @return Reference to this object
         */
        half2& operator/=(half scalar) noexcept;

        /**
         * @brief Compound scalar division assignment (float)
         * @param scalar Scalar to divide by
         * @return Reference to this object
         */
        half2& operator/=(float scalar) noexcept;

        // ============================================================================
        // Unary Operators
        // ============================================================================

        /**
         * @brief Unary plus operator
         * @return Positive vector
         */
        half2 operator+() const noexcept;

        /**
         * @brief Unary minus operator
         * @return Negated vector
         */
        half2 operator-() const noexcept;

        // ============================================================================
        // Access Operators
        // ============================================================================

        /**
         * @brief Access component by index
         * @param index Component index (0 = x, 1 = y)
         * @return Reference to component
         */
        half& operator[](int index) noexcept;

        /**
         * @brief Access component by index (const)
         * @param index Component index (0 = x, 1 = y)
         * @return Const reference to component
         */
        const half& operator[](int index) const noexcept;

        // ============================================================================
        // Conversion Operators
        // ============================================================================

        /**
         * @brief Convert to float2 (promotes components to full precision)
         * @return 32-bit floating point vector
         */
        explicit operator float2() const noexcept;

        // ============================================================================
        // Static Constructors
        // ============================================================================

        /**
         * @brief Zero vector (0, 0)
         * @return Zero vector
         */
        static half2 zero() noexcept;

        /**
         * @brief One vector (1, 1)
         * @return One vector
         */
        static half2 one() noexcept;

        /**
         * @brief Unit X vector (1, 0)
         * @return Unit X vector
         */
        static half2 unit_x() noexcept;

        /**
         * @brief Unit Y vector (0, 1)
         * @return Unit Y vector
         */
        static half2 unit_y() noexcept;

        /**
         * @brief Create from texture coordinates
         * @param u U coordinate
         * @param v V coordinate
         * @return Texture coordinate vector
         */
        static half2 uv(half u, half v) noexcept;

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
        half2 normalize() const noexcept;

        /**
         * @brief Compute dot product with another vector
         * @param other Other vector
         * @return Dot product result
         */
        half dot(const half2& other) const noexcept;

        /**
         * @brief Compute perpendicular vector (90 degree counter-clockwise rotation)
         * @return Perpendicular vector (-y, x)
         */
        half2 perpendicular() const noexcept;

        /**
         * @brief Compute distance to another point
         * @param other Other point
         * @return Euclidean distance
         */
        half distance(const half2& other) const noexcept;

        /**
         * @brief Compute squared distance to another point (faster)
         * @param other Other point
         * @return Squared Euclidean distance
         */
        half distance_sq(const half2& other) const noexcept;

        // ============================================================================
        // HLSL-like Functions
        // ============================================================================

        /**
         * @brief HLSL-like abs function (component-wise absolute value)
         * @return Vector with absolute values of components
         */
        half2 abs() const noexcept;

        /**
         * @brief HLSL-like sign function (component-wise sign)
         * @return Vector with signs of components (-1, 0, or 1)
         */
        half2 sign() const noexcept;

        /**
         * @brief HLSL-like floor function (component-wise floor)
         * @return Vector with floored components
         */
        half2 floor() const noexcept;

        /**
         * @brief HLSL-like ceil function (component-wise ceiling)
         * @return Vector with ceiling components
         */
        half2 ceil() const noexcept;

        /**
         * @brief HLSL-like round function (component-wise rounding)
         * @return Vector with rounded components
         */
        half2 round() const noexcept;

        /**
         * @brief HLSL-like frac function (component-wise fractional part)
         * @return Vector with fractional parts of components
         */
        half2 frac() const noexcept;

        /**
         * @brief HLSL-like saturate function (clamp components to [0, 1])
         * @return Saturated vector
         */
        half2 saturate() const noexcept;

        /**
         * @brief HLSL-like step function (component-wise step)
         * @param edge Edge value
         * @return 1.0 if component >= edge, else 0.0
         */
        half2 step(half edge) const noexcept;

        // ============================================================================
        // Static Mathematical Functions
        // ============================================================================

        /**
         * @brief Compute dot product of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Dot product result
         */
        static half dot(const half2& a, const half2& b) noexcept;

        /**
         * @brief Linear interpolation between two vectors
         * @param a Start vector
         * @param b End vector
         * @param t Interpolation factor [0, 1]
         * @return Interpolated vector
         */
        static half2 lerp(const half2& a, const half2& b, half t) noexcept;

        /**
         * @brief Linear interpolation between two vectors (float factor)
         * @param a Start vector
         * @param b End vector
         * @param t Interpolation factor [0, 1] as float
         * @return Interpolated vector
         */
        static half2 lerp(const half2& a, const half2& b, float t) noexcept;

        /**
         * @brief HLSL-like saturate function (clamp components to [0, 1])
         * @param vec Vector to saturate
         * @return Saturated vector
         */
        static half2 saturate(const half2& vec) noexcept;

        /**
         * @brief Component-wise minimum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise minimum
         */
        static half2 min(const half2& a, const half2& b) noexcept;

        /**
         * @brief Component-wise maximum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise maximum
         */
        static half2 max(const half2& a, const half2& b) noexcept;

        // ============================================================================
        // Swizzle Operations (HLSL style)
        // ============================================================================

        /**
         * @brief Swizzle to (y, x)
         * @return Vector with components swapped
         */
        half2 yx() const noexcept;

        /**
         * @brief Swizzle to (x, x)
         * @return Vector with x component duplicated
         */
        half2 xx() const noexcept;

        /**
         * @brief Swizzle to (y, y)
         * @return Vector with y component duplicated
         */
        half2 yy() const noexcept;

        // ============================================================================
        // Texture Coordinate Accessors
        // ============================================================================

        /**
         * @brief Get U texture coordinate (alias for x)
         * @return U coordinate
         */
        half u() const noexcept;

        /**
         * @brief Get V texture coordinate (alias for y)
         * @return V coordinate
         */
        half v() const noexcept;

        /**
         * @brief Set U texture coordinate
         * @param u U coordinate value
         */
        void set_u(half u) noexcept;

        /**
         * @brief Set V texture coordinate
         * @param v V coordinate value
         */
        void set_v(half v) noexcept;

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
        bool approximately(const half2& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

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
         * @return String in format "(x, y)"
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

        // ============================================================================
        // Comparison Operators
        // ============================================================================

        /**
         * @brief Equality comparison
         * @param rhs Vector to compare with
         * @return True if vectors are approximately equal
         */
        bool operator==(const half2& rhs) const noexcept;

        /**
         * @brief Inequality comparison
         * @param rhs Vector to compare with
         * @return True if vectors are not approximately equal
         */
        bool operator!=(const half2& rhs) const noexcept;

        /**
         * @brief Check if any component is infinity
         * @return True if any component is positive or negative infinity
         */
        bool is_inf() const noexcept
        {
            return x.is_inf() || y.is_inf();
        }

        /**
         * @brief Check if any component is negative infinity
         * @return True if any component is negative infinity
         */
        bool is_negative_inf() const noexcept
        {
            return x.is_negative_inf() || y.is_negative_inf();
        }

        /**
         * @brief Check if any component is positive infinity
         * @return True if any component is positive infinity
         */
        bool is_positive_inf() const noexcept
        {
            return x.is_positive_inf() || y.is_positive_inf();
        }

        /**
         * @brief Check if any component is negative (including negative zero)
         * @return True if any component is negative
         */
        bool is_negative() const noexcept
        {
            return x.is_negative() || y.is_negative();
        }

        /**
         * @brief Check if all components are negative (including negative zero)
         * @return True if all components are negative
         */
        bool is_all_negative() const noexcept
        {
            return x.is_negative() && y.is_negative();
        }

        /**
         * @brief Check if any component is positive (excluding negative zero)
         * @return True if any component is positive
         */
        bool is_positive() const noexcept
        {
            return x.is_positive() || y.is_positive();
        }

        /**
         * @brief Check if all components are positive (excluding negative zero)
         * @return True if all components are positive
         */
        bool is_all_positive() const noexcept
        {
            return x.is_positive() && y.is_positive();
        }

        /**
         * @brief Check if any component is NaN (Not a Number)
         * @return True if any component is NaN
         */
        bool is_nan() const noexcept
        {
            return x.is_nan() || y.is_nan();
        }

        /**
         * @brief Check if all components are NaN
         * @return True if all components are NaN
         */
        bool is_all_nan() const noexcept
        {
            return x.is_nan() && y.is_nan();
        }

        /**
         * @brief Check if any component is finite (not NaN and not infinity)
         * @return True if any component is finite
         */
        bool is_finite() const noexcept
        {
            return x.is_finite() || y.is_finite();
        }

        /**
         * @brief Check if all components are finite (not NaN and not infinity)
         * @return True if all components are finite
         */
        bool is_all_finite() const noexcept
        {
            return x.is_finite() && y.is_finite();
        }

        /**
         * @brief Check if any component is zero (positive or negative)
         * @return True if any component is zero
         */
        bool is_zero() const noexcept
        {
            return x.is_zero() || y.is_zero();
        }

        /**
         * @brief Check if all components are zero (positive or negative)
         * @return True if all components are zero
         */
        bool is_all_zero() const noexcept
        {
            return x.is_zero() && y.is_zero();
        }

        /**
         * @brief Check if any component is positive zero
         * @return True if any component is positive zero
         */
        bool is_positive_zero() const noexcept
        {
            return x.is_positive_zero() || y.is_positive_zero();
        }

        /**
         * @brief Check if any component is negative zero
         * @return True if any component is negative zero
         */
        bool is_negative_zero() const noexcept
        {
            return x.is_negative_zero() || y.is_negative_zero();
        }
    };

    // ============================================================================
    // Binary Operators (declarations)
    // ============================================================================

    /**
     * @brief Vector addition
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of addition
     */
    half2 operator+(half2 lhs, const half2& rhs) noexcept;

    /**
     * @brief Vector subtraction
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of subtraction
     */
    half2 operator-(half2 lhs, const half2& rhs) noexcept;

    /**
     * @brief Component-wise vector multiplication
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of multiplication
     */
    half2 operator*(half2 lhs, const half2& rhs) noexcept;

    /**
     * @brief Component-wise vector division
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of division
     */
    half2 operator/(half2 lhs, const half2& rhs) noexcept;

    /**
     * @brief Vector-scalar multiplication (half)
     * @param vec Vector to multiply
     * @param scalar Scalar multiplier
     * @return Scaled vector
     */
    half2 operator*(half2 vec, half scalar) noexcept;

    /**
     * @brief Scalar-vector multiplication (half)
     * @param scalar Scalar multiplier
     * @param vec Vector to multiply
     * @return Scaled vector
     */
    half2 operator*(half scalar, half2 vec) noexcept;

    /**
     * @brief Vector-scalar division (half)
     * @param vec Vector to divide
     * @param scalar Scalar divisor
     * @return Scaled vector
     */
    half2 operator/(half2 vec, half scalar) noexcept;

    /**
     * @brief Vector-scalar multiplication (float)
     * @param vec Vector to multiply
     * @param scalar Scalar multiplier
     * @return Scaled vector
     */
    half2 operator*(half2 vec, float scalar) noexcept;

    /**
     * @brief Scalar-vector multiplication (float)
     * @param scalar Scalar multiplier
     * @param vec Vector to multiply
     * @return Scaled vector
     */
    half2 operator*(float scalar, half2 vec) noexcept;

    /**
     * @brief Vector-scalar division (float)
     * @param vec Vector to divide
     * @param scalar Scalar divisor
     * @return Scaled vector
     */
    half2 operator/(half2 vec, float scalar) noexcept;

    // ============================================================================
    // Mixed Type Operators (half2 <-> float2) (declarations)
    // ============================================================================

    /**
     * @brief Addition between half2 and float2
     * @param lhs half2 vector
     * @param rhs float2 vector
     * @return Result of addition
     */
    half2 operator+(const half2& lhs, const float2& rhs) noexcept;

    /**
     * @brief Subtraction between half2 and float2
     * @param lhs half2 vector
     * @param rhs float2 vector
     * @return Result of subtraction
     */
    half2 operator-(const half2& lhs, const float2& rhs) noexcept;

    /**
     * @brief Multiplication between half2 and float2
     * @param lhs half2 vector
     * @param rhs float2 vector
     * @return Result of multiplication
     */
    half2 operator*(const half2& lhs, const float2& rhs) noexcept;

    /**
     * @brief Division between half2 and float2
     * @param lhs half2 vector
     * @param rhs float2 vector
     * @return Result of division
     */
    half2 operator/(const half2& lhs, const float2& rhs) noexcept;

    /**
     * @brief Addition between float2 and half2
     * @param lhs float2 vector
     * @param rhs half2 vector
     * @return Result of addition
     */
    half2 operator+(const float2& lhs, const half2& rhs) noexcept;

    /**
     * @brief Subtraction between float2 and half2
     * @param lhs float2 vector
     * @param rhs half2 vector
     * @return Result of subtraction
     */
    half2 operator-(const float2& lhs, const half2& rhs) noexcept;

    /**
     * @brief Multiplication between float2 and half2
     * @param lhs float2 vector
     * @param rhs half2 vector
     * @return Result of multiplication
     */
    half2 operator*(const float2& lhs, const half2& rhs) noexcept;

    /**
     * @brief Division between float2 and half2
     * @param lhs float2 vector
     * @param rhs half2 vector
     * @return Result of division
     */
    half2 operator/(const float2& lhs, const half2& rhs) noexcept;

    // ============================================================================
    // Global Mathematical Functions (declarations)
    // ============================================================================

    /**
     * @brief Compute distance between two points
     * @param a First point
     * @param b Second point
     * @return Euclidean distance between points
     */
    half distance(const half2& a, const half2& b) noexcept;

    /**
     * @brief Compute squared distance between two points (faster)
     * @param a First point
     * @param b Second point
     * @return Squared Euclidean distance
     */
    half distance_sq(const half2& a, const half2& b) noexcept;

    /**
     * @brief Compute dot product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return Dot product result
     */
    half dot(const half2& a, const half2& b) noexcept;

    /**
     * @brief Normalize vector to unit length
     * @param vec Vector to normalize
     * @return Normalized vector
     */
    half2 normalize(const half2& vec) noexcept;

    /**
     * @brief Linear interpolation between two vectors
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation factor [0, 1]
     * @return Interpolated vector
     */
    half2 lerp(const half2& a, const half2& b, half t) noexcept;

    /**
     * @brief Linear interpolation between two vectors (float factor)
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation factor [0, 1] as float
     * @return Interpolated vector
     */
    half2 lerp(const half2& a, const half2& b, float t) noexcept;

    /**
     * @brief HLSL-like saturate function (clamp components to [0, 1])
     * @param vec Vector to saturate
     * @return Saturated vector
     */
    half2 saturate(const half2& vec) noexcept;

    /**
     * @brief Check if two vectors are approximately equal
     * @param a First vector
     * @param b Second vector
     * @param epsilon Comparison tolerance
     * @return True if vectors are approximately equal
     */
    bool approximately(const half2& a, const half2& b, float epsilon = Constants::Constants<float>::Epsilon) noexcept;

    /**
     * @brief Check if vector contains valid finite values
     * @param vec Vector to check
     * @return True if vector is valid (finite values)
     */
    bool is_valid(const half2& vec) noexcept;

    /**
     * @brief Check if vector is normalized
     * @param vec Vector to check
     * @param epsilon Comparison tolerance
     * @return True if vector is normalized
     */
    bool is_normalized(const half2& vec, float epsilon = Constants::Constants<float>::Epsilon) noexcept;

    /**
     * @brief Check if any component of vector is infinity
     * @param vec Vector to check
     * @return True if any component is infinity
     */
    bool is_inf(const half2& vec) noexcept;

    /**
     * @brief Check if any component of vector is negative infinity
     * @param vec Vector to check
     * @return True if any component is negative infinity
     */
    bool is_negative_inf(const half2& vec) noexcept;

    /**
     * @brief Check if any component of vector is positive infinity
     * @param vec Vector to check
     * @return True if any component is positive infinity
     */
    bool is_positive_inf(const half2& vec) noexcept;

    /**
     * @brief Check if any component of vector is negative
     * @param vec Vector to check
     * @return True if any component is negative
     */
    bool is_negative(const half2& vec) noexcept;

    /**
     * @brief Check if all components of vector are negative
     * @param vec Vector to check
     * @return True if all components are negative
     */
    bool is_all_negative(const half2& vec) noexcept;

    /**
     * @brief Check if any component of vector is positive
     * @param vec Vector to check
     * @return True if any component is positive
     */
    bool is_positive(const half2& vec) noexcept;

    /**
     * @brief Check if all components of vector are positive
     * @param vec Vector to check
     * @return True if all components are positive
     */
    bool is_all_positive(const half2& vec) noexcept;

    /**
     * @brief Check if any component of vector is NaN
     * @param vec Vector to check
     * @return True if any component is NaN
     */
    bool is_nan(const half2& vec) noexcept;

    /**
     * @brief Check if all components of vector are NaN
     * @param vec Vector to check
     * @return True if all components are NaN
     */
    bool is_all_nan(const half2& vec) noexcept;

    /**
     * @brief Check if any component of vector is finite
     * @param vec Vector to check
     * @return True if any component is finite
     */
    bool is_finite(const half2& vec) noexcept;

    /**
     * @brief Check if all components of vector are finite
     * @param vec Vector to check
     * @return True if all components are finite
     */
    bool is_all_finite(const half2& vec) noexcept;

    /**
     * @brief Check if any component of vector is zero
     * @param vec Vector to check
     * @return True if any component is zero
     */
    bool is_zero(const half2& vec) noexcept;

    /**
     * @brief Check if all components of vector are zero
     * @param vec Vector to check
     * @return True if all components are zero
     */
    bool is_all_zero(const half2& vec) noexcept;

    // ============================================================================
    // HLSL-like Global Functions (declarations)
    // ============================================================================

    /**
     * @brief HLSL-like abs function (component-wise absolute value)
     * @param vec Input vector
     * @return Vector with absolute values of components
     */
    half2 abs(const half2& vec) noexcept;

    /**
     * @brief HLSL-like sign function (component-wise sign)
     * @param vec Input vector
     * @return Vector with signs of components
     */
    half2 sign(const half2& vec) noexcept;

    /**
     * @brief HLSL-like floor function (component-wise floor)
     * @param vec Input vector
     * @return Vector with floored components
     */
    half2 floor(const half2& vec) noexcept;

    /**
     * @brief HLSL-like ceil function (component-wise ceiling)
     * @param vec Input vector
     * @return Vector with ceiling components
     */
    half2 ceil(const half2& vec) noexcept;

    /**
     * @brief HLSL-like round function (component-wise rounding)
     * @param vec Input vector
     * @return Vector with rounded components
     */
    half2 round(const half2& vec) noexcept;

    /**
     * @brief HLSL-like frac function (component-wise fractional part)
     * @param vec Input vector
     * @return Vector with fractional parts of components
     */
    half2 frac(const half2& vec) noexcept;

    /**
     * @brief HLSL-like step function (component-wise step)
     * @param edge Edge value
     * @param vec Input vector
     * @return Step result vector
     */
    half2 step(half edge, const half2& vec) noexcept;

    /**
     * @brief HLSL-like min function (component-wise minimum)
     * @param a First vector
     * @param b Second vector
     * @return Component-wise minimum
     */
    half2 min(const half2& a, const half2& b) noexcept;

    /**
     * @brief HLSL-like max function (component-wise maximum)
     * @param a First vector
     * @param b Second vector
     * @return Component-wise maximum
     */
    half2 max(const half2& a, const half2& b) noexcept;

    /**
     * @brief HLSL-like clamp function (component-wise clamping)
     * @param vec Vector to clamp
     * @param min_val Minimum values
     * @param max_val Maximum values
     * @return Clamped vector
     */
    half2 clamp(const half2& vec, const half2& min_val, const half2& max_val) noexcept;

    /**
     * @brief HLSL-like clamp function (scalar boundaries)
     * @param vec Vector to clamp
     * @param min_val Minimum value
     * @param max_val Maximum value
     * @return Clamped vector
     */
    half2 clamp(const half2& vec, float min_val, float max_val) noexcept;

    /**
     * @brief HLSL-like smoothstep function (component-wise smooth interpolation)
     * @param edge0 Lower edge
     * @param edge1 Upper edge
     * @param vec Input vector
     * @return Smoothly interpolated vector
     */
    half2 smoothstep(half edge0, half edge1, const half2& vec) noexcept;

    // ============================================================================
    // Geometric Operations (declarations)
    // ============================================================================

    /**
     * @brief Compute perpendicular vector (90 degree counter-clockwise rotation)
     * @param vec Input vector
     * @return Perpendicular vector (-y, x)
     */
    half2 perpendicular(const half2& vec) noexcept;

    /**
     * @brief Compute 2D cross product (scalar result)
     * @param a First vector
     * @param b Second vector
     * @return Cross product result (x1*y2 - y1*x2)
     */
    half cross(const half2& a, const half2& b) noexcept;

    /**
     * @brief Compute angle of vector relative to X-axis
     * @param vec Input vector
     * @return Angle in radians between [-π, π]
     */
    half angle(const half2& vec) noexcept;

    /**
     * @brief Compute angle between two vectors in radians
     * @param a First vector
     * @param b Second vector
     * @return Angle in radians between [0, π]
     */
    half angle_between(const half2& a, const half2& b) noexcept;

    // ============================================================================
    // Type Conversion Functions (declarations)
    // ============================================================================

    /**
     * @brief Convert half2 to float2 (promotes components to full precision)
     * @param vec half-precision vector
     * @return full-precision vector
     */
    float2 to_float2(const half2& vec) noexcept;

    /**
     * @brief Convert float2 to half2 (demotes components to half precision)
     * @param vec full-precision vector
     * @return half-precision vector
     */
    half2 to_half2(const float2& vec) noexcept;

    // ============================================================================
    // Useful Constants
    // ============================================================================

    /**
     * @brief Zero vector constant (0, 0)
     */
    extern const half2 half2_Zero;

    /**
     * @brief One vector constant (1, 1)
     */
    extern const half2 half2_One;

    /**
     * @brief Unit X vector constant (1, 0)
     */
    extern const half2 half2_UnitX;

    /**
     * @brief Unit Y vector constant (0, 1)
     */
    extern const half2 half2_UnitY;

    /**
     * @brief Zero texture coordinate constant (0, 0)
     */
    extern const half2 half2_UV_Zero;

    /**
     * @brief One texture coordinate constant (1, 1)
     */
    extern const half2 half2_UV_One;

    /**
     * @brief Half texture coordinate constant (0.5, 0.5)
     */
    extern const half2 half2_UV_Half;

    /**
     * @brief Right vector constant (1, 0)
     */
    extern const half2 half2_Right;

    /**
     * @brief Left vector constant (-1, 0)
     */
    extern const half2 half2_Left;

    /**
     * @brief Up vector constant (0, 1)
     */
    extern const half2 half2_Up;

    /**
     * @brief Down vector constant (0, -1)
     */
    extern const half2 half2_Down;

} // namespace Math

// Include inline implementations
#include "math_half2.inl"
