#ifndef MATH_FLOAT2X2_INL
#define MATH_FLOAT2X2_INL

namespace Math
{
    // ============================================================================
    // Binary Operators
    // ============================================================================

    inline float2x2 operator+(float2x2 lhs, const float2x2& rhs) noexcept
    {
        return lhs += rhs;
    }

    inline float2x2 operator-(float2x2 lhs, const float2x2& rhs) noexcept
    {
        return lhs -= rhs;
    }

    inline float2x2 operator*(float2x2 mat, float scalar) noexcept
    {
        return mat *= scalar;
    }

    inline float2x2 operator*(float scalar, float2x2 mat) noexcept
    {
        return mat *= scalar;
    }

    inline float2x2 operator/(float2x2 mat, float scalar) noexcept
    {
        return mat /= scalar;
    }

    inline float2 operator*(const float2& vec, const float2x2& mat) noexcept
    {
        return mat.transform_vector(vec);
    }

    // ============================================================================
    // Global Functions
    // ============================================================================

    inline float2x2 transpose(const float2x2& mat) noexcept
    {
        return mat.transposed();
    }

    inline float2x2 inverse(const float2x2& mat) noexcept
    {
        return mat.inverted();
    }

    inline float determinant(const float2x2& mat) noexcept
    {
        return mat.determinant();
    }

    inline float2 mul(const float2& vec, const float2x2& mat) noexcept
    {
        return vec * mat;
    }

    inline float2x2 mul(const float2x2& lhs, const float2x2& rhs) noexcept
    {
        return lhs * rhs;
    }

    inline float trace(const float2x2& mat) noexcept
    {
        return mat.trace();
    }

    inline float2 diagonal(const float2x2& mat) noexcept
    {
        return mat.diagonal();
    }

    inline float frobenius_norm(const float2x2& mat) noexcept
    {
        return mat.frobenius_norm();
    }

    inline bool approximately(const float2x2& a, const float2x2& b,
        float epsilon) noexcept
    {
        return a.approximately(b, epsilon);
    }

    inline bool is_orthogonal(const float2x2& mat, float epsilon) noexcept
    {
        return mat.is_orthogonal(epsilon);
    }

    inline bool is_rotation(const float2x2& mat, float epsilon) noexcept
    {
        return mat.is_rotation(epsilon);
    }

    // ============================================================================
    // Constructors
    // ============================================================================

    inline float2x2::float2x2() noexcept
        : col0_(1.0f, 0.0f), col1_(0.0f, 1.0f) {}

    inline float2x2::float2x2(const float2& col0, const float2& col1) noexcept
        : col0_(col0), col1_(col1) {}

    inline float2x2::float2x2(float col0x, float col0y, float col1x, float col1y) noexcept
        : col0_(col0x, col0y), col1_(col1x, col1y) {}

    inline float2x2::float2x2(const float* data) noexcept
        : col0_(data[0], data[1]), col1_(data[2], data[3]) {}

    inline float2x2::float2x2(float scalar) noexcept
        : col0_(scalar, 0.0f), col1_(0.0f, scalar) {}

    inline float2x2::float2x2(const float2& diagonal) noexcept
        : col0_(diagonal.x, 0.0f), col1_(0.0f, diagonal.y) {}

    inline float2x2::float2x2(__m128 sse_data) noexcept
    {
        set_sse_data(sse_data);
    }

    // ============================================================================
    // Static Constructors
    // ============================================================================

    inline float2x2 float2x2::identity() noexcept
    {
        return float2x2(float2(1.0f, 0.0f), float2(0.0f, 1.0f));
    }

    inline float2x2 float2x2::zero() noexcept
    {
        return float2x2(float2(0.0f, 0.0f), float2(0.0f, 0.0f));
    }

    inline float2x2 float2x2::rotation(float angle) noexcept
    {
        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);
        return float2x2(float2(c, s), float2(-s, c));
    }

    inline float2x2 float2x2::scaling(const float2& scale) noexcept
    {
        return float2x2(float2(scale.x, 0.0f), float2(0.0f, scale.y));
    }

    inline float2x2 float2x2::scaling(float x, float y) noexcept
    {
        return scaling(float2(x, y));
    }

    inline float2x2 float2x2::scaling(float uniformScale) noexcept
    {
        return scaling(float2(uniformScale, uniformScale));
    }

    inline float2x2 float2x2::shear(const float2& shear) noexcept
    {
        return float2x2(float2(1.0f, shear.y), float2(shear.x, 1.0f));
    }

    inline float2x2 float2x2::shear(float x, float y) noexcept
    {
        return shear(float2(x, y));
    }

    // ============================================================================
    // Access Operators
    // ============================================================================

    inline float2& float2x2::operator[](int colIndex) noexcept
    {
        return (colIndex == 0) ? col0_ : col1_;
    }

    inline const float2& float2x2::operator[](int colIndex) const noexcept
    {
        return (colIndex == 0) ? col0_ : col1_;
    }

    inline float& float2x2::operator()(int row, int col) noexcept
    {
        return (col == 0) ?
            (row == 0 ? col0_.x : col0_.y) :
            (row == 0 ? col1_.x : col1_.y);
    }

    inline const float& float2x2::operator()(int row, int col) const noexcept
    {
        return (col == 0) ?
            (row == 0 ? col0_.x : col0_.y) :
            (row == 0 ? col1_.x : col1_.y);
    }

    // ============================================================================
    // Column and Row Accessors
    // ============================================================================

    inline float2 float2x2::col0() const noexcept { return col0_; }
    inline float2 float2x2::col1() const noexcept { return col1_; }

    inline float2 float2x2::row0() const noexcept { return float2(col0_.x, col1_.x); }
    inline float2 float2x2::row1() const noexcept { return float2(col0_.y, col1_.y); }

    inline void float2x2::set_col0(const float2& col) noexcept { col0_ = col; }
    inline void float2x2::set_col1(const float2& col) noexcept { col1_ = col; }

    inline void float2x2::set_row0(const float2& row) noexcept
    {
        col0_.x = row.x;
        col1_.x = row.y;
    }

    inline void float2x2::set_row1(const float2& row) noexcept
    {
        col0_.y = row.x;
        col1_.y = row.y;
    }

    // ============================================================================
    // SSE Accessors
    // ============================================================================

    inline __m128 float2x2::sse_data() const noexcept
    {
        return _mm_setr_ps(col0_.x, col0_.y, col1_.x, col1_.y);
    }

    inline void float2x2::set_sse_data(__m128 sse_data) noexcept
    {
        float temp[4];
        _mm_store_ps(temp, sse_data);
        col0_.x = temp[0]; col0_.y = temp[1];
        col1_.x = temp[2]; col1_.y = temp[3];
    }

    // ============================================================================
    // Compound Assignment Operators
    // ============================================================================

    inline float2x2& float2x2::operator+=(const float2x2& rhs) noexcept
    {
        __m128 result = _mm_add_ps(this->sse_data(), rhs.sse_data());
        this->set_sse_data(result);
        return *this;
    }

    inline float2x2& float2x2::operator-=(const float2x2& rhs) noexcept
    {
        __m128 result = _mm_sub_ps(this->sse_data(), rhs.sse_data());
        this->set_sse_data(result);
        return *this;
    }

    inline float2x2& float2x2::operator*=(float scalar) noexcept
    {
        col0_ *= scalar;
        col1_ *= scalar;
        return *this;
    }

    inline float2x2& float2x2::operator/=(float scalar) noexcept
    {
        const float inv_scalar = 1.0f / scalar;
        col0_ *= inv_scalar;
        col1_ *= inv_scalar;
        return *this;
    }

    inline float2x2& float2x2::operator*=(const float2x2& rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    // ============================================================================
    // Unary Operators
    // ============================================================================

    inline float2x2 float2x2::operator+() const noexcept { return *this; }

    inline float2x2 float2x2::operator-() const noexcept
    {
        return float2x2(-col0_, -col1_);
    }

    // ============================================================================
    // Matrix Operations
    // ============================================================================

    inline float2x2 float2x2::transposed() const noexcept
    {
        return float2x2(
            float2(col0_.x, col1_.x),
            float2(col0_.y, col1_.y)
        );
    }

    inline float float2x2::determinant() const noexcept
    {
        return col0_.x * col1_.y - col0_.y * col1_.x;
    }

    inline float2x2 float2x2::inverted() const noexcept
    {
        const float det = determinant();
        if (std::abs(det) < Constants::Constants<float>::Epsilon)
        {
            return identity();
        }

        const float inv_det = 1.0f / det;
        return float2x2(
            float2(col1_.y, -col0_.y) * inv_det,
            float2(-col1_.x, col0_.x) * inv_det
        );
    }

    inline float2x2 float2x2::adjugate() const noexcept
    {
        return float2x2(
            float2(col1_.y, -col0_.y),
            float2(-col1_.x, col0_.x)
        );
    }

    inline float float2x2::trace() const noexcept
    {
        return col0_.x + col1_.y;
    }

    inline float2 float2x2::diagonal() const noexcept
    {
        return float2(col0_.x, col1_.y);
    }

    inline float float2x2::frobenius_norm() const noexcept
    {
        return std::sqrt(col0_.length_sq() + col1_.length_sq());
    }

    // ============================================================================
    // Vector Transformations
    // ============================================================================

    inline float2 float2x2::transform_vector(const float2& vec) const noexcept
    {
        return float2(
            col0_.x * vec.x + col1_.x * vec.y,
            col0_.y * vec.x + col1_.y * vec.y
        );
    }

    inline float2 float2x2::transform_point(const float2& point) const noexcept
    {
        return transform_vector(point);
    }

    // ============================================================================
    // Transformation Component Extraction
    // ============================================================================

    inline float float2x2::get_rotation() const noexcept
    {
        float2 x_axis = col0_.normalize();
        return std::atan2(x_axis.y, x_axis.x);
    }

    inline float2 float2x2::get_scale() const noexcept
    {
        return float2(col0_.length(), col1_.length());
    }

    inline void float2x2::set_rotation(float angle) noexcept
    {
        float2 current_scale = get_scale();
        float cos_angle = std::cos(angle);
        float sin_angle = std::sin(angle);

        col0_ = float2(cos_angle, sin_angle) * current_scale.x;
        col1_ = float2(-sin_angle, cos_angle) * current_scale.y;
    }

    inline void float2x2::set_scale(const float2& scale) noexcept
    {
        float2 current_scale = get_scale();
        if (current_scale.x > 0) col0_ = col0_ * (scale.x / current_scale.x);
        if (current_scale.y > 0) col1_ = col1_ * (scale.y / current_scale.y);
    }

    // ============================================================================
    // Utility Methods
    // ============================================================================

    inline bool float2x2::is_identity(float epsilon) const noexcept
    {
        return col0_.approximately(float2(1.0f, 0.0f), epsilon) &&
            col1_.approximately(float2(0.0f, 1.0f), epsilon);
    }

    inline bool float2x2::is_orthogonal(float epsilon) const noexcept
    {
        return MathFunctions::approximately(dot(col0_, col1_), 0.0f, epsilon);
    }

    inline bool float2x2::is_rotation(float epsilon) const noexcept
    {
        return is_orthogonal(epsilon) &&
            MathFunctions::approximately(col0_.length_sq(), 1.0f, epsilon) &&
            MathFunctions::approximately(col1_.length_sq(), 1.0f, epsilon) &&
            MathFunctions::approximately(determinant(), 1.0f, epsilon);
    }

    inline bool float2x2::approximately(const float2x2& other, float epsilon) const noexcept
    {
        return col0_.approximately(other.col0_, epsilon) &&
            col1_.approximately(other.col1_, epsilon);
    }

    inline bool float2x2::approximately_zero(float epsilon) const noexcept
    {
        return col0_.approximately_zero(epsilon) &&
            col1_.approximately_zero(epsilon);
    }

    inline std::string float2x2::to_string() const
    {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer),
            "[%8.4f, %8.4f]\n"
            "[%8.4f, %8.4f]",
            col0_.x, col1_.x,
            col0_.y, col1_.y);
        return std::string(buffer);
    }

    inline void float2x2::to_column_major(float* data) const noexcept
    {
        data[0] = col0_.x;
        data[1] = col0_.y;
        data[2] = col1_.x;
        data[3] = col1_.y;
    }

    inline void float2x2::to_row_major(float* data) const noexcept
    {
        data[0] = col0_.x;
        data[1] = col1_.x;
        data[2] = col0_.y;
        data[3] = col1_.y;
    }

    // ============================================================================
    // Comparison Operators
    // ============================================================================

    inline bool float2x2::operator==(const float2x2& rhs) const noexcept
    {
        return approximately(rhs);
    }

    inline bool float2x2::operator!=(const float2x2& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    // ============================================================================
    // Matrix Multiplication (non-member, but defined in .inl)
    // ============================================================================

    inline float2x2 operator*(const float2x2& lhs, const float2x2& rhs) noexcept
    {
        __m128 lhs_data = lhs.sse_data();
        __m128 rhs_data = rhs.sse_data();

        // SSE реализация матричного умножения
        __m128 rhs_col0 = _mm_shuffle_ps(rhs_data, rhs_data, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 lhs_swizzled0 = _mm_shuffle_ps(lhs_data, lhs_data, _MM_SHUFFLE(1, 0, 1, 0));
        __m128 part0 = _mm_mul_ps(lhs_swizzled0, rhs_col0);

        __m128 rhs_col0_second = _mm_shuffle_ps(rhs_data, rhs_data, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 lhs_swizzled1 = _mm_shuffle_ps(lhs_data, lhs_data, _MM_SHUFFLE(3, 2, 3, 2));
        __m128 part1 = _mm_mul_ps(lhs_swizzled1, rhs_col0_second);

        __m128 result_col0 = _mm_add_ps(part0, part1);

        __m128 rhs_col1 = _mm_shuffle_ps(rhs_data, rhs_data, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 part2 = _mm_mul_ps(lhs_swizzled0, rhs_col1);

        __m128 rhs_col1_second = _mm_shuffle_ps(rhs_data, rhs_data, _MM_SHUFFLE(3, 3, 3, 3));
        __m128 part3 = _mm_mul_ps(lhs_swizzled1, rhs_col1_second);

        __m128 result_col1 = _mm_add_ps(part2, part3);

        __m128 result = _mm_shuffle_ps(result_col0, result_col1, _MM_SHUFFLE(1, 0, 1, 0));

        return float2x2(result);
    }

    // ============================================================================
    // Useful Constants
    // ============================================================================

    const float2x2 float2x2_Identity = float2x2::identity();
    const float2x2 float2x2_Zero = float2x2::zero();

} // namespace Math

#endif // MATH_FLOAT2X2_INL
