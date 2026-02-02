// Description: 3-dimensional vector class with comprehensive 
//              mathematical operations and SSE optimization (Packed 12-byte version)
// Author: NSDeathman, DeepSeek
#pragma once

#include <xmmintrin.h>
#include <pmmintrin.h> // SSE3
#include <smmintrin.h>
#include <string>
#include <cstdio>
#include <cmath>
#include <algorithm>

#include "math_config.h"
#include "math_constants.h"
#include "math_functions.h"
#include "math_float2.h"

namespace Math
{
    /**
     * @class float3
     * @brief 3-dimensional vector with comprehensive mathematical operations
     *
     * Represents a 3D vector (x, y, z). This version is packed to 12 bytes
     * (no padding) to ensure compatibility with standard vertex buffers.
     * Operations still use SSE acceleration by loading/storing to registers on the fly.
     *
     * @note Perfect for 3D game development, computer graphics, and physics engines
     * @note Size is exactly 12 bytes.
     * @note Includes comprehensive HLSL-like function set
     */
    class float3
    {
    public:
        // ============================================================================
        // Data Members (Public for Direct Access)
        // ============================================================================

        // Removed union with __m128 to enforce 12-byte size
        float x; ///< X component of the vector
        float y; ///< Y component of the vector  
        float z; ///< Z component of the vector

        // ============================================================================
        // Constructors
        // ============================================================================

        /**
         * @brief Default constructor (initializes to zero vector)
         */
        float3() noexcept;

        /**
         * @brief Construct from components
         * @param x X component
         * @param y Y component
         * @param z Z component
         */
        float3(float x, float y, float z) noexcept;

        /**
         * @brief Construct from scalar (all components set to same value)
         * @param scalar Value for all components
         */
        explicit float3(float scalar) noexcept;

        /**
         * @brief Construct from float2 and z component
         * @param vec 2D vector for x and y components
         * @param z Z component
         */
        float3(const float2& vec, float z = 0.0f) noexcept;

        /**
         * @brief Copy constructor
         */
        float3(const float3&) noexcept = default;

        /**
         * @brief Construct from raw float array
         * @param data Pointer to float array [x, y, z]
         */
        explicit float3(const float* data) noexcept;

        /**
         * @brief Construct from SSE register (internal helper)
         * @param simd_val SSE register containing vector data in [x, y, z, -]
         */
        explicit float3(__m128 simd_val) noexcept;

        // ============================================================================
        // Assignment Operators
        // ============================================================================

        /**
         * @brief Copy assignment operator
         */
        float3& operator=(const float3&) noexcept = default;

        /**
         * @brief Scalar assignment (sets all components to same value)
         * @param scalar Value for all components
         */
        float3& operator=(float scalar) noexcept;

        // ============================================================================
        // Compound Assignment Operators
        // ============================================================================

        /**
         * @brief Component-wise addition assignment
         * @param rhs Right-hand side vector
         * @return Reference to this vector
         */
        float3& operator+=(const float3& rhs) noexcept;

        /**
         * @brief Component-wise subtraction assignment
         * @param rhs Right-hand side vector
         * @return Reference to this vector
         */
        float3& operator-=(const float3& rhs) noexcept;

        /**
         * @brief Component-wise multiplication assignment
         * @param rhs Right-hand side vector
         * @return Reference to this vector
         */
        float3& operator*=(const float3& rhs) noexcept;

        /**
         * @brief Component-wise division assignment
         * @param rhs Right-hand side vector
         * @return Reference to this vector
         */
        float3& operator/=(const float3& rhs) noexcept;

        /**
         * @brief Scalar multiplication assignment
         * @param scalar Scalar multiplier
         * @return Reference to this vector
         */
        float3& operator*=(float scalar) noexcept;

        /**
         * @brief Scalar division assignment
         * @param scalar Scalar divisor
         * @return Reference to this vector
         */
        float3& operator/=(float scalar) noexcept;

        // ============================================================================
        // Unary Operators
        // ============================================================================

        /**
         * @brief Unary plus operator
         * @return Copy of this vector
         */
        float3 operator+() const noexcept;

        /**
         * @brief Unary minus operator
         * @return Negated vector
         */
        float3 operator-() const noexcept;

        // ============================================================================
        // Access Operators
        // ============================================================================

        /**
         * @brief Access component by index
         * @param index Component index (0 = x, 1 = y, 2 = z)
         * @return Reference to component
         */
        float& operator[](int index) noexcept;

        /**
         * @brief Access component by index (const)
         * @param index Component index (0 = x, 1 = y, 2 = z)
         * @return Const reference to component
         */
        const float& operator[](int index) const noexcept;

        // ============================================================================
        // Conversion Operators
        // ============================================================================

        /**
         * @brief Convert to const float pointer (for interoperability)
         * @return Pointer to const float array [x, y, z]
         */
        operator const float* () const noexcept;

        /**
         * @brief Convert to float pointer (for interoperability)
         * @return Pointer to float array [x, y, z]
         */
        operator float* () noexcept;

        /**
         * @brief Convert to SSE register (Loads from memory)
         * @return SSE register containing vector data
         */
        operator __m128() const noexcept;

        // ============================================================================
        // Static Constructors
        // ============================================================================

        /**
         * @brief Zero vector (0, 0, 0)
         * @return Zero vector
         */
        static float3 zero() noexcept;

        /**
         * @brief One vector (1, 1, 1)
         * @return One vector
         */
        static float3 one() noexcept;

        /**
         * @brief Unit X vector (1, 0, 0)
         * @return Unit X vector
         */
        static float3 unit_x() noexcept;

        /**
         * @brief Unit Y vector (0, 1, 0)
         * @return Unit Y vector
         */
        static float3 unit_y() noexcept;

        /**
         * @brief Unit Z vector (0, 0, 1)
         * @return Unit Z vector
         */
        static float3 unit_z() noexcept;

        /**
         * @brief Forward vector (0, 0, 1) - common in 3D graphics
         * @return Forward vector
         */
        static float3 forward() noexcept;

        /**
         * @brief Up vector (0, 1, 0) - common in 3D graphics
         * @return Up vector
         */
        static float3 up() noexcept;

        /**
         * @brief Right vector (1, 0, 0) - common in 3D graphics
         * @return Right vector
         */
        static float3 right() noexcept;

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
        float length_sq() const noexcept;

        /**
         * @brief Normalize vector to unit length
         * @return Normalized vector
         * @note Returns zero vector if length is zero
         */
        float3 normalize() const noexcept;

        /**
         * @brief Compute dot product with another vector
         * @param other Other vector
         * @return Dot product result
         */
        float dot(const float3& other) const noexcept;

        /**
         * @brief Compute cross product with another vector
         * @param other Other vector
         * @return Cross product result
         */
        float3 cross(const float3& other) const noexcept;

        /**
         * @brief Compute distance to another point
         * @param other Other point
         * @return Euclidean distance
         */
        float distance(const float3& other) const noexcept;

        /**
         * @brief Compute squared distance to another point (faster)
         * @param other Other point
         * @return Squared Euclidean distance
         */
        float distance_sq(const float3& other) const noexcept;

        // ============================================================================
        // HLSL-like Functions
        // ============================================================================

        /**
         * @brief HLSL-like clamp function (component-wise clamping)
         * @param vec Vector to clamp
         * @param min_val Minimum values
         * @param max_val Maximum values
         * @return Clamped vector
         */
        static float3 clamp(const float3& vec, const float3& min_val, const float3& max_val) noexcept;

        /**
         * @brief Scalar clamp function (all components clamped to same range)
         * @param vec Vector to clamp
         * @param min_val Minimum value for all components
         * @param max_val Maximum value for all components
         * @return Clamped vector
         */
        static float3 clamp(const float3& vec, float min_val, float max_val) noexcept;

        /**
         * @brief HLSL-like abs function (component-wise absolute value)
         * @return Vector with absolute values of components
         */
        float3 abs() const noexcept;

        /**
         * @brief HLSL-like sign function (component-wise sign)
         * @return Vector with signs of components (-1, 0, or 1)
         */
        float3 sign() const noexcept;

        /**
         * @brief HLSL-like floor function (component-wise floor)
         * @return Vector with floored components
         */
        float3 floor() const noexcept;

        /**
         * @brief HLSL-like ceil function (component-wise ceiling)
         * @return Vector with ceiling components
         */
        float3 ceil() const noexcept;

        /**
         * @brief HLSL-like round function (component-wise rounding)
         * @return Vector with rounded components
         */
        float3 round() const noexcept;

        /**
         * @brief HLSL-like frac function (component-wise fractional part)
         * @return Vector with fractional parts of components
         */
        float3 frac() const noexcept;

        /**
         * @brief HLSL-like saturate function (clamp components to [0, 1])
         * @return Saturated vector
         */
        float3 saturate() const noexcept;

        /**
         * @brief HLSL-like step function (component-wise step)
         * @param edge Edge value
         * @return 1.0 if component >= edge, else 0.0
         */
        float3 step(float edge) const noexcept;

        // ============================================================================
        // Geometric Operations
        // ============================================================================

        /**
         * @brief Compute reflection vector
         * @param normal Surface normal (must be normalized)
         * @return Reflected vector
         */
        float3 reflect(const float3& normal) const noexcept;

        /**
         * @brief Compute refraction vector
         * @param normal Surface normal (must be normalized)
         * @param eta Ratio of indices of refraction
         * @return Refracted vector
         */
        float3 refract(const float3& normal, float eta) const noexcept;

        /**
         * @brief Project vector onto another vector
         * @param onto Vector to project onto
         * @return Projected vector
         */
        float3 project(const float3& onto) const noexcept;

        /**
         * @brief Reject vector from another vector (component perpendicular)
         * @param from Vector to reject from
         * @return Rejected vector
         */
        float3 reject(const float3& from) const noexcept;

        // ============================================================================
        // Static Mathematical Functions
        // ============================================================================

        /**
         * @brief Compute dot product of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Dot product result
         */
        static float dot(const float3& a, const float3& b) noexcept;

        /**
         * @brief Compute cross product of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Cross product result
         */
        static float3 cross(const float3& a, const float3& b) noexcept;

        /**
         * @brief Linear interpolation between two vectors
         * @param a Start vector
         * @param b End vector
         * @param t Interpolation factor [0, 1]
         * @return Interpolated vector
         */
        static float3 lerp(const float3& a, const float3& b, float t) noexcept;

        /**
         * @brief Spherical linear interpolation (for directions)
         * @param a Start vector (should be normalized)
         * @param b End vector (should be normalized)
         * @param t Interpolation factor [0, 1]
         * @return Interpolated vector
         */
        static float3 slerp(const float3& a, const float3& b, float t) noexcept;

        /**
         * @brief Component-wise minimum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise minimum
         */
        static float3 min(const float3& a, const float3& b) noexcept;

        /**
         * @brief Component-wise maximum of two vectors
         * @param a First vector
         * @param b Second vector
         * @return Component-wise maximum
         */
        static float3 max(const float3& a, const float3& b) noexcept;

        /**
         * @brief HLSL-like saturate function (clamp components to [0, 1])
         * @param vec Vector to saturate
         * @return Saturated vector
         */
        static float3 saturate(const float3& vec) noexcept;

        /**
         * @brief Compute reflection vector
         * @param incident Incident vector
         * @param normal Surface normal (must be normalized)
         * @return Reflected vector
         */
        static float3 reflect(const float3& incident, const float3& normal) noexcept;

        /**
         * @brief Compute refraction vector
         * @param incident Incident vector
         * @param normal Surface normal (must be normalized)
         * @param eta Ratio of indices of refraction
         * @return Refracted vector
         */
        static float3 refract(const float3& incident, const float3& normal, float eta) noexcept;

        // ============================================================================
        // Swizzle Operations (HLSL style)
        // ============================================================================

        float2 xy() const noexcept;
        float2 xz() const noexcept;
        float2 yz() const noexcept;
        float2 yx() const noexcept;
        float2 zx() const noexcept;
        float2 zy() const noexcept;

        float3 yxz() const noexcept;
        float3 zxy() const noexcept;
        float3 zyx() const noexcept;
        float3 xzy() const noexcept;
        float3 xyx() const noexcept;
        float3 xyz() const noexcept;
        float3 xzx() const noexcept;
        float3 yxy() const noexcept;
        float3 yzy() const noexcept;
        float3 zxz() const noexcept;
        float3 zyz() const noexcept;

        // Color swizzles
        float r() const noexcept;
        float g() const noexcept;
        float b() const noexcept;
        float2 rg() const noexcept;
        float2 rb() const noexcept;
        float2 gb() const noexcept;
        float3 rgb() const noexcept;
        float3 bgr() const noexcept;
        float3 gbr() const noexcept;

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
        bool approximately(const float3& other, float epsilon = EPSILON) const noexcept;

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
        bool is_normalized(float epsilon = 0.001f) const noexcept;

        /**
         * @brief Convert to string representation
         * @return String in format "(x, y, z)"
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
         * @brief Set x and y components from float2
         * @param xy 2D vector for x and y components
         */
        void set_xy(const float2& xy) noexcept;

        /**
         * @brief Create SSE register from current data (internal helper)
         * @return SSE register containing vector data
         */
        __m128 get_simd() const noexcept;

        /**
         * @brief Store SSE register to current data (internal helper)
         * @param new_simd SSE register to set
         */
        void set_simd(__m128 new_simd) noexcept;

        // ============================================================================
        // Comparison Operators
        // ============================================================================

        bool operator==(const float3& rhs) const noexcept;
        bool operator!=(const float3& rhs) const noexcept;
    };

    // ============================================================================
    // Binary Operators
    // ============================================================================

    inline float3 operator+(float3 lhs, const float3& rhs) noexcept;
    inline float3 operator-(float3 lhs, const float3& rhs) noexcept;
    inline float3 operator*(float3 lhs, const float3& rhs) noexcept;
    inline float3 operator/(float3 lhs, const float3& rhs) noexcept;
    inline float3 operator*(float3 vec, float scalar) noexcept;
    inline float3 operator*(float scalar, float3 vec) noexcept;
    inline float3 operator/(float3 vec, float scalar) noexcept;

    // ============================================================================
    // Global Mathematical Functions
    // ============================================================================

    inline float distance(const float3& a, const float3& b) noexcept;
    inline float distance_sq(const float3& a, const float3& b) noexcept;
    inline float dot(const float3& a, const float3& b) noexcept;
    inline float3 cross(const float3& a, const float3& b) noexcept;
    inline float3 normalize(const float3& vec) noexcept;
    inline float3 lerp(const float3& a, const float3& b, float t) noexcept;
    inline float3 slerp(const float3& a, const float3& b, float t) noexcept;
    inline bool approximately(const float3& a, const float3& b, float epsilon = EPSILON) noexcept;
    inline bool is_normalized(const float3& vec, float epsilon = EPSILON) noexcept;
    inline bool are_orthogonal(const float3& a, const float3& b, float epsilon = EPSILON) noexcept;
    inline bool is_orthonormal_basis(const float3& x, const float3& y, const float3& z, float epsilon = EPSILON) noexcept;
    inline bool isValid(const float3& vec) noexcept;

    // ============================================================================
    // HLSL-like Global Functions
    // ============================================================================

    inline float3 abs(const float3& vec) noexcept;
    inline float3 sign(const float3& vec) noexcept;
    inline float3 floor(const float3& vec) noexcept;
    inline float3 ceil(const float3& vec) noexcept;
    inline float3 round(const float3& vec) noexcept;
    inline float3 frac(const float3& vec) noexcept;
    inline float3 saturate(const float3& vec) noexcept;
    inline float3 step(float edge, const float3& vec) noexcept;
    inline float3 min(const float3& a, const float3& b) noexcept;
    inline float3 max(const float3& a, const float3& b) noexcept;
    inline float3 clamp(const float3& vec, const float3& min_val, const float3& max_val) noexcept;
    inline float3 clamp(const float3& vec, float min_val, float max_val) noexcept;
    inline float length(const float3& vec) noexcept;
    inline float length_sq(const float3& vec) noexcept;

    // ============================================================================
    // Geometric Operations
    // ============================================================================

    inline float3 reflect(const float3& incident, const float3& normal) noexcept;
    inline float3 refract(const float3& incident, const float3& normal, float eta) noexcept;
    inline float3 project(const float3& vec, const float3& onto) noexcept;
    inline float3 reject(const float3& vec, const float3& from) noexcept;
    inline float angle_between(const float3& a, const float3& b) noexcept;

    // ============================================================================
    // Useful Constants
    // ============================================================================

    extern const float3 float3_Zero;
    extern const float3 float3_One;
    extern const float3 float3_UnitX;
    extern const float3 float3_UnitY;
    extern const float3 float3_UnitZ;
    extern const float3 float3_Forward;
    extern const float3 float3_Up;
    extern const float3 float3_Right;

} // namespace Math

#include "math_float3.inl"
