#ifndef MATH_FLOAT2X2_INL
#define MATH_FLOAT2X2_INL

namespace Math
{
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

    inline bool approximately(const float2x2& a, const float2x2& b, float epsilon) noexcept
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

    inline float2x2::float2x2() noexcept
        : row0_(1.0f, 0.0f), row1_(0.0f, 1.0f) {}

    inline float2x2::float2x2(const float2& row0, const float2& row1) noexcept
        : row0_(row0), row1_(row1) {}

    inline float2x2::float2x2(float m00, float m01, float m10, float m11) noexcept
        : row0_(m00, m01), row1_(m10, m11) {}

    inline float2x2::float2x2(const float* data) noexcept
        : row0_(data[0], data[1]), row1_(data[2], data[3]) {}

    inline float2x2::float2x2(float scalar) noexcept
        : row0_(scalar, 0.0f), row1_(0.0f, scalar) {}

    inline float2x2::float2x2(const float2& diagonal) noexcept
        : row0_(diagonal.x, 0.0f), row1_(0.0f, diagonal.y) {}

    inline float2x2::float2x2(__m128 sse_data) noexcept
    {
        set_sse_data(sse_data);
    }

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
        return float2x2(float2(c, -s), float2(s, c));
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
        return float2x2(float2(1.0f, shear.x), float2(shear.y, 1.0f));
    }

    inline float2x2 float2x2::shear(float x, float y) noexcept
    {
        return shear(float2(x, y));
    }

    inline float2& float2x2::operator[](int rowIndex) noexcept
    {
        return (rowIndex == 0) ? row0_ : row1_;
    }

    inline const float2& float2x2::operator[](int rowIndex) const noexcept
    {
        return (rowIndex == 0) ? row0_ : row1_;
    }

    inline float& float2x2::operator()(int row, int col) noexcept
    {
        return (row == 0) ?
            (col == 0 ? row0_.x : row0_.y) :
            (col == 0 ? row1_.x : row1_.y);
    }

    inline const float& float2x2::operator()(int row, int col) const noexcept
    {
        return (row == 0) ?
            (col == 0 ? row0_.x : row0_.y) :
            (col == 0 ? row1_.x : row1_.y);
    }

    inline float2 float2x2::row0() const noexcept { return row0_; }
    inline float2 float2x2::row1() const noexcept { return row1_; }

    inline float2 float2x2::col0() const noexcept { return float2(row0_.x, row1_.x); }
    inline float2 float2x2::col1() const noexcept { return float2(row0_.y, row1_.y); }

    inline void float2x2::set_row0(const float2& row) noexcept { row0_ = row; }
    inline void float2x2::set_row1(const float2& row) noexcept { row1_ = row; }

    inline void float2x2::set_col0(const float2& col) noexcept
    {
        row0_.x = col.x;
        row1_.x = col.y;
    }

    inline void float2x2::set_col1(const float2& col) noexcept
    {
        row0_.y = col.x;
        row1_.y = col.y;
    }

    inline __m128 float2x2::sse_data() const noexcept
    {
        return _mm_setr_ps(row0_.x, row0_.y, row1_.x, row1_.y);
    }

    inline void float2x2::set_sse_data(__m128 sse_data) noexcept
    {
        float temp[4];
        _mm_store_ps(temp, sse_data);
        row0_.x = temp[0]; row0_.y = temp[1];
        row1_.x = temp[2]; row1_.y = temp[3];
    }

    inline float2x2& float2x2::operator+=(const float2x2& rhs) noexcept
    {
        __m128 result = _mm_add_ps(sse_data(), rhs.sse_data());
        set_sse_data(result);
        return *this;
    }

    inline float2x2& float2x2::operator-=(const float2x2& rhs) noexcept
    {
        __m128 result = _mm_sub_ps(sse_data(), rhs.sse_data());
        set_sse_data(result);
        return *this;
    }

    inline float2x2& float2x2::operator*=(float scalar) noexcept
    {
        row0_ *= scalar;
        row1_ *= scalar;
        return *this;
    }

    inline float2x2& float2x2::operator/=(float scalar) noexcept
    {
        float inv_scalar = 1.0f / scalar;
        row0_ *= inv_scalar;
        row1_ *= inv_scalar;
        return *this;
    }

    inline float2x2& float2x2::operator*=(const float2x2& rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    inline float2x2 float2x2::operator+() const noexcept { return *this; }

    inline float2x2 float2x2::operator-() const noexcept
    {
        return float2x2(-row0_, -row1_);
    }

    inline float2x2 float2x2::transposed() const noexcept
    {
        return float2x2(
            float2(row0_.x, row1_.x),
            float2(row0_.y, row1_.y)
        );
    }

    inline float float2x2::determinant() const noexcept
    {
        return row0_.x * row1_.y - row0_.y * row1_.x;
    }

    inline float2x2 float2x2::inverted() const noexcept
    {
        float det = determinant();
        if (std::abs(det) < Constants::Constants<float>::Epsilon)
        {
            return identity();
        }

        float inv_det = 1.0f / det;
        return float2x2(
            float2(row1_.y, -row0_.y) * inv_det,
            float2(-row1_.x, row0_.x) * inv_det
        );
    }

    inline float2x2 float2x2::adjugate() const noexcept
    {
        return float2x2(
            float2(row1_.y, -row0_.y),
            float2(-row1_.x, row0_.x)
        );
    }

    inline float float2x2::trace() const noexcept
    {
        return row0_.x + row1_.y;
    }

    inline float2 float2x2::diagonal() const noexcept
    {
        return float2(row0_.x, row1_.y);
    }

    inline float float2x2::frobenius_norm() const noexcept
    {
        return std::sqrt(row0_.length_sq() + row1_.length_sq());
    }

    inline float2 float2x2::transform_vector(const float2& vec) const noexcept
    {
        return float2(
            row0_.x * vec.x + row0_.y * vec.y,
            row1_.x * vec.x + row1_.y * vec.y
        );
    }

    inline float2 float2x2::transform_point(const float2& point) const noexcept
    {
        return transform_vector(point);
    }

    inline float float2x2::get_rotation() const noexcept
    {
        if (!is_rotation()) {
            return 0.0f;
        }

        return std::atan2(row1_.x, row0_.x);
    }

    inline float2 float2x2::get_scale() const noexcept
    {
        return float2(col0().length(), col1().length());
    }

    inline void float2x2::set_rotation(float angle) noexcept
    {
        float2 current_scale = get_scale();
        bool has_reflection = determinant() < 0.0f;

        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);

        if (has_reflection) {
            s = -s;
        }

        set_row0(float2(c, -s) * current_scale.x);
        set_row1(float2(s, c) * current_scale.y);
    }

    inline void float2x2::set_scale(const float2& scale) noexcept
    {
        float2 current_scale = get_scale();
        if (current_scale.x > 0) set_col0(col0() * (scale.x / current_scale.x));
        if (current_scale.y > 0) set_col1(col1() * (scale.y / current_scale.y));
    }

    inline bool float2x2::is_identity(float epsilon) const noexcept
    {
        return row0_.approximately(float2(1.0f, 0.0f), epsilon) &&
            row1_.approximately(float2(0.0f, 1.0f), epsilon);
    }

    inline bool float2x2::is_orthogonal(float epsilon) const noexcept
    {
        return MathFunctions::approximately(dot(col0(), col1()), 0.0f, epsilon);
    }

    inline bool float2x2::is_rotation(float epsilon) const noexcept
    {
        return is_orthogonal(epsilon) &&
            MathFunctions::approximately(col0().length_sq(), 1.0f, epsilon) &&
            MathFunctions::approximately(col1().length_sq(), 1.0f, epsilon) &&
            MathFunctions::approximately(determinant(), 1.0f, epsilon);
    }

    inline bool float2x2::approximately(const float2x2& other, float epsilon) const noexcept
    {
        return row0_.approximately(other.row0_, epsilon) &&
            row1_.approximately(other.row1_, epsilon);
    }

    inline bool float2x2::approximately_zero(float epsilon) const noexcept
    {
        return row0_.approximately_zero(epsilon) &&
            row1_.approximately_zero(epsilon);
    }

    inline std::string float2x2::to_string() const
    {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer),
            "[%8.4f, %8.4f]\n"
            "[%8.4f, %8.4f]",
            row0_.x, row0_.y,
            row1_.x, row1_.y);
        return std::string(buffer);
    }

    inline void float2x2::to_row_major(float* data) const noexcept
    {
        data[0] = row0_.x;
        data[1] = row0_.y;
        data[2] = row1_.x;
        data[3] = row1_.y;
    }

    inline void float2x2::to_column_major(float* data) const noexcept
    {
        data[0] = row0_.x;
        data[1] = row1_.x;
        data[2] = row0_.y;
        data[3] = row1_.y;
    }

    inline bool float2x2::operator==(const float2x2& rhs) const noexcept
    {
        return approximately(rhs);
    }

    inline bool float2x2::operator!=(const float2x2& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    inline float2x2 operator*(const float2x2& lhs, const float2x2& rhs) noexcept
    {
        __m128 lhs_data = lhs.sse_data();
        __m128 rhs_data = rhs.sse_data();

        __m128 rhs_row0 = _mm_shuffle_ps(rhs_data, rhs_data, _MM_SHUFFLE(0, 0, 0, 0));
        __m128 lhs_swizzled0 = _mm_shuffle_ps(lhs_data, lhs_data, _MM_SHUFFLE(0, 0, 1, 1));
        __m128 part0 = _mm_mul_ps(lhs_swizzled0, rhs_row0);

        __m128 rhs_row0_second = _mm_shuffle_ps(rhs_data, rhs_data, _MM_SHUFFLE(1, 1, 1, 1));
        __m128 lhs_swizzled1 = _mm_shuffle_ps(lhs_data, lhs_data, _MM_SHUFFLE(2, 2, 3, 3));
        __m128 part1 = _mm_mul_ps(lhs_swizzled1, rhs_row0_second);

        __m128 result_row0 = _mm_add_ps(part0, part1);

        __m128 rhs_row1 = _mm_shuffle_ps(rhs_data, rhs_data, _MM_SHUFFLE(2, 2, 2, 2));
        __m128 part2 = _mm_mul_ps(lhs_swizzled0, rhs_row1);

        __m128 rhs_row1_second = _mm_shuffle_ps(rhs_data, rhs_data, _MM_SHUFFLE(3, 3, 3, 3));
        __m128 part3 = _mm_mul_ps(lhs_swizzled1, rhs_row1_second);

        __m128 result_row1 = _mm_add_ps(part2, part3);

        __m128 result = _mm_shuffle_ps(result_row0, result_row1, _MM_SHUFFLE(2, 0, 2, 0));

        return float2x2(result);
    }

    inline const float2x2 float2x2_Identity = float2x2::identity();
    inline const float2x2 float2x2_Zero = float2x2::zero();

}
#endif
