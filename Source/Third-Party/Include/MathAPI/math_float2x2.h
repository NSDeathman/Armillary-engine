// Description: 2x2 matrix class with comprehensive mathematical operations,
//              SSE optimization, and full linear algebra support for 2D graphics
// Author: NSDeathman, DeepSeek
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
    /**
     * @class float2x2
     * @brief 2x2 matrix class with comprehensive mathematical operations
     *
     * Represents a 2x2 matrix stored in column-major order with SSE optimization.
     * Provides comprehensive linear algebra operations including matrix multiplication,
     * inversion, determinant calculation, and various 2D transformation matrices.
     *
     * @note Column-major storage for consistency with other matrix types
     * @note SSE optimization for performance-critical operations
     * @note Perfect for 2D transformations, linear algebra, and computer vision
     */
    class MATH_API float2x2
    {
    private:
        alignas(16) float2 col0_;  // First column: [m00, m10]
        alignas(16) float2 col1_;  // Second column: [m01, m11]

    public:
        // ============================================================================
        // Constructors
        // ============================================================================

        /**
         * @brief Default constructor (initializes to identity matrix)
         */
        float2x2() noexcept;

        /**
         * @brief Construct from column vectors
         * @param col0 First column vector
         * @param col1 Second column vector
         */
        float2x2(const float2& col0, const float2& col1) noexcept;

        /**
         * @brief Construct from components
         * @param col0x First column X component
         * @param col0y First column Y component
         * @param col1x Second column X component
         * @param col1y Second column Y component
         */
        float2x2(float col0x, float col0y, float col1x, float col1y) noexcept;

        /**
         * @brief Construct from column-major array
         * @param data Column-major array of 4 elements
         * @note Expected order: [col0.x, col0.y, col1.x, col1.y]
         */
        explicit float2x2(const float* data) noexcept;

        /**
         * @brief Construct from scalar (diagonal matrix)
         * @param scalar Value for diagonal elements
         */
        explicit float2x2(float scalar) noexcept;

        /**
         * @brief Construct from diagonal vector
         * @param diagonal Diagonal elements (x, y)
         */
        explicit float2x2(const float2& diagonal) noexcept;

        /**
         * @brief Construct from SSE data
         * @param sse_data SSE register containing matrix data
         */
        explicit float2x2(__m128 sse_data) noexcept;

        float2x2(const float2x2&) noexcept = default;

        // ============================================================================
        // Assignment Operators
        // ============================================================================

        float2x2& operator=(const float2x2&) noexcept = default;

        // ============================================================================
        // Static Constructors
        // ============================================================================

        /**
         * @brief Identity matrix
         * @return 2x2 identity matrix
         */
        static float2x2 identity() noexcept;

        /**
         * @brief Zero matrix
         * @return 2x2 zero matrix
         */
        static float2x2 zero() noexcept;

        /**
         * @brief Rotation matrix
         * @param angle Rotation angle in radians
         * @return 2D rotation matrix
         * @note Positive angle = counter-clockwise rotation
         */
        static float2x2 rotation(float angle) noexcept;

        /**
         * @brief Scaling matrix
         * @param scale Scale factors
         * @return 2D scaling matrix
         */
        static float2x2 scaling(const float2& scale) noexcept;

        /**
         * @brief Scaling matrix from components
         * @param x X scale
         * @param y Y scale
         * @return Scaling matrix
         */
        static float2x2 scaling(float x, float y) noexcept;

        /**
         * @brief Uniform scaling matrix
         * @param uniformScale Uniform scale factor
         * @return Scaling matrix
         */
        static float2x2 scaling(float uniformScale) noexcept;

        /**
         * @brief Shear matrix
         * @param shear Shear factors (x, y)
         * @return 2D shear matrix
         */
        static float2x2 shear(const float2& shear) noexcept;

        /**
         * @brief Shear matrix from components
         * @param x X shear factor
         * @param y Y shear factor
         * @return Shear matrix
         */
        static float2x2 shear(float x, float y) noexcept;

        // ============================================================================
        // Access Operators
        // ============================================================================

        /**
         * @brief Access column by index
         * @param colIndex Column index (0 or 1)
         * @return Reference to column
         * @note Column-major storage: [col][row]
         */
        float2& operator[](int colIndex) noexcept;

        /**
         * @brief Access column by index (const)
         * @param colIndex Column index (0 or 1)
         * @return Const reference to column
         */
        const float2& operator[](int colIndex) const noexcept;

        /**
         * @brief Access element by row and column (column-major)
         * @param row Row index (0 or 1)
         * @param col Column index (0 or 1)
         * @return Reference to element
         * @note Column-major: [col][row]
         */
        float& operator()(int row, int col) noexcept;

        /**
         * @brief Access element by row and column (const)
         * @param row Row index (0 or 1)
         * @param col Column index (0 or 1)
         * @return Const reference to element
         */
        const float& operator()(int row, int col) const noexcept;

        // ============================================================================
        // Column and Row Accessors
        // ============================================================================

        /**
         * @brief Get column 0
         * @return First column
         */
        float2 col0() const noexcept;

        /**
         * @brief Get column 1
         * @return Second column
         */
        float2 col1() const noexcept;

        /**
         * @brief Get row 0
         * @return First row
         */
        float2 row0() const noexcept;

        /**
         * @brief Get row 1
         * @return Second row
         */
        float2 row1() const noexcept;

        /**
         * @brief Set column 0
         * @param col New column values
         */
        void set_col0(const float2& col) noexcept;

        /**
         * @brief Set column 1
         * @param col New column values
         */
        void set_col1(const float2& col) noexcept;

        /**
         * @brief Set row 0
         * @param row New row values
         */
        void set_row0(const float2& row) noexcept;

        /**
         * @brief Set row 1
         * @param row New row values
         */
        void set_row1(const float2& row) noexcept;

        // ============================================================================
        // SSE Accessors
        // ============================================================================

        /**
         * @brief Get raw SSE data
         * @return SSE register containing matrix data
         */
        __m128 sse_data() const noexcept;

        /**
         * @brief Set raw SSE data
         * @param sse_data SSE register containing matrix data
         */
        void set_sse_data(__m128 sse_data) noexcept;

        // ============================================================================
        // Compound Assignment Operators
        // ============================================================================

        /**
         * @brief Matrix addition assignment
         * @param rhs Right-hand side matrix
         * @return Reference to this matrix
         */
        float2x2& operator+=(const float2x2& rhs) noexcept;

        /**
         * @brief Matrix subtraction assignment
         * @param rhs Right-hand side matrix
         * @return Reference to this matrix
         */
        float2x2& operator-=(const float2x2& rhs) noexcept;

        /**
         * @brief Scalar multiplication assignment
         * @param scalar Scalar multiplier
         * @return Reference to this matrix
         */
        float2x2& operator*=(float scalar) noexcept;

        /**
         * @brief Scalar division assignment
         * @param scalar Scalar divisor
         * @return Reference to this matrix
         */
        float2x2& operator/=(float scalar) noexcept;

        /**
         * @brief Matrix multiplication assignment
         * @param rhs Right-hand side matrix
         * @return Reference to this matrix
         */
        float2x2& operator*=(const float2x2& rhs) noexcept;

        // ============================================================================
        // Unary Operators
        // ============================================================================

        /**
         * @brief Unary plus operator
         * @return Copy of this matrix
         */
        float2x2 operator+() const noexcept;

        /**
         * @brief Unary minus operator
         * @return Negated matrix
         */
        float2x2 operator-() const noexcept;

        // ============================================================================
        // Matrix Operations
        // ============================================================================

        /**
         * @brief Compute transposed matrix
         * @return Transposed matrix
         */
        float2x2 transposed() const noexcept;

        /**
         * @brief Compute matrix determinant
         * @return Determinant value
         */
        float determinant() const noexcept;

        /**
         * @brief Compute inverse matrix
         * @return Inverse matrix
         * @note Returns identity if matrix is singular
         * @warning Matrix must be invertible (determinant != 0)
         */
        float2x2 inverted() const noexcept;

        /**
         * @brief Compute adjugate matrix
         * @return Adjugate matrix
         * @note Used for inverse calculation
         */
        float2x2 adjugate() const noexcept;

        /**
         * @brief Compute matrix trace (sum of diagonal elements)
         * @return Trace value
         */
        float trace() const noexcept;

        /**
         * @brief Extract diagonal elements
         * @return Diagonal as float2 vector
         */
        float2 diagonal() const noexcept;

        /**
         * @brief Compute Frobenius norm (sqrt of sum of squares of all elements)
         * @return Frobenius norm
         */
        float frobenius_norm() const noexcept;

        // ============================================================================
        // Vector Transformations
        // ============================================================================

        /**
         * @brief Transform 2D vector (matrix * vector)
         * @param vec 2D vector to transform
         * @return Transformed 2D vector
         */
        float2 transform_vector(const float2& vec) const noexcept;

        /**
         * @brief Transform 2D point
         * @param point 2D point to transform
         * @return Transformed 2D point
         */
        float2 transform_point(const float2& point) const noexcept;

        // ============================================================================
        // Transformation Component Extraction
        // ============================================================================

        /**
         * @brief Extract rotation angle from matrix
         * @return Rotation angle in radians
         * @note Assumes matrix represents pure rotation
         */
        float get_rotation() const noexcept;

        /**
         * @brief Extract scale component
         * @return Scale vector
         * @note Computes length of each axis vector
         */
        float2 get_scale() const noexcept;

        /**
         * @brief Set rotation component
         * @param angle Rotation angle in radians
         * @note Preserves existing scale
         */
        void set_rotation(float angle) noexcept;

        /**
         * @brief Set scale component
         * @param scale New scale vector
         * @note Preserves existing rotation
         */
        void set_scale(const float2& scale) noexcept;

        // ============================================================================
        // Utility Methods
        // ============================================================================

        /**
         * @brief Check if matrix is identity within tolerance
         * @param epsilon Comparison tolerance
         * @return True if matrix is approximately identity
         */
        bool is_identity(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Check if matrix is orthogonal
         * @param epsilon Comparison tolerance
         * @return True if matrix is orthogonal
         */
        bool is_orthogonal(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Check if matrix is rotation matrix
         * @param epsilon Comparison tolerance
         * @return True if matrix is pure rotation
         */
        bool is_rotation(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Check approximate equality with another matrix
         * @param other Matrix to compare with
         * @param epsilon Comparison tolerance
         * @return True if matrices are approximately equal
         */
        bool approximately(const float2x2& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Check if all matrix elements are approximately zero
         * @param epsilon Comparison tolerance
         * @return True if all elements are approximately zero within tolerance
         */
        bool approximately_zero(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Convert to string representation
         * @return String representation of matrix
         * @note Format: "[row0]\n[row1]"
         */
        std::string to_string() const;

        /**
         * @brief Store matrix to column-major array
         * @param data Destination array (must have at least 4 elements)
         * @note Column-major order: [col0.x, col0.y, col1.x, col1.y]
         */
        void to_column_major(float* data) const noexcept;

        /**
         * @brief Store matrix to row-major array
         * @param data Destination array (must have at least 4 elements)
         * @note Row-major order: [row0.x, row0.y, row1.x, row1.y]
         */
        void to_row_major(float* data) const noexcept;

        // ============================================================================
        // Comparison Operators
        // ============================================================================

        /**
         * @brief Equality comparison
         * @param rhs Right-hand side matrix
         * @return True if matrices are approximately equal
         */
        bool operator==(const float2x2& rhs) const noexcept;

        /**
         * @brief Inequality comparison
         * @param rhs Right-hand side matrix
         * @return True if matrices are not approximately equal
         */
        bool operator!=(const float2x2& rhs) const noexcept;
    };

    // ============================================================================
    // Binary Operators
    // ============================================================================

    float2x2 operator+(float2x2 lhs, const float2x2& rhs) noexcept;
    float2x2 operator-(float2x2 lhs, const float2x2& rhs) noexcept;
    float2x2 operator*(const float2x2& lhs, const float2x2& rhs) noexcept;
    float2x2 operator*(float2x2 mat, float scalar) noexcept;
    float2x2 operator*(float scalar, float2x2 mat) noexcept;
    float2x2 operator/(float2x2 mat, float scalar) noexcept;
    float2 operator*(const float2& vec, const float2x2& mat) noexcept;

    // ============================================================================
    // Global Functions
    // ============================================================================

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

    // ============================================================================
    // Useful Constants
    // ============================================================================

    /**
     * @brief Identity matrix constant
     */
    extern const float2x2 float2x2_Identity;

    /**
     * @brief Zero matrix constant
     */
    extern const float2x2 float2x2_Zero;

} // namespace Math

// Include inline implementation
#include "math_float2x2.inl"
