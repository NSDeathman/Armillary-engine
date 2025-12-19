// Description: 2-dimensional vector class with comprehensive 
//              mathematical operations and SSE optimization
// Author: NSDeathman, DeepSeek
#pragma once

#include <string>
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <cassert>
#include <xmmintrin.h>
#include <pmmintrin.h>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"

#if defined(MATH_SUPPORT_D3DX)
#include <d3dx9.h>
#endif

namespace Math
{
    /**
     * @class float2
     * @brief 2-dimensional vector with comprehensive mathematical operations
     *
     * Represents a 2D vector (x, y) with optimized operations for 2D graphics,
     * physics simulations, and 2D mathematics. Features SSE optimization for
     * performance-critical operations.
     *
     * @note Perfect for 2D game development, UI systems, and 2D physics engines
     * @note All operations are optimized and constexpr where possible
     * @note Includes comprehensive HLSL-like function set
     */
    class MATH_API float2 {
    public:
        // ============================================================================
        // Data Members (Public for Direct Access)
        // ============================================================================

        float x; ///< X component of the vector
        float y; ///< Y component of the vector

        // ============================================================================
        // Constructors
        // ============================================================================

        /**
         * @brief Default constructor (initializes to zero vector)
         */
        constexpr float2() noexcept : x(0.0f), y(0.0f) {}

        /**
         * @brief Construct from components
         * @param x X component
         * @param y Y component
         */
        constexpr float2(float x, float y) noexcept : x(x), y(y) {}

        /**
         * @brief Construct from scalar (all components set to same value)
         * @param scalar Value for all components
         */
        explicit constexpr float2(float scalar) noexcept : x(scalar), y(scalar) {}

        /**
         * @brief Copy constructor
         */
        constexpr float2(const float2&) noexcept = default;

        /**
         * @brief Construct from raw float array
         * @param data Pointer to float array [x, y]
         */
        explicit float2(const float* data) noexcept;

        /**
         * @brief Construct from SSE register (advanced users)
         * @param simd_ SSE register containing vector data
         */
        explicit float2(__m128 simd_) noexcept;

#if defined(MATH_SUPPORT_D3DX)
        /**
         * @brief Construct from D3DXVECTOR2
         * @param vec DirectX 2D vector
         */
        float2(const D3DXVECTOR2& vec) noexcept;

        /**
         * @brief Construct from D3DXVECTOR4 (extracts x, y)
         * @param vec DirectX 4D vector
         */
        float2(const D3DXVECTOR4& vec) noexcept;

        /**
         * @brief Construct from D3DCOLOR (extracts R, G channels)
         * @param color DirectX color value
         */
        explicit float2(D3DCOLOR color) noexcept;
#endif

        // ============================================================================
        // Assignment Operators
        // ============================================================================

        /**
         * @brief Copy assignment operator
         */
        float2& operator=(const float2&) noexcept = default;

        /**
         * @brief Scalar assignment (sets all components to same value)
         * @param scalar Value for all components
         */
        float2& operator=(float scalar) noexcept;

#if defined(MATH_SUPPORT_D3DX)
        /**
         * @brief Assignment from D3DXVECTOR2
         * @param vec DirectX 2D vector
         */
        float2& operator=(const D3DXVECTOR2& vec) noexcept;

        /**
         * @brief Assignment from D3DCOLOR
         * @param color DirectX color value
         */
        float2& operator=(D3DCOLOR color) noexcept;
#endif

        // ============================================================================
        // Compound Assignment Operators
        // ============================================================================

        /**
         * @brief Component-wise addition assignment
         * @param rhs Right-hand side vector
         * @return Reference to this vector
         */
        float2& operator+=(const float2& rhs) noexcept;

        /**
         * @brief Component-wise subtraction assignment
         * @param rhs Right-hand side vector
         * @return Reference to this vector
         */
        float2& operator-=(const float2& rhs) noexcept;

        /**
         * @brief Component-wise multiplication assignment
         * @param rhs Right-hand side vector
         * @return Reference to this vector
         */
        float2& operator*=(const float2& rhs) noexcept;

        /**
         * @brief Component-wise division assignment
         * @param rhs Right-hand side vector
         * @return Reference to this vector
         */
        float2& operator/=(const float2& rhs) noexcept;

        /**
         * @brief Scalar multiplication assignment
         * @param scalar Scalar multiplier
         * @return Reference to this vector
         */
        float2& operator*=(float scalar) noexcept;

        /**
         * @brief Scalar division assignment
         * @param scalar Scalar divisor
         * @return Reference to this vector
         */
        float2& operator/=(float scalar) noexcept;

        // ============================================================================
        // Binary Operators
        // ============================================================================

        /**
         * @brief Vector addition
         * @param rhs Right-hand side vector
         * @return Result of addition
         */
        float2 operator+(const float2& rhs) const noexcept;

        /**
         * @brief Vector subtraction
         * @param rhs Right-hand side vector
         * @return Result of subtraction
         */
        float2 operator-(const float2& rhs) const noexcept;

        /**
         * @brief Vector-scalar addition
         * @param rhs Scalar to add to all components
         * @return Result of addition
         */
        float2 operator+(const float& rhs) const noexcept;

        /**
         * @brief Vector-scalar subtraction
         * @param rhs Scalar to subtract from all components
         * @return Result of subtraction
         */
        float2 operator-(const float& rhs) const noexcept;

        // ============================================================================
        // Unary Operators
        // ============================================================================

        /**
         * @brief Unary plus operator
         * @return Copy of this vector
         */
        constexpr float2 operator+() const noexcept { return *this; }

        /**
         * @brief Unary minus operator
         * @return Negated vector
         */
        constexpr float2 operator-() const noexcept { return float2(-x, -y); }

        // ============================================================================
        // Access Operators
        // ============================================================================

        /**
         * @brief Access component by index
         * @param index Component index (0 = x, 1 = y)
         * @return Reference to component
         */
        float& operator[](int index) noexcept;

        /**
         * @brief Access component by index (const)
         * @param index Component index (0 = x, 1 = y)
         * @return Const reference to component
         */
        const float& operator[](int index) const noexcept;

        // ============================================================================
        // Conversion Operators
        // ============================================================================

        /**
         * @brief Convert to const float pointer (for interoperability)
         * @return Pointer to const float array [x, y]
         */
        operator const float* () const noexcept;

        /**
         * @brief Convert to float pointer (for interoperability)
         * @return Pointer to float array [x, y]
         */
        operator float* () noexcept;

        /**
         * @brief Convert to SSE register (advanced users)
         * @return SSE register containing vector data
         */
        operator __m128() const noexcept;

#if defined(MATH_SUPPORT_D3DX)
        /**
         * @brief Convert to D3DXVECTOR2
         * @return D3DXVECTOR2 equivalent
         */
        operator D3DXVECTOR2() const noexcept;
#endif

        // ============================================================================
        // Static Constructors
        // ============================================================================

        /**
         * @brief Zero vector (0, 0)
         * @return Zero vector
         */
        static constexpr float2 zero() noexcept { return float2(0.0f, 0.0f); }

        /**
         * @brief One vector (1, 1)
         * @return One vector
         */
        static constexpr float2 one() noexcept { return float2(1.0f, 1.0f); }

        /**
         * @brief Unit X vector (1, 0)
         * @return Unit X vector
         */
        static constexpr float2 unit_x() noexcept { return float2(1.0f, 0.0f); }

        /**
         * @brief Unit Y vector (0, 1)
         * @return Unit Y vector
         */
        static constexpr float2 unit_y() noexcept { return float2(0.0f, 1.0f); }

        // ============================================================================
        // Mathematical Functions
        // ============================================================================

        /**
         * @brief Compute Euclidean length (magnitude)
         * @return Length of the vector
         */
        float length() const noexcept;

        /**
         * @brief Compute squared length (faster, useful for comparisons)
         * @return Squared length of the vector
         */
        constexpr float length_sq() const noexcept { return x * x + y * y; }

        /**
         * @brief Normalize vector to unit length
         * @return Normalized vector
         * @note Returns zero vector if length is zero
         */
        float2 normalize() const noexcept;

        /**
         * @brief Compute dot product with another vector
         * @param other Other vector
         * @return Dot product result
         */
        float dot(const float2& other) const noexcept;

        /**
         * @brief Compute 2D cross product (scalar result)
         * @param other Other vector
         * @return Cross product scalar (x1*y2 - y1*x2)
         * @note 2D cross product returns a scalar representing the signed area
         */
        float cross(const float2& other) const;

        /**
         * @brief Compute distance to another point
         * @param other Other point
         * @return Euclidean distance
         */
        float distance(const float2& other) const noexcept;

        /**
         * @brief Compute squared distance to another point (faster)
         * @param other Other point
         * @return Squared Euclidean distance
         */
        constexpr float distance_sq(const float2& other) const noexcept {
            float dx = x - other.x; float dy = y - other.y; return dx * dx + dy * dy;
        }

        // ============================================================================
        // HLSL-like Functions
        // ============================================================================

        /**
         * @brief HLSL-like abs function (component-wise absolute value)
         * @return Vector with absolute values of components
         */
        float2 abs() const noexcept;

        /**
         * @brief HLSL-like sign function (component-wise sign)
         * @return Vector with signs of components (-1, 0, or 1)
         */
        float2 sign() const noexcept;

        /**
         * @brief HLSL-like floor function (component-wise floor)
         * @return Vector with floored components
         */
        float2 floor() const noexcept;

        /**
         * @brief HLSL-like ceil function (component-wise ceiling)
         * @return Vector with ceiling components
         */
        float2 ceil() const noexcept;

        /**
         * @brief HLSL-like round function (component-wise rounding)
         * @return Vector with rounded components
         */
        float2 round() const noexcept;

        /**
         * @brief HLSL-like frac function (component-wise fractional part)
         * @return Vector with fractional parts of components
         */
        float2 frac() const noexcept;

        /**
         * @brief HLSL-like saturate function (clamp components to [0, 1])
         * @return Saturated vector
         */
        float2 saturate() const noexcept;

        /**
         * @brief HLSL-like step function (component-wise step)
         * @param edge Edge value
         * @return 1.0 if component >= edge, else 0.0
         */
        float2 step(float edge) const noexcept;

        /**
         * @brief HLSL-like smoothstep function (smooth interpolation)
         * @param edge0 Lower edge
         * @param edge1 Upper edge
         * @return Smoothly interpolated values between 0 and 1
         */
        float2 smoothstep(float edge0, float edge1) const noexcept;

        // ============================================================================
        // Geometric Operations
        // ============================================================================

        /**
         * @brief Get perpendicular vector (90-degree rotation)
         * @return Perpendicular vector (-y, x)
         */
        constexpr float2 perpendicular() const noexcept { return float2(-y, x); }

        /**
         * @brief Compute reflection vector
         * @param normal Surface normal (must be normalized)
         * @return Reflected vector
         */
        float2 reflect(const float2& normal) const noexcept;

        /**
         * @brief Compute refraction vector
         * @param normal Surface normal (must be normalized)
         * @param eta Ratio of indices of refraction
         * @return Refracted vector
         */
        float2 refract(const float2& normal, float eta) const noexcept;

        /**
         * @brief Rotate vector by angle
         * @param angle Rotation angle in radians
         * @return Rotated vector
         */
        float2 rotate(float angle) const noexcept;

        /**
         * @brief Get angle of vector in radians
         * @return Angle from positive x-axis in range [-π, π]
         */
        float angle() const noexcept;

        // ============================================================================
        // Swizzle Operations (HLSL style)
        // ============================================================================

        /**
         * @brief Get YX components as float2 (swapped)
         * @return 2D vector with y and x components
         */
        constexpr float2 yx() const noexcept { return float2(y, x); }

        /**
         * @brief Get XX components as float2
         * @return 2D vector with x and x components
         */
        constexpr float2 xx() const noexcept { return float2(x, x); }

        /**
         * @brief Get YY components as float2
         * @return 2D vector with y and y components
         */
        constexpr float2 yy() const noexcept { return float2(y, y); }

        // ============================================================================
        // Utility Methods
        // ============================================================================

        /**
         * @brief Check if vector contains finite values
         * @return True if all components are finite (not NaN or infinity)
         */
        bool isValid() const noexcept;

        /**
         * @brief Check if vector is approximately equal to another
         * @param other Vector to compare with
         * @param epsilon Comparison tolerance
         * @return True if vectors are approximately equal
         */
        bool approximately(const float2& other, float epsilon = EPSILON) const noexcept;

        /**
         * @brief Check if vector is approximately zero
         * @param epsilon Comparison tolerance
         * @return True if vector length is approximately zero
         */
        bool approximately_zero(float epsilon = EPSILON) const noexcept;

        /**
         * @brief Check if vector is normalized
         * @param epsilon Comparison tolerance
         * @return True if vector length is approximately 1.0
         */
        bool is_normalized(float epsilon = EPSILON) const noexcept;

        /**
         * @brief Convert to string representation
         * @return String in format "(x, y)"
         */
        std::string to_string() const;

        /**
         * @brief Get pointer to raw data
         * @return Pointer to first component
         */
        const float* data() const noexcept;

        /**
         * @brief Get pointer to raw data (mutable)
         * @return Pointer to first component
         */
        float* data() noexcept;

        // ============================================================================
        // Comparison Operators
        // ============================================================================

        /**
         * @brief Equality comparison (approximate)
         * @param rhs Right-hand side vector
         * @return True if vectors are approximately equal
         */
        bool operator==(const float2& rhs) const noexcept;

        /**
         * @brief Inequality comparison
         * @param rhs Right-hand side vector
         * @return True if vectors are not approximately equal
         */
        bool operator!=(const float2& rhs) const noexcept;
    };

    // ============================================================================
    // Global Operators
    // ============================================================================

    /**
     * @brief Component-wise vector multiplication
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of multiplication
     */
    inline float2 operator*(float2 lhs, const float2& rhs) noexcept;

    /**
     * @brief Component-wise vector division
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of division
     */
    inline float2 operator/(float2 lhs, const float2& rhs) noexcept;

    /**
     * @brief Vector-scalar multiplication
     * @param vec Vector to multiply
     * @param scalar Scalar multiplier
     * @return Scaled vector
     */
    inline float2 operator*(float2 vec, float scalar) noexcept;

    /**
     * @brief Scalar-vector multiplication
     * @param scalar Scalar multiplier
     * @param vec Vector to multiply
     * @return Scaled vector
     */
    inline float2 operator*(float scalar, float2 vec) noexcept;

    /**
     * @brief Vector-scalar division
     * @param vec Vector to divide
     * @param scalar Scalar divisor
     * @return Scaled vector
     */
    inline float2 operator/(float2 vec, float scalar) noexcept;

    /**
     * @brief Scalar-vector addition
     * @param scalar Scalar to add
     * @param vec Vector to add to
     * @return Result vector
     */
    inline float2 operator+(float scalar, float2 vec) noexcept;

    // ============================================================================
    // Global Mathematical Functions
    // ============================================================================

    /**
     * @brief Compute distance between two points
     * @param a First point
     * @param b Second point
     * @return Euclidean distance between points
     */
    float distance(const float2& a, const float2& b) noexcept;

    /**
     * @brief Compute squared distance between two points (faster)
     * @param a First point
     * @param b Second point
     * @return Squared Euclidean distance
     */
    float distance_sq(const float2& a, const float2& b) noexcept;

    /**
     * @brief Compute dot product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return Dot product result
     */
    float dot(const float2& a, const float2& b) noexcept;

    /**
     * @brief Compute 2D cross product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return Cross product scalar
     */
    float cross(const float2& a, const float2& b) noexcept;

    /**
     * @brief Check if two vectors are approximately equal
     * @param a First vector
     * @param b Second vector
     * @param epsilon Comparison tolerance
     * @return True if vectors are approximately equal
     */
    bool approximately(const float2& a, const float2& b, float epsilon) noexcept;

    /**
     * @brief Check if vector contains valid finite values
     * @param vec Vector to check
     * @return True if vector is valid (finite values)
     */
    bool isValid(const float2& vec) noexcept;

    /**
     * @brief Linear interpolation between two vectors
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation factor [0, 1]
     * @return Interpolated vector
     */
    float2 lerp(const float2& a, const float2& b, float t) noexcept;

    /**
     * @brief Spherical linear interpolation (for directions)
     * @param a Start vector (should be normalized)
     * @param b End vector (should be normalized)
     * @param t Interpolation factor [0, 1]
     * @return Interpolated vector
     */
    float2 slerp(const float2& a, const float2& b, float t) noexcept;

    /**
     * @brief Get perpendicular vector
     * @param vec Input vector
     * @return Perpendicular vector (-y, x)
     */
    float2 perpendicular(const float2& vec) noexcept;

    /**
     * @brief Compute reflection vector
     * @param incident Incident vector
     * @param normal Surface normal (must be normalized)
     * @return Reflected vector
     */
    float2 reflect(const float2& incident, const float2& normal) noexcept;

    /**
     * @brief Compute refraction vector
     * @param incident Incident vector
     * @param normal Surface normal (must be normalized)
     * @param eta Ratio of indices of refraction
     * @return Refracted vector
     */
    float2 refract(const float2& incident, const float2& normal, float eta) noexcept;

    /**
     * @brief Rotate vector by angle
     * @param vec Vector to rotate
     * @param angle Rotation angle in radians
     * @return Rotated vector
     */
    float2 rotate(const float2& vec, float angle) noexcept;

    /**
     * @brief Compute angle between two vectors in radians
     * @param a First vector
     * @param b Second vector
     * @return Angle in radians between [0, π]
     */
    float angle_between(const float2& a, const float2& b) noexcept;

    /**
     * @brief Compute signed angle between two vectors
     * @param from Starting vector
     * @param to Target vector
     * @return Signed angle in radians between [-π, π]
     */
    float signed_angle_between(const float2& from, const float2& to) noexcept;

    /**
     * @brief Project vector onto another vector
     * @param vec Vector to project
     * @param onto Vector to project onto
     * @return Projected vector
     */
    float2 project(const float2& vec, const float2& onto) noexcept;

    /**
     * @brief Reject vector from another vector (component perpendicular)
     * @param vec Vector to reject
     * @param from Vector to reject from
     * @return Rejected vector
     */
    float2 reject(const float2& vec, const float2& from) noexcept;

    // ============================================================================
    // HLSL-like Global Functions
    // ============================================================================

    /**
     * @brief HLSL-like abs function (component-wise absolute value)
     * @param vec Input vector
     * @return Vector with absolute values of components
     */
    float2 abs(const float2& vec) noexcept;

    /**
     * @brief HLSL-like sign function (component-wise sign)
     * @param vec Input vector
     * @return Vector with signs of components
     */
    float2 sign(const float2& vec) noexcept;

    /**
     * @brief HLSL-like floor function (component-wise floor)
     * @param vec Input vector
     * @return Vector with floored components
     */
    float2 floor(const float2& vec) noexcept;

    /**
     * @brief HLSL-like ceil function (component-wise ceiling)
     * @param vec Input vector
     * @return Vector with ceiling components
     */
    float2 ceil(const float2& vec) noexcept;

    /**
     * @brief HLSL-like round function (component-wise rounding)
     * @param vec Input vector
     * @return Vector with rounded components
     */
    float2 round(const float2& vec) noexcept;

    /**
     * @brief HLSL-like frac function (component-wise fractional part)
     * @param vec Input vector
     * @return Vector with fractional parts of components
     */
    float2 frac(const float2& vec) noexcept;

    /**
     * @brief HLSL-like saturate function (clamp components to [0, 1])
     * @param vec Input vector
     * @return Saturated vector
     */
    float2 saturate(const float2& vec) noexcept;

    /**
     * @brief HLSL-like step function (component-wise step)
     * @param edge Edge value
     * @param vec Input vector
     * @return Step result vector
     */
    float2 step(float edge, const float2& vec) noexcept;

    /**
     * @brief HLSL-like smoothstep function (smooth interpolation)
     * @param edge0 Lower edge
     * @param edge1 Upper edge
     * @param vec Input vector
     * @return Smoothly interpolated values
     */
    float2 smoothstep(float edge0, float edge1, const float2& vec) noexcept;

    /**
     * @brief HLSL-like clamp function (component-wise clamping)
     * @param vec Vector to clamp
     * @param min_val Minimum values
     * @param max_val Maximum values
     * @return Clamped vector
     */
    float2 clamp(const float2& vec, const float2& min_val, const float2& max_val) noexcept;

    /**
     * @brief HLSL-like min function (component-wise minimum)
     * @param a First vector
     * @param b Second vector
     * @return Component-wise minimum
     */
    float2 min(const float2& a, const float2& b) noexcept;

    /**
     * @brief HLSL-like max function (component-wise maximum)
     * @param a First vector
     * @param b Second vector
     * @return Component-wise maximum
     */
    float2 max(const float2& a, const float2& b) noexcept;

    // ============================================================================
    // D3D Compatibility Functions
    // ============================================================================

#if defined(MATH_SUPPORT_D3DX)

    /**
     * @brief Convert float2 to D3DXVECTOR2
     * @param vec float2 vector to convert
     * @return D3DXVECTOR2 equivalent
     */
    D3DXVECTOR2 ToD3DXVECTOR2(const float2& vec) noexcept;

    /**
     * @brief Convert D3DXVECTOR2 to float2
     * @param vec D3DXVECTOR2 to convert
     * @return float2 equivalent
     */
    float2 FromD3DXVECTOR2(const D3DXVECTOR2& vec) noexcept;

    /**
     * @brief Convert float2 to D3DCOLOR (uses x,y as R,G channels)
     * @param color float2 representing color (x=R, y=G)
     * @return D3DCOLOR equivalent (blue=0, alpha=255)
     */
    D3DCOLOR ToD3DCOLOR(const float2& color) noexcept;

    /**
     * @brief Convert array of float2 to array of D3DXVECTOR2
     * @param source Source float2 array
     * @param destination Destination D3DXVECTOR2 array
     * @param count Number of elements to convert
     */
    void float2ArrayToD3D(const float2* source, D3DXVECTOR2* destination, size_t count) noexcept;

    /**
     * @brief Convert array of D3DXVECTOR2 to array of float2
     * @param source Source D3DXVECTOR2 array
     * @param destination Destination float2 array
     * @param count Number of elements to convert
     */
    void D3DArrayTofloat2(const D3DXVECTOR2* source, float2* destination, size_t count) noexcept;

#endif

    // ============================================================================
    // Utility Functions
    // ============================================================================

    /**
     * @brief Compute distance from point to line segment
     * @param point Test point
     * @param line_start Start point of line segment
     * @param line_end End point of line segment
     * @return Minimum distance from point to line segment
     */
    float distance_to_line_segment(const float2& point, const float2& line_start, const float2& line_end) noexcept;

    /**
     * @brief Check if point is inside triangle
     * @param point Test point
     * @param a First triangle vertex
     * @param b Second triangle vertex
     * @param c Third triangle vertex
     * @return True if point is inside triangle (including edges)
     */
    bool point_in_triangle(const float2& point, const float2& a, const float2& b, const float2& c) noexcept;

    // ============================================================================
    // Useful Constants
    // ============================================================================

    /**
     * @brief Zero vector constant (0, 0)
     */
    extern const float2 float2_Zero;

    /**
     * @brief One vector constant (1, 1)
     */
    extern const float2 float2_One;

    /**
     * @brief Unit X vector constant (1, 0)
     */
    extern const float2 float2_UnitX;

    /**
     * @brief Unit Y vector constant (0, 1)
     */
    extern const float2 float2_UnitY;

    /**
     * @brief Right vector constant (1, 0)
     */
    extern const float2 float2_Right;

    /**
     * @brief Left vector constant (-1, 0)
     */
    extern const float2 float2_Left;

    /**
     * @brief Up vector constant (0, 1)
     */
    extern const float2 float2_Up;

    /**
     * @brief Down vector constant (0, -1)
     */
    extern const float2 float2_Down;

} // namespace Math

// Include Implementation at the end
#include "math_float2.inl"
