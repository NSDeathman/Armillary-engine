#ifndef MATH_FLOAT4X4_INL
#define MATH_FLOAT4X4_INL

#include "math_float4x4.h"
#include "math_float3x3.h"
#include "math_quaternion.h"

namespace Math
{
    inline float4x4 operator+(const float4x4& lhs, const float4x4& rhs) noexcept { return float4x4(lhs) += rhs; }
    inline float4x4 operator-(const float4x4& lhs, const float4x4& rhs) noexcept { return float4x4(lhs) -= rhs; }
    inline float4x4 operator*(const float4x4& mat, float scalar) noexcept { return float4x4(mat) *= scalar; }
    inline float4x4 operator*(float scalar, const float4x4& mat) noexcept { return float4x4(mat) *= scalar; }
    inline float4x4 operator/(const float4x4& mat, float scalar) noexcept { return float4x4(mat) /= scalar; }
    inline float4 operator*(const float4& vec, const float4x4& mat) noexcept { return mat.transform_vector(vec); }
    inline float3 operator*(const float3& point, const float4x4& mat) noexcept { return mat.transform_point(point); }

    inline float4x4 transpose(const float4x4& mat) noexcept { return mat.transposed(); }
    inline float4x4 inverse(const float4x4& mat) noexcept { return mat.inverted(); }
    inline float determinant(const float4x4& mat) noexcept { return mat.determinant(); }
    inline float4 mul(const float4& vec, const float4x4& mat) noexcept { return vec * mat; }
    inline float3 mul(const float3& point, const float4x4& mat) noexcept { return point * mat; }
    inline float4x4 mul(const float4x4& lhs, const float4x4& rhs) noexcept { return lhs * rhs; }

    inline float4x4::float4x4() noexcept
        : row0_(1, 0, 0, 0), row1_(0, 1, 0, 0), row2_(0, 0, 1, 0), row3_(0, 0, 0, 1) {}

    inline float4x4::float4x4(const float4& r0, const float4& r1, const float4& r2, const float4& r3) noexcept
        : row0_(r0), row1_(r1), row2_(r2), row3_(r3) {}

    inline float4x4::float4x4(float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33) noexcept
        : row0_(m00, m01, m02, m03)
        , row1_(m10, m11, m12, m13)
        , row2_(m20, m21, m22, m23)
        , row3_(m30, m31, m32, m33) {}

    inline float4x4::float4x4(const float* data) noexcept
        : row0_(data[0], data[1], data[2], data[3])
        , row1_(data[4], data[5], data[6], data[7])
        , row2_(data[8], data[9], data[10], data[11])
        , row3_(data[12], data[13], data[14], data[15]) {}

    inline float4x4::float4x4(float scalar) noexcept
        : row0_(scalar, 0, 0, 0), row1_(0, scalar, 0, 0), row2_(0, 0, scalar, 0), row3_(0, 0, 0, scalar) {}

    inline float4x4::float4x4(const float4& diagonal) noexcept
        : row0_(diagonal.x, 0, 0, 0), row1_(0, diagonal.y, 0, 0), row2_(0, 0, diagonal.z, 0), row3_(0, 0, 0, diagonal.w) {}

    inline float4x4::float4x4(const float3x3& m) noexcept {
        row0_ = float4(m.row0(), 0.0f);
        row1_ = float4(m.row1(), 0.0f);
        row2_ = float4(m.row2(), 0.0f);
        row3_ = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline float4x4::float4x4(const quaternion& q) noexcept {
        float xx = q.x * q.x;
        float yy = q.y * q.y;
        float zz = q.z * q.z;
        float xy = q.x * q.y;
        float xz = q.x * q.z;
        float yz = q.y * q.z;
        float wx = q.w * q.x;
        float wy = q.w * q.y;
        float wz = q.w * q.z;

        // LH Conversion matching quaternion::to_matrix3x3 and float4x4::rotation_y
        // Upper triangle W subtracted, Lower triangle W added
        row0_ = float4(1.0f - 2.0f * (yy + zz), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f);
        row1_ = float4(2.0f * (xy - wz), 1.0f - 2.0f * (xx + zz), 2.0f * (yz + wx), 0.0f);
        row2_ = float4(2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xx + yy), 0.0f);
        row3_ = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline float4x4 float4x4::identity() noexcept { return float4x4(); }
    inline float4x4 float4x4::zero() noexcept { return float4x4(0.0f); }

    inline float4x4 float4x4::translation(float x, float y, float z) noexcept {
        return float4x4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, x, y, z, 1);
    }

    inline float4x4 float4x4::translation(const float3& p) noexcept {
        return translation(p.x, p.y, p.z);
    }

    inline float4x4 float4x4::scaling(float x, float y, float z) noexcept {
        return float4x4(x, 0, 0, 0, 0, y, 0, 0, 0, 0, z, 0, 0, 0, 0, 1);
    }

    inline float4x4 float4x4::scaling(const float3& s) noexcept {
        return scaling(s.x, s.y, s.z);
    }

    inline float4x4 float4x4::scaling(float s) noexcept {
        return scaling(s, s, s);
    }

    inline float4x4 float4x4::rotation_x(float angle) noexcept {
        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);
        return float4x4(1, 0, 0, 0, 0, c, -s, 0, 0, s, c, 0, 0, 0, 0, 1);
    }

    inline float4x4 float4x4::rotation_y(float angle) noexcept {
        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);
        // LH Rotation Y: [c 0 -s]
        return float4x4(
            c, 0, -s, 0,
            0, 1, 0, 0,
            s, 0, c, 0,
            0, 0, 0, 1
        );
    }

    inline float4x4 float4x4::rotation_z(float angle) noexcept {
        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);
        return float4x4(c, -s, 0, 0, s, c, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
    }

    inline float4x4 float4x4::rotation_axis(const float3& axis, float angle) noexcept {
        if (axis.approximately_zero(Constants::Constants<float>::Epsilon)) {
            return identity();
        }

        float s, c;
        MathFunctions::sin_cos(angle, &s, &c);
        float t = 1.0f - c;
        float3 n = axis.normalize();
        float x = n.x, y = n.y, z = n.z;

        return float4x4(
            t * x * x + c, t * x * y - z * s, t * x * z + y * s, 0,
            t * x * y + z * s, t * y * y + c, t * y * z - x * s, 0,
            t * x * z - y * s, t * y * z + x * s, t * z * z + c, 0,
            0, 0, 0, 1
        );
    }

    inline float4x4 float4x4::rotation_euler(const float3& a) noexcept {
        return rotation_z(a.z) * rotation_y(a.y) * rotation_x(a.x);
    }

    inline float4x4 float4x4::TRS(const float3& t, const quaternion& r, const float3& s) noexcept {
        return scaling(s) * float4x4(r) * translation(t);
    }

    inline float4x4 float4x4::perspective_lh_zo(float fov, float ar, float zn, float zf) noexcept {
        float h = 1.0f / std::tan(fov * 0.5f);
        float w = h / ar;
        float r = zf / (zf - zn);
        return float4x4(w, 0, 0, 0, 0, h, 0, 0, 0, 0, r, 1, 0, 0, -r * zn, 0);
    }

    inline float4x4 float4x4::perspective_rh_zo(float fov, float ar, float zn, float zf) noexcept {
        float tanHalfFov = std::tan(fov * 0.5f);
        float h = 1.0f / tanHalfFov;
        float w = h / ar;
        float range_inv = 1.0f / (zn - zf);

        return float4x4(
            w, 0, 0, 0,
            0, h, 0, 0,
            0, 0, zf * range_inv, -1,
            0, 0, -zn * zf * range_inv, 0
        );
    }


    inline float4x4 float4x4::perspective_lh_no(float fov, float ar, float zn, float zf) noexcept {
        float h = 1.0f / std::tan(fov * 0.5f);
        float w = h / ar;
        return float4x4(w, 0, 0, 0, 0, h, 0, 0, 0, 0, (zf + zn) / (zf - zn), 1, 0, 0, -2 * zn * zf / (zf - zn), 0);
    }

    inline float4x4 float4x4::perspective_rh_no(float fov, float ar, float zn, float zf) noexcept {
        float h = 1.0f / std::tan(fov * 0.5f);
        float w = h / ar;
        return float4x4(w, 0, 0, 0, 0, h, 0, 0, 0, 0, -(zf + zn) / (zf - zn), -1, 0, 0, -2 * zn * zf / (zf - zn), 0);
    }

    inline float4x4 float4x4::perspective(float fov, float ar, float zn, float zf) noexcept {
        return perspective_lh_zo(fov, ar, zn, zf);
    }

    inline float4x4 float4x4::orthographic_lh_zo(float width, float height, float zNear, float zFar) noexcept {
        float fRange = 1.0f / (zFar - zNear);
        return float4x4(
            2.0f / width, 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / height, 0.0f, 0.0f,
            0.0f, 0.0f, fRange, 0.0f,
            0.0f, 0.0f, -zNear * fRange, 1.0f
        );
    }

    inline float4x4 float4x4::orthographic_off_center_lh_zo(float left, float right, float bottom, float top, float zNear, float zFar) noexcept {
        float fRange = 1.0f / (zFar - zNear);
        return float4x4(
            2.0f / (right - left), 0.0f, 0.0f, 0.0f,
            0.0f, 2.0f / (top - bottom), 0.0f, 0.0f,
            0.0f, 0.0f, fRange, 0.0f,
            -(left + right) / (right - left), -(top + bottom) / (top - bottom), -zNear * fRange, 1.0f
        );
    }

    inline float4x4 float4x4::orthographic(float w, float h, float zn, float zf) noexcept {
        return orthographic_lh_zo(w, h, zn, zf);
    }

    inline float4x4 float4x4::look_at_lh(const float3& eye, const float3& target, const float3& up) noexcept {
        float3 z = (target - eye).normalize();
        float3 x = up.cross(z).normalize();
        float3 y = z.cross(x);
        return float4x4(x.x, x.y, x.z, 0, y.x, y.y, y.z, 0, z.x, z.y, z.z, 0, -x.dot(eye), -y.dot(eye), -z.dot(eye), 1);
    }

    inline float4x4 float4x4::look_at_rh(const float3& eye, const float3& target, const float3& up) noexcept {
        float3 z = (eye - target).normalize();
        float3 x = up.cross(z).normalize();
        float3 y = z.cross(x);
        return float4x4(x.x, x.y, x.z, 0, y.x, y.y, y.z, 0, z.x, z.y, z.z, 0, -x.dot(eye), -y.dot(eye), -z.dot(eye), 1);
    }

    inline float4x4 float4x4::look_at(const float3& eye, const float3& target, const float3& up) noexcept {
        return look_at_lh(eye, target, up);
    }

    inline float4& float4x4::operator[](int rowIndex) noexcept {
        return (&row0_)[rowIndex];
    }

    inline const float4& float4x4::operator[](int rowIndex) const noexcept {
        return (&row0_)[rowIndex];
    }

    inline float& float4x4::operator()(int r, int c) noexcept {
        return (&row0_)[r][c];
    }

    inline const float& float4x4::operator()(int r, int c) const noexcept {
        return (&row0_)[r][c];
    }

    inline float4 float4x4::row0() const noexcept { return row0_; }
    inline float4 float4x4::row1() const noexcept { return row1_; }
    inline float4 float4x4::row2() const noexcept { return row2_; }
    inline float4 float4x4::row3() const noexcept { return row3_; }

    inline void float4x4::set_row0(const float4& r) noexcept { row0_ = r; }
    inline void float4x4::set_row1(const float4& r) noexcept { row1_ = r; }
    inline void float4x4::set_row2(const float4& r) noexcept { row2_ = r; }
    inline void float4x4::set_row3(const float4& r) noexcept { row3_ = r; }

    inline float4 float4x4::col0() const noexcept {
        return float4(row0_.x, row1_.x, row2_.x, row3_.x);
    }

    inline float4 float4x4::col1() const noexcept {
        return float4(row0_.y, row1_.y, row2_.y, row3_.y);
    }

    inline float4 float4x4::col2() const noexcept {
        return float4(row0_.z, row1_.z, row2_.z, row3_.z);
    }

    inline float4 float4x4::col3() const noexcept {
        return float4(row0_.w, row1_.w, row2_.w, row3_.w);
    }

    inline float4x4& float4x4::operator+=(const float4x4& rhs) noexcept {
        row0_ += rhs.row0_;
        row1_ += rhs.row1_;
        row2_ += rhs.row2_;
        row3_ += rhs.row3_;
        return *this;
    }

    inline float4x4& float4x4::operator-=(const float4x4& rhs) noexcept {
        row0_ -= rhs.row0_;
        row1_ -= rhs.row1_;
        row2_ -= rhs.row2_;
        row3_ -= rhs.row3_;
        return *this;
    }

    inline float4x4& float4x4::operator*=(float s) noexcept {
        row0_ *= s;
        row1_ *= s;
        row2_ *= s;
        row3_ *= s;
        return *this;
    }

    inline float4x4& float4x4::operator/=(float s) noexcept {
        float is = 1.0f / s;
        row0_ *= is;
        row1_ *= is;
        row2_ *= is;
        row3_ *= is;
        return *this;
    }

    inline float4x4& float4x4::operator*=(const float4x4& rhs) noexcept {
        *this = *this * rhs;
        return *this;
    }

    inline float4x4 float4x4::operator+() const noexcept { return *this; }

    inline float4x4 float4x4::operator-() const noexcept {
        return float4x4(-row0_, -row1_, -row2_, -row3_);
    }

    inline float4x4 float4x4::transposed() const noexcept {
        __m128 t0 = _mm_shuffle_ps(row0_.get_simd(), row1_.get_simd(), 0x44);
        __m128 t2 = _mm_shuffle_ps(row0_.get_simd(), row1_.get_simd(), 0xEE);
        __m128 t1 = _mm_shuffle_ps(row2_.get_simd(), row3_.get_simd(), 0x44);
        __m128 t3 = _mm_shuffle_ps(row2_.get_simd(), row3_.get_simd(), 0xEE);

        __m128 row0 = _mm_shuffle_ps(t0, t1, 0x88);
        __m128 row1 = _mm_shuffle_ps(t0, t1, 0xDD);
        __m128 row2 = _mm_shuffle_ps(t2, t3, 0x88);
        __m128 row3 = _mm_shuffle_ps(t2, t3, 0xDD);

        return float4x4(float4(row0), float4(row1), float4(row2), float4(row3));
    }

    inline float float4x4::determinant() const noexcept {
        float a = row0_.x, b = row0_.y, c = row0_.z, d = row0_.w;
        float e = row1_.x, f = row1_.y, g = row1_.z, h = row1_.w;
        float i = row2_.x, j = row2_.y, k = row2_.z, l = row2_.w;
        float m = row3_.x, n = row3_.y, o = row3_.z, p = row3_.w;

        float kplo = k * p - l * o;
        float jpln = j * p - l * n;
        float jokn = j * o - k * n;
        float iplm = i * p - l * m;
        float iokm = i * o - k * m;
        float in_jm = i * n - j * m;

        return a * (f * kplo - g * jpln + h * jokn) -
            b * (e * kplo - g * iplm + h * iokm) +
            c * (e * jpln - f * iplm + h * in_jm) -
            d * (e * jokn - f * iokm + g * in_jm);
    }

    inline float4x4 float4x4::inverted_affine() const noexcept {
        const float3 r0 = row0_.xyz();
        const float3 r1 = row1_.xyz();
        const float3 r2 = row2_.xyz();
        const float3 t = get_translation();

        const float det = r0.dot(r1.cross(r2));

        if (std::abs(det) < Constants::Constants<float>::Epsilon) {
            return identity();
        }

        const float inv_det = 1.0f / det;

        const float3 c0 = r1.cross(r2) * inv_det;
        const float3 c1 = r2.cross(r0) * inv_det;
        const float3 c2 = r0.cross(r1) * inv_det;

        const float3 inv_t = float3(-c0.dot(t), -c1.dot(t), -c2.dot(t));

        return float4x4(
            c0.x, c1.x, c2.x, 0.0f,
            c0.y, c1.y, c2.y, 0.0f,
            c0.z, c1.z, c2.z, 0.0f,
            inv_t.x, inv_t.y, inv_t.z, 1.0f
        );
    }

    inline float4x4 float4x4::inverted() const noexcept
    {
        if (is_affine()) {
            return inverted_affine();
        }

        float det = determinant();
        if (std::abs(det) < Constants::Constants<float>::Epsilon) {
            return identity();
        }

        return adjugate() / det;
    }

    inline float4x4 float4x4::adjugate() const noexcept {
        float a = row0_.x, b = row0_.y, c = row0_.z, d = row0_.w;
        float e = row1_.x, f = row1_.y, g = row1_.z, h = row1_.w;
        float i = row2_.x, j = row2_.y, k = row2_.z, l = row2_.w;
        float m = row3_.x, n = row3_.y, o = row3_.z, p = row3_.w;

        float kplo = k * p - l * o;
        float jpln = j * p - l * n;
        float jokn = j * o - k * n;
        float iplm = i * p - l * m;
        float iokm = i * o - k * m;
        float in_jm = i * n - j * m;
        float gpho = g * p - h * o;
        float fphn = f * p - h * n;
        float fogn = f * o - g * n;
        float ep_hm = e * p - h * m;
        float eogm = e * o - g * m;
        float en_fm = e * n - f * m;
        float gl_hk = g * l - h * k;
        float fl_hj = f * l - h * j;
        float fk_gj = f * k - g * j;
        float el_hi = e * l - h * i;
        float ek_gi = e * k - g * i;
        float ej_fi = e * j - f * i;

        return float4x4(
            f * kplo - g * jpln + h * jokn,
            -b * kplo + c * jpln - d * jokn,
            b * gpho - c * fphn + d * fogn,
            -b * gl_hk + c * fl_hj - d * fk_gj,
            -e * kplo + g * iplm - h * iokm,
            a * kplo - c * iplm + d * iokm,
            -a * gpho + c * ep_hm - d * eogm,
            a * gl_hk - c * el_hi + d * ek_gi,
            e * jpln - f * iplm + h * in_jm,
            -a * jpln + b * iplm - d * in_jm,
            a * fphn - b * ep_hm + d * en_fm,
            -a * fl_hj + b * el_hi - d * ej_fi,
            -e * jokn + f * iokm - g * in_jm,
            a * jokn - b * iokm + c * in_jm,
            -a * fogn + b * eogm - c * en_fm,
            a * fk_gj - b * ek_gi + c * ej_fi
        );
    }

    inline float3x3 float4x4::normal_matrix() const noexcept {
        float3x3 upper(
            float3(row0_.x, row0_.y, row0_.z),
            float3(row1_.x, row1_.y, row1_.z),
            float3(row2_.x, row2_.y, row2_.z)
        );

        float3x3 result = upper.inverted().transposed();

        float3 col0 = result.col0().normalize();
        float3 col1 = result.col1().normalize();
        float3 col2 = result.col2().normalize();

        return float3x3(col0, col1, col2);
    }

    inline float float4x4::trace() const noexcept {
        return row0_.x + row1_.y + row2_.z + row3_.w;
    }

    inline float4 float4x4::diagonal() const noexcept {
        return float4(row0_.x, row1_.y, row2_.z, row3_.w);
    }

    inline float float4x4::frobenius_norm() const noexcept {
        return std::sqrt(row0_.length_sq() + row1_.length_sq() + row2_.length_sq() + row3_.length_sq());
    }

    inline float4 float4x4::transform_vector(const float4& v) const noexcept {
        __m128 result = _mm_mul_ps(_mm_set1_ps(v.x), row0_.get_simd());
        result = _mm_add_ps(result, _mm_mul_ps(_mm_set1_ps(v.y), row1_.get_simd()));
        result = _mm_add_ps(result, _mm_mul_ps(_mm_set1_ps(v.z), row2_.get_simd()));
        result = _mm_add_ps(result, _mm_mul_ps(_mm_set1_ps(v.w), row3_.get_simd()));
        return float4(result);
    }

    inline float3 float4x4::transform_point(const float3& p) const noexcept {
        float4 r = transform_vector(float4(p, 1.0f));
        return float3(r.x / r.w, r.y / r.w, r.z / r.w);
    }

    inline float3 float4x4::transform_vector(const float3& v) const noexcept {
        float4 r = transform_vector(float4(v, 0.0f));
        return float3(r.x, r.y, r.z);
    }

    inline float3 float4x4::transform_direction(const float3& d) const noexcept {
        return transform_vector(d).normalize();
    }

    inline float3 float4x4::get_translation() const noexcept {
        return float3(row3_.x, row3_.y, row3_.z);
    }

    inline float3 float4x4::get_scale() const noexcept {
        return float3(
            float3(row0_.x, row0_.y, row0_.z).length(),
            float3(row1_.x, row1_.y, row1_.z).length(),
            float3(row2_.x, row2_.y, row2_.z).length()
        );
    }

    inline quaternion float4x4::get_rotation() const noexcept {
        float3 scale = get_scale();

        const float epsilon = Constants::Constants<float>::Epsilon;

        float inv_scale_x = (std::abs(scale.x) > epsilon) ? (1.0f / scale.x) : 0.0f;
        float inv_scale_y = (std::abs(scale.y) > epsilon) ? (1.0f / scale.y) : 0.0f;
        float inv_scale_z = (std::abs(scale.z) > epsilon) ? (1.0f / scale.z) : 0.0f;

        // Correctly apply inverse scale to each ROW (which represents a basis vector in Row-Major)
        // Previous error was applying components of scale to components of rows
        float3 r0 = float3(row0_.x * inv_scale_x, row0_.y * inv_scale_x, row0_.z * inv_scale_x);
        float3 r1 = float3(row1_.x * inv_scale_y, row1_.y * inv_scale_y, row1_.z * inv_scale_y);
        float3 r2 = float3(row2_.x * inv_scale_z, row2_.y * inv_scale_z, row2_.z * inv_scale_z);

        float3x3 rot_matrix(r0, r1, r2);

        rot_matrix = rot_matrix.extract_rotation();

        // Transpose because quaternion::from_matrix expects column vectors
        return quaternion::from_matrix(rot_matrix.transposed());
    }

    inline void float4x4::set_translation(const float3& t) noexcept {
        row3_.x = t.x;
        row3_.y = t.y;
        row3_.z = t.z;
    }

    inline void float4x4::set_scale(const float3& scale) noexcept {
        float3 x_axis = row0_.xyz().normalize();
        float3 y_axis = row1_.xyz().normalize();
        float3 z_axis = row2_.xyz().normalize();

        x_axis *= scale.x;
        y_axis *= scale.y;
        z_axis *= scale.z;

        row0_ = float4(x_axis, row0_.w);
        row1_ = float4(y_axis, row1_.w);
        row2_ = float4(z_axis, row2_.w);
    }

    inline bool float4x4::is_identity(float epsilon) const noexcept {
        return row0_.approximately(float4(1, 0, 0, 0), epsilon) &&
            row1_.approximately(float4(0, 1, 0, 0), epsilon) &&
            row2_.approximately(float4(0, 0, 1, 0), epsilon) &&
            row3_.approximately(float4(0, 0, 0, 1), epsilon);
    }

    inline bool float4x4::is_affine(float eps) const noexcept {
        return std::abs(row0_.w) < eps &&
            std::abs(row1_.w) < eps &&
            std::abs(row2_.w) < eps &&
            std::abs(row3_.w - 1.0f) < eps;
    }

    inline bool float4x4::is_orthogonal(float epsilon) const noexcept {
        if (!is_affine(epsilon)) return false;

        float3 r0 = row0_.xyz();
        float3 r1 = row1_.xyz();
        float3 r2 = row2_.xyz();

        float dot01 = std::abs(r0.dot(r1));
        float dot02 = std::abs(r0.dot(r2));
        float dot12 = std::abs(r1.dot(r2));

        if (dot01 > epsilon || dot02 > epsilon || dot12 > epsilon) {
            return false;
        }

        float len0 = r0.length_sq();
        float len1 = r1.length_sq();
        float len2 = r2.length_sq();

        return MathFunctions::approximately(len0, 1.0f, epsilon) &&
            MathFunctions::approximately(len1, 1.0f, epsilon) &&
            MathFunctions::approximately(len2, 1.0f, epsilon);
    }

    inline bool float4x4::approximately(const float4x4& o, float e) const noexcept {
        return row0_.approximately(o.row0_, e) &&
            row1_.approximately(o.row1_, e) &&
            row2_.approximately(o.row2_, e) &&
            row3_.approximately(o.row3_, e);
    }

    inline bool float4x4::approximately_zero(float e) const noexcept {
        return approximately(zero(), e);
    }

    inline std::string float4x4::to_string() const {
        char buffer[512];
        std::snprintf(buffer, sizeof(buffer),
            "[%8.4f, %8.4f, %8.4f, %8.4f]\n"
            "[%8.4f, %8.4f, %8.4f, %8.4f]\n"
            "[%8.4f, %8.4f, %8.4f, %8.4f]\n"
            "[%8.4f, %8.4f, %8.4f, %8.4f]",
            row0_.x, row0_.y, row0_.z, row0_.w,
            row1_.x, row1_.y, row1_.z, row1_.w,
            row2_.x, row2_.y, row2_.z, row2_.w,
            row3_.x, row3_.y, row3_.z, row3_.w);
        return std::string(buffer);
    }

    inline void float4x4::to_column_major(float* data) const noexcept {
        data[0] = row0_.x; data[1] = row1_.x; data[2] = row2_.x; data[3] = row3_.x;
        data[4] = row0_.y; data[5] = row1_.y; data[6] = row2_.y; data[7] = row3_.y;
        data[8] = row0_.z; data[9] = row1_.z; data[10] = row2_.z; data[11] = row3_.z;
        data[12] = row0_.w; data[13] = row1_.w; data[14] = row2_.w; data[15] = row3_.w;
    }

    inline void float4x4::to_row_major(float* data) const noexcept {
        data[0] = row0_.x; data[1] = row0_.y; data[2] = row0_.z; data[3] = row0_.w;
        data[4] = row1_.x; data[5] = row1_.y; data[6] = row1_.z; data[7] = row1_.w;
        data[8] = row2_.x; data[9] = row2_.y; data[10] = row2_.z; data[11] = row2_.w;
        data[12] = row3_.x; data[13] = row3_.y; data[14] = row3_.z; data[15] = row3_.w;
    }

    inline bool float4x4::operator==(const float4x4& rhs) const noexcept {
        return approximately(rhs);
    }

    inline bool float4x4::operator!=(const float4x4& rhs) const noexcept {
        return !(*this == rhs);
    }

    inline float4x4 operator*(const float4x4& lhs, const float4x4& rhs) noexcept {
        float4x4 res;

        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                float sum = 0.0f;
                for (int k = 0; k < 4; ++k) {
                    sum += lhs(i, k) * rhs(k, j);
                }
                res(i, j) = sum;
            }
        }

        return res;
    }

    inline const float4x4 float4x4_Identity = float4x4::identity();
    inline const float4x4 float4x4_Zero = float4x4::zero();
}
#endif
