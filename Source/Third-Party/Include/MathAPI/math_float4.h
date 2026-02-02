// Description: 4-dimensional vector class with comprehensive 
//              mathematical operations and SSE optimization
//              Supports both 4D vectors and homogeneous coordinates
// Author: NSDeathman, DeepSeek
#pragma once

/**
 * @file math_float4.h
 * @brief 4-dimensional vector with comprehensive mathematical operations and SSE optimization
 * @note Supports both 4D vectors and homogeneous coordinates, includes color operations
 * @note Perfect for 4D graphics, homogeneous coordinates, RGBA colors, and quaternions
 */

#include <xmmintrin.h>
#include <pmmintrin.h> // SSE3
#include <smmintrin.h> // SSE4.1
#include <string>
#include <cstdio>
#include <cmath>
#include <algorithm>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"
#include "math_float2.h"
#include "math_float3.h"

///////////////////////////////////////////////////////////////
namespace Math
{
    // ============================================================================
    // Fwd Declaration
    // ============================================================================

    class float4;

    /**
     * @brief Vector addition operator
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of addition
     */
    inline float4 operator+(float4 lhs, const float4& rhs) noexcept;

    /**
     * @brief Vector subtraction operator
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of subtraction
     */
    inline float4 operator-(float4 lhs, const float4& rhs) noexcept;

    /**
     * @brief Component-wise vector multiplication operator
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of multiplication
     */
    inline float4 operator*(float4 lhs, const float4& rhs) noexcept;

    /**
     * @brief Component-wise vector division operator
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of division
     */
    inline float4 operator/(float4 lhs, const float4& rhs) noexcept;

    /**
     * @brief Vector-scalar multiplication operator
     * @param vec Vector to multiply
     * @param scalar Scalar multiplier
     * @return Scaled vector
     */
    inline float4 operator*(float4 vec, float scalar) noexcept;

    /**
     * @brief Scalar-vector multiplication operator
     * @param scalar Scalar multiplier
     * @param vec Vector to multiply
     * @return Scaled vector
     */
    inline float4 operator*(float scalar, float4 vec) noexcept;

    /**
     * @brief Vector-scalar division operator
     * @param vec Vector to divide
     * @param scalar Scalar divisor
     * @return Scaled vector
     */
    inline float4 operator/(float4 vec, float scalar) noexcept;

    /**
     * @class float4
     * @brief 4-dimensional vector with comprehensive mathematical operations
     *
     * Represents a 4D vector (x, y, z, w) with optimized operations for 4D graphics,
     * homogeneous coordinates, colors with alpha, and 4D mathematics.
     * Features SSE optimization for performance-critical operations.
     *
     * @note Perfect for 4D graphics, homogeneous coordinates, RGBA colors, and quaternions
     * @note All operations are optimized and constexpr where possible
     * @note Includes comprehensive HLSL-like function set and color operations
     * @note Supports both aligned and unaligned memory operations
     */
    class float4
    {
    public:
        // ============================================================================
        // Data Members (Public for Direct Access)
        // ============================================================================

        union {
            struct {
                float x; ///< X component of the vector
                float y; ///< Y component of the vector  
                float z; ///< Z component of the vector
                float w; ///< W component of the vector (homogeneous coordinate or alpha)
            };
            __m128 simd_; ///< SSE register for optimized operations
        };

        // ============================================================================
        // Constructors
        // ============================================================================

        /**
         * @brief Default constructor (initializes to zero vector)
         * @note Uses SSE optimized initialization
         */
        float4() noexcept;

        /**
         * @brief Construct from components
         * @param x X component
         * @param y Y component
         * @param z Z component
         * @param w W component
         * @note Components are stored in SSE register for optimal performance
         */
        float4(float x, float y, float z, float w) noexcept;

        /**
         * @brief Construct from scalar (all components set to same value)
         * @param scalar Value for all components
         * @note Useful for creating uniform vectors
         */
        explicit float4(float scalar) noexcept;

        /**
         * @brief Construct from float2 and z, w components
         * @param vec 2D vector for x and y components
         * @param z Z component (default: 0.0f)
         * @param w W component (default: 0.0f)
         * @note Convenient for extending 2D vectors to 4D
         */
        float4(const float2& vec, float z = 0.0f, float w = 0.0f) noexcept;

        /**
         * @brief Construct from float3 and w component
         * @param vec 3D vector for x, y, z components
         * @param w W component (default: 0.0f)
         * @note Useful for homogeneous coordinates and colors with alpha
         */
        float4(const float3& vec, float w = 0.0f) noexcept;

        /**
         * @brief Copy constructor
         * @note Default implementation is efficient for SSE types
         */
        float4(const float4&) noexcept = default;

        /**
         * @brief Construct from raw float array
         * @param data Pointer to float array [x, y, z, w]
         * @note Uses unaligned load for maximum compatibility
         */
        explicit float4(const float* data) noexcept;

        /**
         * @brief Construct from SSE register (advanced users)
         * @param simd_val SSE register containing vector data
         * @note For advanced SSE optimization scenarios
         */
        explicit float4(__m128 simd_val) noexcept;

        // ============================================================================
        // Assignment Operators
        // ============================================================================

        /**
         * @brief Copy assignment operator
         * @note Default implementation is efficient for SSE types
         */
        float4& operator=(const float4&) noexcept = default;

        /**
         * @brief Scalar assignment (sets all components to same value)
         * @param scalar Value for all components
         * @return Reference to this vector
         */
        float4& operator=(float scalar) noexcept;

        /**
         * @brief Assignment from float3 (preserves w component)
         * @param xyz 3D vector for x, y, z components
         * @return Reference to this vector
         * @note Preserves existing w component value
         */
        float4& operator=(const float3& xyz) noexcept;

        // ============================================================================
        // Compound Assignment Operators
        // ============================================================================

        /**
         * @brief Add and assign operator
         * @param rhs Vector to add
         * @return Reference to this vector
         * @note SSE optimized implementation
         */
        float4& operator+=(const float4& rhs) noexcept;

        /**
         * @brief Subtract and assign operator
         * @param rhs Vector to subtract
         * @return Reference to this vector
         * @note SSE optimized implementation
         */
        float4& operator-=(const float4& rhs) noexcept;

        /**
         * @brief Component-wise multiply and assign operator
         * @param rhs Vector to multiply by
         * @return Reference to this vector
         * @note SSE optimized implementation
         */
        float4& operator*=(const float4& rhs) noexcept;

        /**
         * @brief Component-wise divide and assign operator
         * @param rhs Vector to divide by
         * @return Reference to this vector
         * @note SSE optimized implementation
         */
        float4& operator/=(const float4& rhs) noexcept;

        /**
         * @brief Scalar multiply and assign operator
         * @param scalar Scalar to multiply by
         * @return Reference to this vector
         * @note SSE optimized implementation
         */
        float4& operator*=(float scalar) noexcept;

        /**
         * @brief Scalar divide and assign operator
         * @param scalar Scalar to divide by
         * @return Reference to this vector
         * @note SSE optimized implementation
         */
        float4& operator/=(float scalar) noexcept;

        // ============================================================================
        // Unary Operators
        // ============================================================================

        /**
         * @brief Unary plus operator
         * @return Copy of this vector
         */
        float4 operator+() const noexcept;

        /**
         * @brief Unary minus operator (negation)
         * @return Negated vector
         * @note SSE optimized implementation
         */
        float4 operator-() const noexcept;

        // ============================================================================
        // Access Operators
        // ============================================================================

        /**
         * @brief Access component by index
         * @param index Component index (0 = x, 1 = y, 2 = z, 3 = w)
         * @return Reference to component
         * @warning No bounds checking - use with valid indices 0-3
         */
        float& operator[](int index) noexcept;

        /**
         * @brief Access component by index (const)
         * @param index Component index (0 = x, 1 = y, 2 = z, 3 = w)
         * @return Const reference to component
         * @warning No bounds checking - use with valid indices 0-3
         */
        const float& operator[](int index) const noexcept;

        // ============================================================================
        // Conversion Operators
        // ============================================================================

        /**
         * @brief Convert to const float pointer (for interoperability)
         * @return Pointer to first component
         * @note Useful for passing to C-style APIs
         */
        operator const float* () const noexcept;

        /**
         * @brief Convert to float pointer (for interoperability)
         * @return Pointer to first component
         * @note Useful for modifying data in C-style APIs
         */
        operator float* () noexcept;

        /**
         * @brief Convert to SSE register (advanced users)
         * @return SSE register containing vector data
         * @note For advanced SSE optimization scenarios
         */
        operator __m128() const noexcept;

        // ============================================================================
        // Static Constructors
        // ============================================================================

        /**
         * @brief Zero vector (0, 0, 0, 0)
         * @return Zero vector
         */
        static float4 zero() noexcept;

        /**
         * @brief One vector (1, 1, 1, 1)
         * @return One vector
         */
        static float4 one() noexcept;

        /**
         * @brief Unit X vector (1, 0, 0, 0)
         * @return Unit vector along X axis
         */
        static float4 unit_x() noexcept;

        /**
         * @brief Unit Y vector (0, 1, 0, 0)
         * @return Unit vector along Y axis
         */
        static float4 unit_y() noexcept;

        /**
         * @brief Unit Z vector (0, 0, 1, 0)
         * @return Unit vector along Z axis
         */
        static float4 unit_z() noexcept;

        /**
         * @brief Unit W vector (0, 0, 0, 1)
         * @return Unit vector along W axis
         */
        static float4 unit_w() noexcept;

        /**
         * @brief Create vector from RGBA color values [0-255]
         * @param r Red component [0-255]
         * @param g Green component [0-255]
         * @param b Blue component [0-255]
         * @param a Alpha component [0-255] (default: 255)
         * @return Color vector with components normalized to [0,1]
         * @note Automatically converts from 8-bit to float format
         */
        static float4 from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255) noexcept;

        /**
         * @brief Create vector from float values with automatic normalization
         * @param r Red component [0.0-1.0]
         * @param g Green component [0.0-1.0]
         * @param b Blue component [0.0-1.0]
         * @param a Alpha component [0.0-1.0] (default: 1.0)
         * @return Color vector
         * @note Components should be in [0,1] range for correct color representation
         */
        static float4 from_color(float r, float g, float b, float a = 1.0f) noexcept;

        // ============================================================================
        // Mathematical Functions
        // ============================================================================

        /**
         * @brief Compute Euclidean length (magnitude)
         * @return Length of the vector
         * @note SSE optimized implementation using horizontal addition
         */
        float length() const noexcept;

        /**
         * @brief Compute squared length (faster, useful for comparisons)
         * @return Squared length of the vector
         * @note Faster than length() for distance comparisons
         */
        float length_sq() const noexcept;

        /**
         * @brief Normalize vector to unit length
         * @return Normalized vector
         * @note Returns zero vector if length is zero
         * @note SSE optimized implementation
         */
        float4 normalize() const noexcept;

        /**
         * @brief Compute dot product with another vector
         * @param other Other vector
         * @return Dot product result
         * @note Includes all 4 components in calculation
         */
        float dot(const float4& other) const noexcept;

        /**
         * @brief Compute 3D dot product (ignores w component)
         * @param other Other vector
         * @return 3D dot product result
         * @note Useful for 3D operations in homogeneous coordinates
         */
        float dot3(const float4& other) const noexcept;

        /**
         * @brief Compute 3D cross product (ignores w component)
         * @param other Other vector
         * @return 3D cross product result (w = 0)
         * @note Resulting w component is set to 0
         * @note SSE optimized implementation using shuffles
         */
        float4 cross(const float4& other) const noexcept;

        /**
         * @brief Compute distance to another point
         * @param other Other point
         * @return Euclidean distance
         * @note Treats vectors as points in 4D space
         */
        float distance(const float4& other) const noexcept;

        /**
         * @brief Compute squared distance to another point (faster)
         * @param other Other point
         * @return Squared Euclidean distance
         * @note Faster than distance() for distance comparisons
         */
        float distance_sq(const float4& other) const noexcept;

        // ============================================================================
        // HLSL-like Functions
        // ============================================================================

        /**
         * @brief HLSL-like abs function (component-wise absolute value)
         * @return Vector with absolute values of components
         * @note SSE optimized using bitwise operations
         */
        float4 abs() const noexcept;

        /**
         * @brief HLSL-like sign function (component-wise sign)
         * @return Vector with signs of components (-1, 0, or 1)
         * @note SSE optimized implementation
         */
        float4 sign() const noexcept;

        /**
         * @brief HLSL-like floor function (component-wise floor)
         * @return Vector with floored components
         * @note Uses SSE4.1 instruction when available, fallback to std::floor
         */
        float4 floor() const noexcept;

        /**
         * @brief HLSL-like ceil function (component-wise ceiling)
         * @return Vector with ceiling components
         * @note Uses SSE4.1 instruction when available, fallback to std::ceil
         */
        float4 ceil() const noexcept;

        /**
         * @brief HLSL-like round function (component-wise rounding)
         * @return Vector with rounded components
         * @note Uses SSE4.1 instruction when available, fallback to std::round
         */
        float4 round() const noexcept;

        /**
         * @brief HLSL-like frac function (component-wise fractional part)
         * @return Vector with fractional parts of components
         * @note Computes component - floor(component) for each component
         */
        float4 frac() const noexcept;

        /**
         * @brief HLSL-like saturate function (clamp components to [0, 1])
         * @return Saturated vector
         * @note SSE optimized implementation
         */
        float4 saturate() const noexcept;

        /**
         * @brief HLSL-like step function (component-wise step)
         * @param edge Edge value
         * @return 1.0 if component >= edge, else 0.0
         * @note SSE optimized implementation
         */
        float4 step(float edge) const noexcept;

        // ============================================================================
        // Color Operations
        // ============================================================================

        /**
         * @brief Compute luminance (grayscale value) using standard weights
         * @return Luminance value
         * @note Uses Rec. 709 weights: 0.2126*R + 0.7152*G + 0.0722*B
         * @note Standard weights for HD television and sRGB colorspace
         */
        float luminance() const noexcept;

        /**
         * @brief Compute average brightness (simple average of RGB)
         * @return Brightness value
         * @note Simple average: (R + G + B) / 3
         */
        float brightness() const noexcept;

        /**
         * @brief Premultiply RGB components by alpha
         * @return Premultiplied color
         * @note Useful for alpha blending operations
         * @note Preserves original alpha value
         */
        float4 premultiply_alpha() const noexcept;

        /**
         * @brief Unpremultiply RGB components (divide by alpha)
         * @return Unpremultiplied color
         * @note Returns original color if alpha is zero
         * @note Inverse operation of premultiply_alpha()
         */
        float4 unpremultiply_alpha() const noexcept;

        /**
         * @brief Convert to grayscale using luminance
         * @return Grayscale color (RGB = luminance, alpha preserved)
         * @note Uses standard luminance weights for accurate grayscale conversion
         */
        float4 grayscale() const noexcept;

        // ============================================================================
        // Geometric Operations
        // ============================================================================

        /**
         * @brief Project 4D homogeneous coordinates to 3D
         * @return 3D projected coordinates (x/w, y/w, z/w)
         * @note Returns zero vector if w is zero
         * @note Standard homogeneous coordinate projection
         */
        float3 project() const noexcept;

        /**
         * @brief Transform to homogeneous coordinates (set w = 1)
         * @return Homogeneous coordinates (x, y, z, 1)
         * @note Useful for converting 3D points to homogeneous coordinates
         */
        float4 to_homogeneous() const noexcept;

        // ============================================================================
        // Static Mathematical Functions
        // ============================================================================

        /**
         * @brief Compute dot product of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Dot product result
         * @note SSE optimized implementation
         */
        static float dot(const float4& a, const float4& b) noexcept;

        /**
         * @brief Compute 3D dot product (ignores w component)
         * @param a First vector
         * @param b Second vector
         * @return 3D dot product result
         * @note Useful for 3D operations in homogeneous coordinates
         */
        static float dot3(const float4& a, const float4& b) noexcept;

        /**
         * @brief Compute 3D cross product (ignores w component)
         * @param a First vector
         * @param b Second vector
         * @return 3D cross product result (w = 0)
         * @note SSE optimized implementation using shuffles
         */
        static float4 cross(const float4& a, const float4& b) noexcept;

        /**
         * @brief Linear interpolation between two vectors
         * @param a Start vector
         * @param b End vector
         * @param t Interpolation factor [0, 1]
         * @return Interpolated vector
         * @note SSE optimized implementation
         */
        static float4 lerp(const float4& a, const float4& b, float t) noexcept;

        /**
         * @brief Component-wise minimum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise minimum
         * @note SSE optimized implementation
         */
        static float4 min(const float4& a, const float4& b) noexcept;

        /**
         * @brief Component-wise maximum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise maximum
         * @note SSE optimized implementation
         */
        static float4 max(const float4& a, const float4& b) noexcept;

        /**
         * @brief HLSL-like saturate function (clamp components to [0, 1])
         * @param vec Vector to saturate
         * @return Saturated vector
         * @note SSE optimized implementation
         */
        static float4 saturate(const float4& vec) noexcept;

        // ============================================================================
        // Swizzle Operations (HLSL style)
        // ============================================================================

        /**
         * @brief Swizzle to (x, y)
         * @return 2D vector with x and y components
         */
        float2 xy() const noexcept;

        /**
         * @brief Swizzle to (x, z)
         * @return 2D vector with x and z components
         */
        float2 xz() const noexcept;

        /**
         * @brief Swizzle to (x, w)
         * @return 2D vector with x and w components
         */
        float2 xw() const noexcept;

        /**
         * @brief Swizzle to (y, z)
         * @return 2D vector with y and z components
         */
        float2 yz() const noexcept;

        /**
         * @brief Swizzle to (y, w)
         * @return 2D vector with y and w components
         */
        float2 yw() const noexcept;

        /**
         * @brief Swizzle to (z, w)
         * @return 2D vector with z and w components
         */
        float2 zw() const noexcept;

        /**
         * @brief Swizzle to (x, y, z)
         * @return 3D vector with x, y, z components
         */
        float3 xyz() const noexcept;

        /**
         * @brief Swizzle to (x, y, w)
         * @return 3D vector with x, y, w components
         */
        float3 xyw() const noexcept;

        /**
         * @brief Swizzle to (x, z, w)
         * @return 3D vector with x, z, w components
         */
        float3 xzw() const noexcept;

        /**
         * @brief Swizzle to (y, z, w)
         * @return 3D vector with y, z, w components
         */
        float3 yzw() const noexcept;

        /**
         * @brief Swizzle to (y, x, z, w)
         * @return 4D vector with components swapped
         */
        float4 yxzw() const noexcept;

        /**
         * @brief Swizzle to (z, x, y, w)
         * @return 4D vector with components rotated
         */
        float4 zxyw() const noexcept;

        /**
         * @brief Swizzle to (z, y, x, w)
         * @return 4D vector with components reversed in XYZ
         */
        float4 zyxw() const noexcept;

        /**
         * @brief Swizzle to (w, z, y, x)
         * @return 4D vector with components fully reversed
         */
        float4 wzyx() const noexcept;

        // Color swizzles
        /**
         * @brief Get red component (alias for x)
         * @return Red component
         */
        float r() const noexcept;

        /**
         * @brief Get green component (alias for y)
         * @return Green component
         */
        float g() const noexcept;

        /**
         * @brief Get blue component (alias for z)
         * @return Blue component
         */
        float b() const noexcept;

        /**
         * @brief Get alpha component (alias for w)
         * @return Alpha component
         */
        float a() const noexcept;

        /**
         * @brief Swizzle to (r, g) - red and green components
         * @return 2D vector with red and green components
         */
        float2 rg() const noexcept;

        /**
         * @brief Swizzle to (r, b) - red and blue components
         * @return 2D vector with red and blue components
         */
        float2 rb() const noexcept;

        /**
         * @brief Swizzle to (r, a) - red and alpha components
         * @return 2D vector with red and alpha components
         */
        float2 ra() const noexcept;

        /**
         * @brief Swizzle to (g, b) - green and blue components
         * @return 2D vector with green and blue components
         */
        float2 gb() const noexcept;

        /**
         * @brief Swizzle to (g, a) - green and alpha components
         * @return 2D vector with green and alpha components
         */
        float2 ga() const noexcept;

        /**
         * @brief Swizzle to (b, a) - blue and alpha components
         * @return 2D vector with blue and alpha components
         */
        float2 ba() const noexcept;

        /**
         * @brief Swizzle to (r, g, b) - RGB color
         * @return 3D vector with RGB components
         */
        float3 rgb() const noexcept;

        /**
         * @brief Swizzle to (r, g, a) - red, green, alpha components
         * @return 3D vector with RGA components
         */
        float3 rga() const noexcept;

        /**
         * @brief Swizzle to (r, b, a) - red, blue, alpha components
         * @return 3D vector with RBA components
         */
        float3 rba() const noexcept;

        /**
         * @brief Swizzle to (g, b, a) - green, blue, alpha components
         * @return 3D vector with GBA components
         */
        float3 gba() const noexcept;

        /**
         * @brief Swizzle to (g, r, b, a) - green, red, blue, alpha
         * @return 4D vector with GRBA components
         */
        float4 grba() const noexcept;

        /**
         * @brief Swizzle to (b, r, g, a) - blue, red, green, alpha
         * @return 4D vector with BRGA components
         */
        float4 brga() const noexcept;

        /**
         * @brief Swizzle to (b, g, r, a) - blue, green, red, alpha (BGR format)
         * @return 4D vector with BGRA components
         */
        float4 bgra() const noexcept;

        /**
         * @brief Swizzle to (a, b, g, r) - alpha, blue, green, red
         * @return 4D vector with ABGR components
         */
        float4 abgr() const noexcept;

        // ============================================================================
        // Utility Methods
        // ============================================================================

        /**
         * @brief Check if vector contains finite values
         * @return True if all components are finite (not NaN or infinity)
         * @note SSE optimized implementation
         */
        bool isValid() const noexcept;

        /**
         * @brief Check if vector is approximately equal to another
         * @param other Vector to compare with
         * @param epsilon Comparison tolerance (default: EPSILON)
         * @return True if vectors are approximately equal
         * @note SSE optimized implementation
         */
        bool approximately(const float4& other, float epsilon = EPSILON) const noexcept;

        /**
         * @brief Check if vector is approximately zero
         * @param epsilon Comparison tolerance (default: EPSILON)
         * @return True if vector length is approximately zero
         */
        bool approximately_zero(float epsilon = EPSILON) const noexcept;

        /**
         * @brief Check if vector is normalized
         * @param epsilon Comparison tolerance (default: EPSILON)
         * @return True if vector length is approximately 1.0
         */
        bool is_normalized(float epsilon = EPSILON) const noexcept;

        /**
         * @brief Convert to string representation
         * @return String in format "(x, y, z, w)"
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

        /**
         * @brief Set x, y, z components from float3
         * @param xyz 3D vector for x, y, z components
         * @note Preserves existing w component
         */
        void set_xyz(const float3& xyz) noexcept;

        /**
         * @brief Set x, y components from float2
         * @param xy 2D vector for x, y components
         * @note Preserves existing z and w components
         */
        void set_xy(const float2& xy) noexcept;

        /**
         * @brief Set z, w components from float2
         * @param zw 2D vector for z, w components
         * @note Preserves existing x and y components
         */
        void set_zw(const float2& zw) noexcept;

        // ============================================================================
        // SSE-specific Methods
        // ============================================================================

        /**
         * @brief Get SSE register (advanced users)
         * @return SSE register containing vector data
         * @note For advanced SSE optimization scenarios
         */
        __m128 get_simd() const noexcept;

        /**
         * @brief Set SSE register (advanced users)
         * @param new_simd SSE register to set
         * @note For advanced SSE optimization scenarios
         */
        void set_simd(__m128 new_simd) noexcept;

        /**
         * @brief Load from unaligned memory
         * @param data Pointer to float data
         * @return Loaded vector
         * @note Uses _mm_loadu_ps for unaligned memory access
         */
        static float4 load_unaligned(const float* data) noexcept;

        /**
         * @brief Load from aligned memory (16-byte aligned)
         * @param data Pointer to float data (must be 16-byte aligned)
         * @return Loaded vector
         * @note Uses _mm_load_ps for aligned memory access
         * @warning data must be 16-byte aligned
         */
        static float4 load_aligned(const float* data) noexcept;

        /**
         * @brief Store to unaligned memory
         * @param data Pointer to destination memory
         * @note Uses _mm_storeu_ps for unaligned memory access
         */
        void store_unaligned(float* data) const noexcept;

        /**
         * @brief Store to aligned memory (16-byte aligned)
         * @param data Pointer to destination memory (must be 16-byte aligned)
         * @note Uses _mm_store_ps for aligned memory access
         * @warning data must be 16-byte aligned
         */
        void store_aligned(float* data) const noexcept;

        // ============================================================================
        // Comparison Operators
        // ============================================================================

        /**
         * @brief Equality comparison with epsilon tolerance
         * @param rhs Vector to compare with
         * @return True if vectors are approximately equal
         */
        bool operator==(const float4& rhs) const noexcept;

        /**
         * @brief Inequality comparison with epsilon tolerance
         * @param rhs Vector to compare with
         * @return True if vectors are not approximately equal
         */
        bool operator!=(const float4& rhs) const noexcept;
    };

    // ============================================================================
    // Binary Operators (Declarations)
    // ============================================================================

    /**
     * @brief Vector addition
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of addition
     */
    inline float4 operator+(float4 lhs, const float4& rhs) noexcept;

    /**
     * @brief Vector subtraction
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of subtraction
     */
    inline float4 operator-(float4 lhs, const float4& rhs) noexcept;

    /**
     * @brief Component-wise vector multiplication
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of multiplication
     */
    inline float4 operator*(float4 lhs, const float4& rhs) noexcept;

    /**
     * @brief Component-wise vector division
     * @param lhs Left-hand side vector
     * @param rhs Right-hand side vector
     * @return Result of division
     */
    inline float4 operator/(float4 lhs, const float4& rhs) noexcept;

    /**
     * @brief Vector-scalar multiplication
     * @param vec Vector to multiply
     * @param scalar Scalar multiplier
     * @return Scaled vector
     */
    inline float4 operator*(float4 vec, float scalar) noexcept;

    /**
     * @brief Scalar-vector multiplication
     * @param scalar Scalar multiplier
     * @param vec Vector to multiply
     * @return Scaled vector
     */
    inline float4 operator*(float scalar, float4 vec) noexcept;

    /**
     * @brief Vector-scalar division
     * @param vec Vector to divide
     * @param scalar Scalar divisor
     * @return Scaled vector
     */
    inline float4 operator/(float4 vec, float scalar) noexcept;

    // ============================================================================
    // Global Mathematical Functions (Declarations)
    // ============================================================================

    /**
     * @brief Compute distance between two points
     * @param a First point
     * @param b Second point
     * @return Euclidean distance between points
     */
    inline float distance(const float4& a, const float4& b) noexcept;

    /**
     * @brief Compute squared distance between two points (faster)
     * @param a First point
     * @param b Second point
     * @return Squared Euclidean distance
     */
    inline float distance_sq(const float4& a, const float4& b) noexcept;

    /**
     * @brief Compute dot product of two vectors
     * @param a First vector
     * @param b Second vector
     * @return Dot product result
     */
    inline float dot(const float4& a, const float4& b) noexcept;

    /**
     * @brief Compute 3D dot product (ignores w component)
     * @param a First vector
     * @param b Second vector
     * @return 3D dot product result
     */
    inline float dot3(const float4& a, const float4& b) noexcept;

    /**
     * @brief Compute 3D cross product (ignores w component)
     * @param a First vector
     * @param b Second vector
     * @return 3D cross product result
     */
    inline float4 cross(const float4& a, const float4& b) noexcept;

    /**
     * @brief Normalize vector to unit length
     * @param vec Vector to normalize
     * @return Normalized vector
     */
    inline float4 normalize(const float4& vec) noexcept;

    /**
     * @brief Linear interpolation between two vectors
     * @param a Start vector
     * @param b End vector
     * @param t Interpolation factor [0, 1]
     * @return Interpolated vector
     */
    inline float4 lerp(const float4& a, const float4& b, float t) noexcept;

    /**
     * @brief HLSL-like saturate function (clamp components to [0, 1])
     * @param vec Vector to saturate
     * @return Saturated vector
     */
    inline float4 saturate(const float4& vec) noexcept;

    /**
     * @brief HLSL-like floor function (component-wise floor)
     * @param vec Vector to floor
     * @return Floored vector
     */
    inline float4 floor(const float4& vec) noexcept;

    /**
     * @brief HLSL-like ceil function (component-wise ceiling)
     * @param vec Vector to ceil
     * @return Ceiling vector
     */
    inline float4 ceil(const float4& vec) noexcept;

    /**
     * @brief HLSL-like round function (component-wise rounding)
     * @param vec Vector to round
     * @return Rounded vector
     */
    inline float4 round(const float4& vec) noexcept;

    /**
     * @brief Check if two vectors are approximately equal
     * @param a First vector
     * @param b Second vector
     * @param epsilon Comparison tolerance
     * @return True if vectors are approximately equal
     */
    inline bool approximately(const float4& a, const float4& b, float epsilon = EPSILON) noexcept;

    /**
     * @brief Check if vector is normalized
     * @param vec Vector to check
     * @param epsilon Comparison tolerance
     * @return True if vector length is approximately 1.0
     */
    inline bool is_normalized(const float4& vec, float epsilon = EPSILON) noexcept;

    /**
     * @brief Check if vector contains valid finite values
     * @param vec Vector to check
     * @return True if vector is valid (finite values)
     */
    inline bool isValid(const float4& vec) noexcept;

    // ============================================================================
    // HLSL-like Global Functions (Declarations)
    // ============================================================================

    /**
     * @brief HLSL-like abs function (component-wise absolute value)
     * @param vec Input vector
     * @return Vector with absolute values of components
     */
    inline float4 abs(const float4& vec) noexcept;

    /**
     * @brief HLSL-like sign function (component-wise sign)
     * @param vec Input vector
     * @return Vector with signs of components
     */
    inline float4 sign(const float4& vec) noexcept;

    /**
     * @brief HLSL-like frac function (component-wise fractional part)
     * @param vec Input vector
     * @return Vector with fractional parts of components
     */
    inline float4 frac(const float4& vec) noexcept;

    /**
     * @brief HLSL-like step function (component-wise step)
     * @param edge Edge value
     * @param vec Input vector
     * @return Step result vector
     */
    inline float4 step(float edge, const float4& vec) noexcept;

    /**
     * @brief HLSL-like min function (component-wise minimum)
     * @param a First vector
     * @param b Second vector
     * @return Component-wise minimum
     */
    inline float4 min(const float4& a, const float4& b) noexcept;

    /**
     * @brief HLSL-like max function (component-wise maximum)
     * @param a First vector
     * @param b Second vector
     * @return Component-wise maximum
     */
    inline float4 max(const float4& a, const float4& b) noexcept;

    /**
     * @brief HLSL-like clamp function (component-wise clamping)
     * @param vec Vector to clamp
     * @param min_val Minimum values
     * @param max_val Maximum values
     * @return Clamped vector
     */
    inline float4 clamp(const float4& vec, const float4& min_val, const float4& max_val) noexcept;

    // ============================================================================
    // Color Operations (Declarations)
    // ============================================================================

    /**
     * @brief Compute luminance (grayscale value) using standard weights
     * @param color Input color
     * @return Luminance value
     */
    inline float luminance(const float4& color) noexcept;

    /**
     * @brief Compute average brightness (simple average of RGB)
     * @param color Input color
     * @return Brightness value
     */
    inline float brightness(const float4& color) noexcept;

    /**
     * @brief Premultiply RGB components by alpha
     * @param color Input color
     * @return Premultiplied color
     */
    inline float4 premultiply_alpha(const float4& color) noexcept;

    /**
     * @brief Unpremultiply RGB components (divide by alpha)
     * @param color Input color
     * @return Unpremultiplied color
     */
    inline float4 unpremultiply_alpha(const float4& color) noexcept;

    /**
     * @brief Convert to grayscale using luminance
     * @param color Input color
     * @return Grayscale color
     */
    inline float4 grayscale(const float4& color) noexcept;

    // ============================================================================
    // Geometric Operations (Declarations)
    // ============================================================================

    /**
     * @brief Project 4D homogeneous coordinates to 3D
     * @param vec 4D homogeneous vector
     * @return 3D projected coordinates
     */
    inline float3 project(const float4& vec) noexcept;

    /**
     * @brief Transform to homogeneous coordinates (set w = 1)
     * @param vec Input vector
     * @return Homogeneous coordinates
     */
    inline float4 to_homogeneous(const float4& vec) noexcept;

    // ============================================================================
    // Useful Constants (Declarations)
    // ============================================================================

    /**
     * @brief Zero vector constant (0, 0, 0, 0)
     */
    extern const float4 float4_Zero;

    /**
     * @brief One vector constant (1, 1, 1, 1)
     */
    extern const float4 float4_One;

    /**
     * @brief Unit X vector constant (1, 0, 0, 0)
     */
    extern const float4 float4_UnitX;

    /**
     * @brief Unit Y vector constant (0, 1, 0, 0)
     */
    extern const float4 float4_UnitY;

    /**
     * @brief Unit Z vector constant (0, 0, 1, 0)
     */
    extern const float4 float4_UnitZ;

    /**
     * @brief Unit W vector constant (0, 0, 0, 1)
     */
    extern const float4 float4_UnitW;

    // Color constants

    /**
     * @brief Red color constant (1, 0, 0, 1)
     */
    extern const float4 float4_Red;

    /**
     * @brief Green color constant (0, 1, 0, 1)
     */
    extern const float4 float4_Green;

    /**
     * @brief Blue color constant (0, 0, 1, 1)
     */
    extern const float4 float4_Blue;

    /**
     * @brief White color constant (1, 1, 1, 1)
     */
    extern const float4 float4_White;

    /**
     * @brief Black color constant (0, 0, 0, 1)
     */
    extern const float4 float4_Black;

    /**
     * @brief Transparent color constant (0, 0, 0, 0)
     */
    extern const float4 float4_Transparent;

    /**
     * @brief Yellow color constant (1, 1, 0, 1)
     */
    extern const float4 float4_Yellow;

    /**
     * @brief Cyan color constant (0, 1, 1, 1)
     */
    extern const float4 float4_Cyan;

    /**
     * @brief Magenta color constant (1, 0, 1, 1)
     */
    extern const float4 float4_Magenta;

} // namespace Math

// Include implementation
#include "math_float4.inl"
