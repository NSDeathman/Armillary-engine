// Description: 3x3 matrix class with comprehensive mathematical operations,
//              SSE optimization, and full linear algebra support
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
#include "math_float3.h"
#include "math_float4.h"

namespace Math
{
    class float4x4;
    class quaternion;

    /**
     * @class float3x3
     * @brief 3x3 matrix class with comprehensive mathematical operations
     *
     * Represents a 3x3 matrix stored in column-major order with SSE optimization.
     * Provides comprehensive linear algebra operations including matrix multiplication,
     * inversion, determinant calculation, and various matrix decompositions.
     *
     * @note Column-major storage for compatibility with OpenGL and modern graphics APIs
     * @note SSE optimized for performance-critical operations
     * @note Perfect for 3D transformations, normal matrices, and linear algebra
     */
    class MATH_API float3x3
    {
    public:
        // Store matrix as three float4 for alignment and SSE optimization
        // Each column is stored in float4 (fourth component unused)
        alignas(16) float4 col0_, col1_, col2_;

    public:
        // ============================================================================
        // Constructors
        // ============================================================================

        /**
         * @brief Default constructor (initializes to zero matrix)
         */
        float3x3() noexcept;

        /**
         * @brief Construct from column vectors
         * @param col0 First column vector
         * @param col1 Second column vector
         * @param col2 Third column vector
         */
        float3x3(const float3& col0, const float3& col1, const float3& col2) noexcept;

        /**
         * @brief Construct from 9 scalar values (row-major order)
         * @param m00 Element at row 0, column 0
         * @param m01 Element at row 0, column 1
         * @param m02 Element at row 0, column 2
         * @param m10 Element at row 1, column 0
         * @param m11 Element at row 1, column 1
         * @param m12 Element at row 1, column 2
         * @param m20 Element at row 2, column 0
         * @param m21 Element at row 2, column 1
         * @param m22 Element at row 2, column 2
         * @note Parameters are in row-major order for intuitive initialization
         * @note Internally converts to column-major storage
         */
        float3x3(float m00, float m01, float m02,
            float m10, float m11, float m12,
            float m20, float m21, float m22) noexcept;

        /**
         * @brief Construct from row-major array (converts to column-major)
         * @param data Row-major array of 9 elements
         */
        explicit float3x3(const float* data) noexcept;

        /**
         * @brief Construct from scalar (diagonal matrix)
         * @param scalar Value for diagonal elements
         */
        explicit float3x3(float scalar) noexcept;

        /**
         * @brief Construct from diagonal vector
         * @param diagonal Diagonal elements (x, y, z)
         */
        explicit float3x3(const float3& diagonal) noexcept;

        /**
         * @brief Construct from 4x4 matrix (extracts upper-left 3x3)
         * @param mat4x4 4x4 matrix to extract from
         * @note Uses SSE optimization for efficient extraction
         */
        explicit float3x3(const float4x4& mat4x4) noexcept;

        /**
         * @brief Construct from quaternion (rotation matrix)
         * @param q Unit quaternion representing rotation
         * @note Optimized conversion with SSE support
         */
        explicit float3x3(const quaternion& q) noexcept;

        /**
         * @brief Copy constructor
         */
        float3x3(const float3x3&) noexcept = default;

        // ============================================================================
        // Assignment Operators
        // ============================================================================

        /**
         * @brief Copy assignment operator
         */
        float3x3& operator=(const float3x3&) noexcept = default;

        /**
         * @brief Assignment from 4x4 matrix
         * @param mat4x4 4x4 matrix to extract from
         * @return Reference to this matrix
         * @note Uses SSE optimization for efficient extraction
         */
        float3x3& operator=(const float4x4& mat4x4) noexcept;

        // ============================================================================
        // Access Operators
        // ============================================================================

        /**
         * @brief Access column by index
         * @param colIndex Column index (0, 1, or 2)
         * @return Reference to column as float3
         * @note Column-major storage: [col][row]
         */
        float3& operator[](int colIndex) noexcept;

        /**
         * @brief Access column by index (const)
         * @param colIndex Column index (0, 1, or 2)
         * @return Const reference to column as float3
         */
        const float3& operator[](int colIndex) const noexcept;

        /**
         * @brief Access element by row and column (column-major)
         * @param row Row index (0, 1, or 2)
         * @param col Column index (0, 1, or 2)
         * @return Reference to element
         * @note Column-major: [col][row]
         */
        float& operator()(int row, int col) noexcept;

        /**
         * @brief Access element by row and column (const)
         * @param row Row index (0, 1, or 2)
         * @param col Column index (0, 1, or 2)
         * @return Const reference to element
         */
        const float& operator()(int row, int col) const noexcept;

        // ============================================================================
        // Column and Row Accessors
        // ============================================================================

        /**
         * @brief Get column 0
         * @return First column as float3
         */
        float3 col0() const noexcept;

        /**
         * @brief Get column 1
         * @return Second column as float3
         */
        float3 col1() const noexcept;

        /**
         * @brief Get column 2
         * @return Third column as float3
         */
        float3 col2() const noexcept;

        /**
         * @brief Get row 0
         * @return First row as float3
         */
        float3 row0() const noexcept;

        /**
         * @brief Get row 1
         * @return Second row as float3
         */
        float3 row1() const noexcept;

        /**
         * @brief Get row 2
         * @return Third row as float3
         */
        float3 row2() const noexcept;

        /**
         * @brief Set column 0
         * @param col New column values
         */
        void set_col0(const float3& col) noexcept;

        /**
         * @brief Set column 1
         * @param col New column values
         */
        void set_col1(const float3& col) noexcept;

        /**
         * @brief Set column 2
         * @param col New column values
         */
        void set_col2(const float3& col) noexcept;

        /**
         * @brief Set row 0
         * @param row New row values
         */
        void set_row0(const float3& row) noexcept;

        /**
         * @brief Set row 1
         * @param row New row values
         */
        void set_row1(const float3& row) noexcept;

        /**
         * @brief Set row 2
         * @param row New row values
         */
        void set_row2(const float3& row) noexcept;

        // ============================================================================
        // Static Constructors
        // ============================================================================

        /**
         * @brief Identity matrix
         * @return 3x3 identity matrix
         */
        static float3x3 identity() noexcept;

        /**
         * @brief Zero matrix
         * @return 3x3 zero matrix
         */
        static float3x3 zero() noexcept;

        /**
         * @brief Check if matrix is approximately zero (all elements near zero)
         * @param epsilon Comparison tolerance
         * @return True if all matrix elements are approximately zero
         * @note Useful for checking if matrix can be considered as zero matrix
         */
        bool approximately_zero(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Scaling matrix
         * @param scale Scale factors for x, y, z axes
         * @return Scaling matrix
         */
        static float3x3 scaling(const float3& scale) noexcept;

        static float3x3 scaling(const float& scaleX, const float& scaleY, const float& scaleZ) noexcept;

        /**
         * @brief Scaling matrix from uniform scale
         * @param scale Uniform scale factor
         * @return Scaling matrix
         */
        static float3x3 scaling(float scale) noexcept;

        /**
         * @brief Rotation matrix around X axis
         * @param angle Rotation angle in radians
         * @return Rotation matrix around X
         */
        static float3x3 rotation_x(float angle) noexcept;

        /**
         * @brief Rotation matrix around Y axis
         * @param angle Rotation angle in radians
         * @return Rotation matrix around Y
         */
        static float3x3 rotation_y(float angle) noexcept;

        /**
         * @brief Rotation matrix around Z axis
         * @param angle Rotation angle in radians
         * @return Rotation matrix around Z
         */
        static float3x3 rotation_z(float angle) noexcept;

        /**
         * @brief Rotation matrix from axis and angle
         * @param axis Rotation axis (must be normalized)
         * @param angle Rotation angle in radians
         * @return Rotation matrix
         */
        static float3x3 rotation_axis(const float3& axis, float angle) noexcept;

        /**
         * @brief Rotation matrix from Euler angles (ZYX order)
         * @param angles Euler angles in radians (pitch, yaw, roll)
         * @return Rotation matrix
         * @note Order: rotation_z(angles.z) * rotation_y(angles.y) * rotation_x(angles.x)
         */
        static float3x3 rotation_euler(const float3& angles) noexcept;

        /**
         * @brief Skew-symmetric matrix for cross product
         * @param vec Vector to create skew-symmetric matrix from
         * @return Skew-symmetric matrix such that skew(vec) * a = cross(vec, a)
         */
        static float3x3 skew_symmetric(const float3& vec) noexcept;

        /**
         * @brief Outer product matrix (u * v^T)
         * @param u First vector
         * @param v Second vector
         * @return Outer product matrix
         */
        static float3x3 outer_product(const float3& u, const float3& v) noexcept;

        // ============================================================================
        // Compound Assignment Operators (SSE Optimized)
        // ============================================================================

        /**
         * @brief Matrix addition assignment
         * @param rhs Right-hand side matrix
         * @return Reference to this matrix
         * @note SSE optimized
         */
        float3x3& operator+=(const float3x3& rhs) noexcept;

        /**
         * @brief Matrix subtraction assignment
         * @param rhs Right-hand side matrix
         * @return Reference to this matrix
         * @note SSE optimized
         */
        float3x3& operator-=(const float3x3& rhs) noexcept;

        /**
         * @brief Scalar multiplication assignment
         * @param scalar Scalar multiplier
         * @return Reference to this matrix
         * @note SSE optimized
         */
        float3x3& operator*=(float scalar) noexcept;

        /**
         * @brief Scalar division assignment
         * @param scalar Scalar divisor
         * @return Reference to this matrix
         * @note SSE optimized
         */
        float3x3& operator/=(float scalar) noexcept;

        /**
         * @brief Matrix multiplication assignment
         * @param rhs Right-hand side matrix
         * @return Reference to this matrix
         */
        float3x3& operator*=(const float3x3& rhs) noexcept;

        // ============================================================================
        // Unary Operators
        // ============================================================================

        /**
         * @brief Unary plus operator
         * @return Copy of this matrix
         */
        float3x3 operator+() const noexcept;

        /**
         * @brief Unary minus operator
         * @return Negated matrix
         */
        float3x3 operator-() const noexcept;

        // ============================================================================
        // Matrix Operations (SSE Optimized)
        // ============================================================================

        /**
         * @brief Compute transposed matrix
         * @return Transposed matrix
         * @note SSE optimized using shuffle operations
         */
        float3x3 transposed() const noexcept;

        /**
         * @brief Compute matrix determinant
         * @return Determinant value
         * @note Formula: a(ei − fh) − b(di − fg) + c(dh − eg)
         */
        float determinant() const noexcept;

        /**
         * @brief Compute inverse matrix
         * @return Inverse matrix
         * @note Returns identity if matrix is singular
         * @warning Matrix must be invertible (determinant != 0)
         */
        float3x3 inverted() const noexcept;

        /**
         * @brief Compute normal matrix (transpose(inverse(mat)))
         * @param model Model transformation matrix
         * @return Normal transformation matrix
         * @note Used for transforming normal vectors
         */
        static float3x3 normal_matrix(const float3x3& model) noexcept;

        /**
         * @brief Compute matrix trace (sum of diagonal elements)
         * @return Trace value
         */
        float trace() const noexcept;

        /**
         * @brief Extract diagonal elements
         * @return Diagonal as float3 vector
         */
        float3 diagonal() const noexcept;

        /**
         * @brief Compute Frobenius norm (sqrt of sum of squares of all elements)
         * @return Frobenius norm
         */
        float frobenius_norm() const noexcept;

        /**
         * @brief Compute symmetric part: (A + A^T)/2
         * @return Symmetric part of matrix
         */
        float3x3 symmetric_part() const noexcept;

        /**
         * @brief Compute skew-symmetric part: (A - A^T)/2
         * @return Skew-symmetric part of matrix
         */
        float3x3 skew_symmetric_part() const noexcept;

        // ============================================================================
        // Vector Transformations (SSE Optimized)
        // ============================================================================

        /**
         * @brief Transform vector (matrix * vector)
         * @param vec Vector to transform
         * @return Transformed vector
         * @note SSE optimized matrix-vector multiplication
         */
        float3 transform_vector(const float3& vec) const noexcept;

        /**
         * @brief Transform vector as point (handles affine transformations)
         * @param point Point to transform
         * @return Transformed point
         * @note For 3x3 matrices, same as transform_vector (no translation)
         */
        float3 transform_point(const float3& point) const noexcept;

        /**
         * @brief Transform normal vector (transpose(inverse(mat)) * normal)
         * @param normal Normal vector to transform
         * @return Transformed normal
         * @note Properly handles non-uniform scaling
         */
        float3 transform_normal(const float3& normal) const noexcept;

        // ============================================================================
        // Decomposition Methods
        // ============================================================================

        /**
         * @brief Extract scale factors from transformation matrix
         * @return Scale factors as float3
         * @note Computes length of each column vector
         */
        float3 extract_scale() const noexcept;

        /**
         * @brief Extract rotation matrix (orthonormal basis)
         * @return Rotation matrix
         * @note Removes scaling from transformation matrix
         */
        float3x3 extract_rotation() const noexcept;

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
         * @brief Check if matrix is orthogonal (columns are orthogonal)
         * @param epsilon Comparison tolerance
         * @return True if matrix is orthogonal
         */
        bool is_orthogonal(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Check if matrix is orthonormal (orthogonal with unit-length columns)
         * @param epsilon Comparison tolerance
         * @return True if matrix is orthonormal
         */
        bool is_orthonormal(float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Check approximate equality with another matrix
         * @param other Matrix to compare with
         * @param epsilon Comparison tolerance
         * @return True if matrices are approximately equal
         */
        bool approximately(const float3x3& other, float epsilon = Constants::Constants<float>::Epsilon) const noexcept;

        /**
         * @brief Convert to string representation
         * @return String representation of matrix
         * @note Format: "[row0]\n[row1]\n[row2]"
         */
        std::string to_string() const;

        /**
         * @brief Store matrix to column-major array
         * @param data Destination array (must have at least 9 elements)
         * @note Column-major order: [col0.x, col0.y, col0.z, col1.x, ...]
         */
        void to_column_major(float* data) const noexcept;

        /**
         * @brief Store matrix to row-major array
         * @param data Destination array (must have at least 9 elements)
         * @note Row-major order: [row0.x, row0.y, row0.z, row1.x, ...]
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
        bool operator==(const float3x3& rhs) const noexcept;

        /**
         * @brief Inequality comparison
         * @param rhs Right-hand side matrix
         * @return True if matrices are not approximately equal
         */
        bool operator!=(const float3x3& rhs) const noexcept;

        bool isValid() const noexcept;
    };

    // ============================================================================
    // Binary Operators
    // ============================================================================

    float3x3 operator+(float3x3 lhs, const float3x3& rhs) noexcept;
    float3x3 operator-(float3x3 lhs, const float3x3& rhs) noexcept;
    float3x3 operator*(const float3x3& lhs, const float3x3& rhs) noexcept;
    float3x3 operator*(float3x3 mat, float scalar) noexcept;
    float3x3 operator*(float scalar, float3x3 mat) noexcept;
    float3x3 operator/(float3x3 mat, float scalar) noexcept;
    float3 operator*(const float3& vec, const float3x3& mat) noexcept;
    float3 operator*(const float3x3& mat, const float3& vec) noexcept;

    // ============================================================================
    // Global Functions
    // ============================================================================

    float3x3 transpose(const float3x3& mat) noexcept;
    float3x3 inverse(const float3x3& mat) noexcept;
    float determinant(const float3x3& mat) noexcept;
    float3 mul(const float3& vec, const float3x3& mat) noexcept;
    float3x3 mul(const float3x3& lhs, const float3x3& rhs) noexcept;
    float trace(const float3x3& mat) noexcept;
    float3 diagonal(const float3x3& mat) noexcept;
    float frobenius_norm(const float3x3& mat) noexcept;
    bool approximately(const float3x3& a, const float3x3& b, float epsilon) noexcept;
    bool is_orthogonal(const float3x3& mat, float epsilon) noexcept;
    bool is_orthonormal(const float3x3& mat, float epsilon) noexcept;
    float3x3 normal_matrix(const float3x3& model) noexcept;
    float3 extract_scale(const float3x3& mat) noexcept;
    float3x3 extract_rotation(const float3x3& mat) noexcept;
    float3x3 skew_symmetric(const float3& vec) noexcept;
    float3x3 outer_product(const float3& u, const float3& v) noexcept;

    // ============================================================================
    // Useful Constants
    // ============================================================================

    /**
     * @brief Identity matrix constant
     */
    extern const float3x3 float3x3_Identity;

    /**
     * @brief Zero matrix constant
     */
    extern const float3x3 float3x3_Zero;

} // namespace Math

#include "math_float4x4.h"
#include "math_quaternion.h"
#include "math_float3x3.inl"
