#ifndef MATH_FLOAT3X3_INL
#define MATH_FLOAT3X3_INL

namespace Math
{
    inline float3x3 operator+(float3x3 lhs, const float3x3& rhs) noexcept
    {
        return lhs += rhs;
    }

    inline float3x3 operator-(float3x3 lhs, const float3x3& rhs) noexcept
    {
        return lhs -= rhs;
    }

    inline float3x3 operator*(float3x3 mat, float scalar) noexcept
    {
        return mat *= scalar;
    }

    inline float3x3 operator*(float scalar, float3x3 mat) noexcept
    {
        return mat *= scalar;
    }

    inline float3x3 operator/(float3x3 mat, float scalar) noexcept
    {
        return mat /= scalar;
    }

    inline float3 operator*(const float3& vec, const float3x3& mat) noexcept
    {
        // Умножение вектора-строки на матрицу (вектор слева)
        return float3(
            vec.x * mat(0, 0) + vec.y * mat(1, 0) + vec.z * mat(2, 0),
            vec.x * mat(0, 1) + vec.y * mat(1, 1) + vec.z * mat(2, 1),
            vec.x * mat(0, 2) + vec.y * mat(1, 2) + vec.z * mat(2, 2)
        );
    }

    inline float3 operator*(const float3x3& mat, const float3& vec) noexcept
    {
        return mat.transform_vector(vec);
    }

    inline float3x3 transpose(const float3x3& mat) noexcept
    {
        return mat.transposed();
    }

    inline float3x3 inverse(const float3x3& mat) noexcept
    {
        return mat.inverted();
    }

    inline float determinant(const float3x3& mat) noexcept
    {
        return mat.determinant();
    }

    inline float3 mul(const float3& vec, const float3x3& mat) noexcept
    {
        return vec * mat;
    }

    inline float3x3 mul(const float3x3& lhs, const float3x3& rhs) noexcept
    {
        return lhs * rhs;
    }

    inline float trace(const float3x3& mat) noexcept
    {
        return mat.trace();
    }

    inline float3 diagonal(const float3x3& mat) noexcept
    {
        return mat.diagonal();
    }

    inline float frobenius_norm(const float3x3& mat) noexcept
    {
        return mat.frobenius_norm();
    }

    inline bool approximately(const float3x3& a, const float3x3& b, float epsilon) noexcept
    {
        return a.approximately(b, epsilon);
    }

    inline bool is_orthogonal(const float3x3& mat, float epsilon) noexcept
    {
        return mat.is_orthogonal(epsilon);
    }

    inline bool is_orthonormal(const float3x3& mat, float epsilon) noexcept
    {
        return mat.is_orthonormal(epsilon);
    }

    inline float3x3 normal_matrix(const float3x3& model) noexcept
    {
        return float3x3::normal_matrix(model);
    }

    inline float3 extract_scale(const float3x3& mat) noexcept
    {
        return mat.extract_scale();
    }

    inline float3x3 extract_rotation(const float3x3& mat) noexcept
    {
        return mat.extract_rotation();
    }

    inline float3x3 skew_symmetric(const float3& vec) noexcept
    {
        return float3x3::skew_symmetric(vec);
    }

    inline float3x3 outer_product(const float3& u, const float3& v) noexcept
    {
        return float3x3::outer_product(u, v);
    }

    inline float3x3::float3x3() noexcept
        : row0_(1.0f, 0.0f, 0.0f, 0.0f),
        row1_(0.0f, 1.0f, 0.0f, 0.0f),
        row2_(0.0f, 0.0f, 1.0f, 0.0f)
    {}

    inline float3x3::float3x3(const float3& row0, const float3& row1, const float3& row2) noexcept
    {
        row0_ = float4(row0.x, row0.y, row0.z, 0.0f);
        row1_ = float4(row1.x, row1.y, row1.z, 0.0f);
        row2_ = float4(row2.x, row2.y, row2.z, 0.0f);
    }

    inline float3x3::float3x3(float m00, float m01, float m02,
        float m10, float m11, float m12,
        float m20, float m21, float m22) noexcept
    {
        row0_ = float4(m00, m01, m02, 0.0f);
        row1_ = float4(m10, m11, m12, 0.0f);
        row2_ = float4(m20, m21, m22, 0.0f);
    }

    inline float3x3::float3x3(const float* data) noexcept
    {
        row0_ = float4(data[0], data[1], data[2], 0.0f);
        row1_ = float4(data[3], data[4], data[5], 0.0f);
        row2_ = float4(data[6], data[7], data[8], 0.0f);
    }

    inline float3x3::float3x3(float scalar) noexcept
        : row0_(scalar, 0.0f, 0.0f, 0.0f),
        row1_(0.0f, scalar, 0.0f, 0.0f),
        row2_(0.0f, 0.0f, scalar, 0.0f)
    {}

    inline float3x3::float3x3(const float3& diagonal) noexcept
        : row0_(diagonal.x, 0.0f, 0.0f, 0.0f),
        row1_(0.0f, diagonal.y, 0.0f, 0.0f),
        row2_(0.0f, 0.0f, diagonal.z, 0.0f)
    {}

    inline float3x3::float3x3(const float4x4& mat4x4) noexcept
    {
        const __m128 mask = _mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f);
        row0_ = float4(_mm_and_ps(mat4x4.row0().get_simd(), mask));
        row1_ = float4(_mm_and_ps(mat4x4.row1().get_simd(), mask));
        row2_ = float4(_mm_and_ps(mat4x4.row2().get_simd(), mask));
    }

    inline float3x3::float3x3(const quaternion& q) noexcept
    {
        const float xx = q.x * q.x;
        const float yy = q.y * q.y;
        const float zz = q.z * q.z;
        const float xy = q.x * q.y;
        const float xz = q.x * q.z;
        const float yz = q.y * q.z;
        const float wx = q.w * q.x;
        const float wy = q.w * q.y;
        const float wz = q.w * q.z;

        row0_ = float4(1.0f - 2.0f * (yy + zz), 2.0f * (xy - wz), 2.0f * (xz + wy), 0.0f);
        row1_ = float4(2.0f * (xy + wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz - wx), 0.0f);
        row2_ = float4(2.0f * (xz - wy), 2.0f * (yz + wx), 1.0f - 2.0f * (xx + yy), 0.0f);
    }

    inline float3x3& float3x3::operator=(const float4x4& mat4x4) noexcept
    {
        const __m128 mask = _mm_set_ps(0.0f, 1.0f, 1.0f, 1.0f);
        row0_ = float4(_mm_and_ps(mat4x4.row0().get_simd(), mask));
        row1_ = float4(_mm_and_ps(mat4x4.row1().get_simd(), mask));
        row2_ = float4(_mm_and_ps(mat4x4.row2().get_simd(), mask));
        return *this;
    }

    inline float3& float3x3::operator[](int rowIndex) noexcept
    {
        assert(rowIndex >= 0 && rowIndex < 3 && "Matrix row index out of bounds");

        switch (rowIndex) {
        case 0: return *reinterpret_cast<float3*>(&row0_);
        case 1: return *reinterpret_cast<float3*>(&row1_);
        case 2: return *reinterpret_cast<float3*>(&row2_);
        default:
            return *reinterpret_cast<float3*>(&row0_);
        }
    }

    inline const float3& float3x3::operator[](int rowIndex) const noexcept
    {
        assert(rowIndex >= 0 && rowIndex < 3 && "Matrix row index out of bounds");

        switch (rowIndex) {
        case 0: return *reinterpret_cast<const float3*>(&row0_);
        case 1: return *reinterpret_cast<const float3*>(&row1_);
        case 2: return *reinterpret_cast<const float3*>(&row2_);
        default:
            return *reinterpret_cast<const float3*>(&row0_);
        }
    }

    inline float& float3x3::operator()(int row, int col) noexcept
    {
        switch (row) {
        case 0:
            switch (col) {
            case 0: return row0_.x;
            case 1: return row0_.y;
            case 2: return row0_.z;
            }
        case 1:
            switch (col) {
            case 0: return row1_.x;
            case 1: return row1_.y;
            case 2: return row1_.z;
            }
        case 2:
            switch (col) {
            case 0: return row2_.x;
            case 1: return row2_.y;
            case 2: return row2_.z;
            }
        }
        return row0_.x;
    }

    inline const float& float3x3::operator()(int row, int col) const noexcept
    {
        switch (row) {
        case 0:
            switch (col) {
            case 0: return row0_.x;
            case 1: return row0_.y;
            case 2: return row0_.z;
            }
        case 1:
            switch (col) {
            case 0: return row1_.x;
            case 1: return row1_.y;
            case 2: return row1_.z;
            }
        case 2:
            switch (col) {
            case 0: return row2_.x;
            case 1: return row2_.y;
            case 2: return row2_.z;
            }
        }
        return row0_.x;
    }

    inline float3 float3x3::row0() const noexcept { return float3(row0_.x, row0_.y, row0_.z); }
    inline float3 float3x3::row1() const noexcept { return float3(row1_.x, row1_.y, row1_.z); }
    inline float3 float3x3::row2() const noexcept { return float3(row2_.x, row2_.y, row2_.z); }

    inline float3 float3x3::col0() const noexcept { return float3(row0_.x, row1_.x, row2_.x); }
    inline float3 float3x3::col1() const noexcept { return float3(row0_.y, row1_.y, row2_.y); }
    inline float3 float3x3::col2() const noexcept { return float3(row0_.z, row1_.z, row2_.z); }

    inline void float3x3::set_row0(const float3& row) noexcept
    {
        row0_.x = row.x;
        row0_.y = row.y;
        row0_.z = row.z;
    }

    inline void float3x3::set_row1(const float3& row) noexcept
    {
        row1_.x = row.x;
        row1_.y = row.y;
        row1_.z = row.z;
    }

    inline void float3x3::set_row2(const float3& row) noexcept
    {
        row2_.x = row.x;
        row2_.y = row.y;
        row2_.z = row.z;
    }

    inline void float3x3::set_col0(const float3& col) noexcept
    {
        row0_.x = col.x;
        row1_.x = col.y;
        row2_.x = col.z;
    }

    inline void float3x3::set_col1(const float3& col) noexcept
    {
        row0_.y = col.x;
        row1_.y = col.y;
        row2_.y = col.z;
    }

    inline void float3x3::set_col2(const float3& col) noexcept
    {
        row0_.z = col.x;
        row1_.z = col.y;
        row2_.z = col.z;
    }

    inline float3x3 float3x3::identity() noexcept
    {
        return float3x3(float3(1.0f, 0.0f, 0.0f),
            float3(0.0f, 1.0f, 0.0f),
            float3(0.0f, 0.0f, 1.0f));
    }

    inline float3x3 float3x3::zero() noexcept
    {
        return float3x3(float3(0.0f, 0.0f, 0.0f),
            float3(0.0f, 0.0f, 0.0f),
            float3(0.0f, 0.0f, 0.0f));
    }

    inline bool float3x3::approximately_zero(float epsilon) const noexcept
    {
        return row0_.approximately_zero(epsilon) &&
            row1_.approximately_zero(epsilon) &&
            row2_.approximately_zero(epsilon);
    }

    inline float3x3 float3x3::scaling(const float3& scale) noexcept
    {
        return float3x3(float3(scale.x, 0.0f, 0.0f),
            float3(0.0f, scale.y, 0.0f),
            float3(0.0f, 0.0f, scale.z));
    }

    inline float3x3 float3x3::scaling(float scaleX, float scaleY, float scaleZ) noexcept
    {
        return scaling(float3(scaleX, scaleY, scaleZ));
    }

    inline float3x3 float3x3::scaling(float scale) noexcept
    {
        return float3x3(scale);
    }

    inline float3x3 float3x3::rotation_x(float angle) noexcept
    {
        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);

        return float3x3(
            float3(1.0f, 0.0f, 0.0f),
            float3(0.0f, c, -s),
            float3(0.0f, s, c)
        );
    }

    inline float3x3 float3x3::rotation_y(float angle) noexcept
    {
        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);

        return float3x3(
            float3(c, 0.0f, s),
            float3(0.0f, 1.0f, 0.0f),
            float3(-s, 0.0f, c)
        );
    }

    inline float3x3 float3x3::rotation_z(float angle) noexcept
    {
        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);

        return float3x3(
            float3(c, -s, 0.0f),
            float3(s, c, 0.0f),
            float3(0.0f, 0.0f, 1.0f)
        );
    }

    inline float3x3 float3x3::rotation_axis(const float3& axis, float angle) noexcept
    {
        if (axis.approximately_zero(Constants::Constants<float>::Epsilon)) {
            return identity();
        }

        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);

        const float one_minus_c = 1.0f - c;
        const float3 n = axis.normalize();

        const float x = n.x, y = n.y, z = n.z;
        const float xx = x * x, yy = y * y, zz = z * z;
        const float xy = x * y, xz = x * z, yz = y * z;
        const float xs = x * s, ys = y * s, zs = z * s;

        float m00 = xx * one_minus_c + c;
        float m01 = xy * one_minus_c - zs;
        float m02 = xz * one_minus_c + ys;

        float m10 = xy * one_minus_c + zs;
        float m11 = yy * one_minus_c + c;
        float m12 = yz * one_minus_c - xs;

        float m20 = xz * one_minus_c - ys;
        float m21 = yz * one_minus_c + xs;
        float m22 = zz * one_minus_c + c;

        return float3x3(
            float3(m00, m01, m02),
            float3(m10, m11, m12),
            float3(m20, m21, m22)
        );
    }

    inline float3x3 float3x3::rotation_euler(const float3& angles) noexcept
    {
        return rotation_z(angles.z) * rotation_y(angles.y) * rotation_x(angles.x);
    }

    inline float3x3 float3x3::skew_symmetric(const float3& vec) noexcept
    {
        // [v]× * w = cross(v, w)
        return float3x3(
            float3(0.0f, -vec.z, vec.y),
            float3(vec.z, 0.0f, -vec.x),
            float3(-vec.y, vec.x, 0.0f)
        );
    }

    inline float3x3 float3x3::outer_product(const float3& u, const float3& v) noexcept
    {
        return float3x3(
            float3(u.x * v.x, u.x * v.y, u.x * v.z),
            float3(u.y * v.x, u.y * v.y, u.y * v.z),
            float3(u.z * v.x, u.z * v.y, u.z * v.z)
        );
    }

    inline float3x3& float3x3::operator+=(const float3x3& rhs) noexcept
    {
        row0_ += rhs.row0_;
        row1_ += rhs.row1_;
        row2_ += rhs.row2_;
        return *this;
    }

    inline float3x3& float3x3::operator-=(const float3x3& rhs) noexcept
    {
        row0_ -= rhs.row0_;
        row1_ -= rhs.row1_;
        row2_ -= rhs.row2_;
        return *this;
    }

    inline float3x3& float3x3::operator*=(float scalar) noexcept
    {
        row0_ *= scalar;
        row1_ *= scalar;
        row2_ *= scalar;
        return *this;
    }

    inline float3x3& float3x3::operator/=(float scalar) noexcept
    {
        const float inv_scalar = 1.0f / scalar;
        row0_ *= inv_scalar;
        row1_ *= inv_scalar;
        row2_ *= inv_scalar;
        return *this;
    }

    inline float3x3& float3x3::operator*=(const float3x3& rhs) noexcept
    {
        *this = *this * rhs;
        return *this;
    }

    inline float3x3 float3x3::operator+() const noexcept { return *this; }

    inline float3x3 float3x3::operator-() const noexcept
    {
        return float3x3(-row0_.xyz(), -row1_.xyz(), -row2_.xyz());
    }

    inline float3x3 float3x3::transposed() const noexcept
    {
        return float3x3(col0(), col1(), col2());
    }

    inline float float3x3::determinant() const noexcept
    {
        const float3 r0 = row0_.xyz();
        const float3 r1 = row1_.xyz();
        const float3 r2 = row2_.xyz();

        const float a = r0.x, b = r0.y, c = r0.z;
        const float d = r1.x, e = r1.y, f = r1.z;
        const float g = r2.x, h = r2.y, i = r2.z;

        return a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);
    }

    inline float3x3 float3x3::inverted() const noexcept
    {
        const float3 r0 = row0_.xyz();
        const float3 r1 = row1_.xyz();
        const float3 r2 = row2_.xyz();

        const float a = r0.x, b = r0.y, c = r0.z;
        const float d = r1.x, e = r1.y, f = r1.z;
        const float g = r2.x, h = r2.y, i = r2.z;

        const float det = a * (e * i - f * h) - b * (d * i - f * g) + c * (d * h - e * g);

        if (std::abs(det) < Constants::Constants<float>::Epsilon) {
            return identity();
        }

        const float inv_det = 1.0f / det;

        const float a11 = (e * i - f * h) * inv_det;
        const float a12 = (c * h - b * i) * inv_det;
        const float a13 = (b * f - c * e) * inv_det;
        const float a21 = (f * g - d * i) * inv_det;
        const float a22 = (a * i - c * g) * inv_det;
        const float a23 = (c * d - a * f) * inv_det;
        const float a31 = (d * h - e * g) * inv_det;
        const float a32 = (b * g - a * h) * inv_det;
        const float a33 = (a * e - b * d) * inv_det;

        return float3x3(
            float3(a11, a12, a13),
            float3(a21, a22, a23),
            float3(a31, a32, a33)
        );
    }

    inline float3x3 float3x3::normal_matrix(const float3x3& model) noexcept
    {
        float3x3 inv = model.inverted();
        float3x3 result = inv.transposed();

        float3 col0 = result.col0().normalize();
        float3 col1 = result.col1().normalize();
        float3 col2 = result.col2().normalize();

        return float3x3(col0, col1, col2);
    }

    inline float float3x3::trace() const noexcept
    {
        return row0_.x + row1_.y + row2_.z;
    }

    inline float3 float3x3::diagonal() const noexcept
    {
        return float3(row0_.x, row1_.y, row2_.z);
    }

    inline float float3x3::frobenius_norm() const noexcept
    {
        return std::sqrt(row0_.length_sq() + row1_.length_sq() + row2_.length_sq());
    }

    inline float3x3 float3x3::symmetric_part() const noexcept
    {
        float3x3 trans = transposed();
        return (*this + trans) * 0.5f;
    }

    inline float3x3 float3x3::skew_symmetric_part() const noexcept
    {
        float3x3 trans = transposed();
        return (*this - trans) * 0.5f;
    }

    inline float3 float3x3::transform_vector(const float3& vec) const noexcept
    {
        float x = row0_.x * vec.x + row0_.y * vec.y + row0_.z * vec.z;
        float y = row1_.x * vec.x + row1_.y * vec.y + row1_.z * vec.z;
        float z = row2_.x * vec.x + row2_.y * vec.y + row2_.z * vec.z;

        return float3(x, y, z);
    }

    inline float3 float3x3::transform_point(const float3& point) const noexcept
    {
        return transform_vector(point);
    }

    inline float3 float3x3::transform_normal(const float3& normal) const noexcept
    {
        if (is_orthonormal(1e-4f)) {
            return float3(
                normal.x * row0_.x + normal.y * row0_.y + normal.z * row0_.z,
                normal.x * row1_.x + normal.y * row1_.y + normal.z * row1_.z,
                normal.x * row2_.x + normal.y * row2_.y + normal.z * row2_.z
            );
        }

        float3x3 inv = inverted();
        return float3(
            normal.x * inv.row0_.x + normal.y * inv.row0_.y + normal.z * inv.row0_.z,
            normal.x * inv.row1_.x + normal.y * inv.row1_.y + normal.z * inv.row1_.z,
            normal.x * inv.row2_.x + normal.y * inv.row2_.y + normal.z * inv.row2_.z
        );
    }

    inline float3 float3x3::extract_scale() const noexcept
    {
        return float3(col0().length(), col1().length(), col2().length());
    }

    inline float3x3 float3x3::extract_rotation() const noexcept
    {
        if (is_orthonormal(1e-4f)) {
            return *this;
        }

        float3 c0 = col0().normalize();
        float3 c1 = col1();

        c1 = (c1 - c0 * dot(c1, c0)).normalize();
        float3 c2 = cross(c0, c1).normalize();

        return float3x3(c0, c1, c2);
    }

    inline bool float3x3::is_identity(float epsilon) const noexcept
    {
        return row0_.approximately(float4(1.0f, 0.0f, 0.0f, 0.0f), epsilon) &&
            row1_.approximately(float4(0.0f, 1.0f, 0.0f, 0.0f), epsilon) &&
            row2_.approximately(float4(0.0f, 0.0f, 1.0f, 0.0f), epsilon);
    }

    inline bool float3x3::is_orthogonal(float epsilon) const noexcept
    {
        return MathFunctions::approximately(dot(col0(), col1()), 0.0f, epsilon) &&
            MathFunctions::approximately(dot(col0(), col2()), 0.0f, epsilon) &&
            MathFunctions::approximately(dot(col1(), col2()), 0.0f, epsilon);
    }

    inline bool float3x3::is_orthonormal(float epsilon) const noexcept
    {
        return is_orthogonal(epsilon) &&
            MathFunctions::approximately(col0().length_sq(), 1.0f, epsilon) &&
            MathFunctions::approximately(col1().length_sq(), 1.0f, epsilon) &&
            MathFunctions::approximately(col2().length_sq(), 1.0f, epsilon);
    }

    inline bool float3x3::approximately(const float3x3& other, float epsilon) const noexcept
    {
        return row0_.approximately(other.row0_, epsilon) &&
            row1_.approximately(other.row1_, epsilon) &&
            row2_.approximately(other.row2_, epsilon);
    }

    inline std::string float3x3::to_string() const
    {
        char buffer[256];
        std::snprintf(buffer, sizeof(buffer),
            "[%8.4f, %8.4f, %8.4f]\n"
            "[%8.4f, %8.4f, %8.4f]\n"
            "[%8.4f, %8.4f, %8.4f]",
            row0_.x, row0_.y, row0_.z,
            row1_.x, row1_.y, row1_.z,
            row2_.x, row2_.y, row2_.z);
        return std::string(buffer);
    }

    inline void float3x3::to_row_major(float* data) const noexcept
    {
        data[0] = row0_.x; data[1] = row0_.y; data[2] = row0_.z;
        data[3] = row1_.x; data[4] = row1_.y; data[5] = row1_.z;
        data[6] = row2_.x; data[7] = row2_.y; data[8] = row2_.z;
    }

    inline void float3x3::to_column_major(float* data) const noexcept
    {
        data[0] = row0_.x; data[1] = row1_.x; data[2] = row2_.x;
        data[3] = row0_.y; data[4] = row1_.y; data[5] = row2_.y;
        data[6] = row0_.z; data[7] = row1_.z; data[8] = row2_.z;
    }

    inline bool float3x3::isValid() const noexcept
    {
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (!std::isfinite((*this)(i, j)))
                    return false;
        return true;
    }

    inline bool float3x3::operator==(const float3x3& rhs) const noexcept
    {
        return approximately(rhs);
    }

    inline bool float3x3::operator!=(const float3x3& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    inline float3x3 operator*(const float3x3& lhs, const float3x3& rhs) noexcept
    {
        float3x3 result;

        for (int i = 0; i < 3; ++i) {
            float3 row = lhs[i];
            float3 res_row;
            res_row.x = dot(row, rhs.col0());
            res_row.y = dot(row, rhs.col1());
            res_row.z = dot(row, rhs.col2());

            switch (i) {
            case 0: result.set_row0(res_row); break;
            case 1: result.set_row1(res_row); break;
            case 2: result.set_row2(res_row); break;
            }
        }

        return result;
    }

    inline const float3x3 float3x3_Identity = float3x3::identity();
    inline const float3x3 float3x3_Zero = float3x3::zero();
}
#endif
