// Description: 3-dimensional vector inline implementations (Packed 12-byte version)
// Author: NSDeathman, DeepSeek
#pragma once

namespace Math
{
    // ============================================================================
    // Helper Methods for Packed SSE
    // ============================================================================

    // Loads 3 floats into XMM register [x, y, z, 0]
    inline __m128 float3::get_simd() const noexcept {
        return _mm_set_ps(0.0f, z, y, x);
    }

    // Stores XMM register back to 3 floats (careful not to overwrite 4th float in memory)
    inline void float3::set_simd(__m128 new_simd) noexcept {
        // Since we are unaligned and 12 bytes, we can't do a single store.
        // We use an intermediate buffer or extract manually.
        // Alignas is key for _mm_store_ps correctness on stack.
        alignas(16) float temp[4];
        _mm_store_ps(temp, new_simd);
        x = temp[0];
        y = temp[1];
        z = temp[2];
    }

    // ============================================================================
    // Constructors Implementation
    // ============================================================================

    inline float3::float3() noexcept : x(0.0f), y(0.0f), z(0.0f) {}

    inline float3::float3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

    inline float3::float3(float scalar) noexcept : x(scalar), y(scalar), z(scalar) {}

    inline float3::float3(const float2& vec, float z) noexcept : x(vec.x), y(vec.y), z(z) {}

    inline float3::float3(const float* data) noexcept : x(data[0]), y(data[1]), z(data[2]) {}

    inline float3::float3(__m128 simd_val) noexcept {
        set_simd(simd_val);
    }

#if defined(MATH_SUPPORT_D3DX)
    inline float3::float3(const D3DXVECTOR3& vec) noexcept : x(vec.x), y(vec.y), z(vec.z) {}

    inline float3::float3(const D3DXVECTOR4& vec) noexcept : x(vec.x), y(vec.y), z(vec.z) {}

    inline float3::float3(const D3DXVECTOR2& vec, float z) noexcept : x(vec.x), y(vec.y), z(z) {}

    inline float3::float3(D3DCOLOR color) noexcept {
        x = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
        y = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
        z = static_cast<float>(color & 0xFF) / 255.0f;
    }
#endif

    // ============================================================================
    // Assignment Operators Implementation
    // ============================================================================

    inline float3& float3::operator=(float scalar) noexcept {
        x = y = z = scalar;
        return *this;
    }

#if defined(MATH_SUPPORT_D3DX)
    inline float3& float3::operator=(const D3DXVECTOR3& vec) noexcept {
        x = vec.x; y = vec.y; z = vec.z;
        return *this;
    }

    inline float3& float3::operator=(D3DCOLOR color) noexcept {
        x = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
        y = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
        z = static_cast<float>(color & 0xFF) / 255.0f;
        return *this;
    }
#endif

    // ============================================================================
    // Compound Assignment Operators Implementation
    // ============================================================================

    inline float3& float3::operator+=(const float3& rhs) noexcept {
        set_simd(_mm_add_ps(get_simd(), rhs.get_simd()));
        return *this;
    }

    inline float3& float3::operator-=(const float3& rhs) noexcept {
        set_simd(_mm_sub_ps(get_simd(), rhs.get_simd()));
        return *this;
    }

    inline float3& float3::operator*=(const float3& rhs) noexcept {
        set_simd(_mm_mul_ps(get_simd(), rhs.get_simd()));
        return *this;
    }

    inline float3& float3::operator/=(const float3& rhs) noexcept {
        set_simd(_mm_div_ps(get_simd(), rhs.get_simd()));
        return *this;
    }

    inline float3& float3::operator*=(float scalar) noexcept {
        set_simd(_mm_mul_ps(get_simd(), _mm_set1_ps(scalar)));
        return *this;
    }

    inline float3& float3::operator/=(float scalar) noexcept {
        set_simd(_mm_div_ps(get_simd(), _mm_set1_ps(scalar)));
        return *this;
    }

    // ============================================================================
    // Unary Operators Implementation
    // ============================================================================

    inline float3 float3::operator+() const noexcept {
        return *this;
    }

    inline float3 float3::operator-() const noexcept {
        return float3(_mm_sub_ps(_mm_setzero_ps(), get_simd()));
    }

    // ============================================================================
    // Access Operators Implementation
    // ============================================================================

    inline float& float3::operator[](int index) noexcept {
        return (&x)[index];
    }

    inline const float& float3::operator[](int index) const noexcept {
        return (&x)[index];
    }

    // ============================================================================
    // Conversion Operators Implementation
    // ============================================================================

    inline float3::operator const float* () const noexcept { return &x; }

    inline float3::operator float* () noexcept { return &x; }

    inline float3::operator __m128() const noexcept { return get_simd(); }

#if defined(MATH_SUPPORT_D3DX)
    inline float3::operator D3DXVECTOR3() const noexcept {
        return D3DXVECTOR3(x, y, z);
    }
#endif

    // ============================================================================
    // Static Constructors Implementation
    // ============================================================================

    inline float3 float3::zero() noexcept { return float3(0.0f, 0.0f, 0.0f); }
    inline float3 float3::one() noexcept { return float3(1.0f, 1.0f, 1.0f); }
    inline float3 float3::unit_x() noexcept { return float3(1.0f, 0.0f, 0.0f); }
    inline float3 float3::unit_y() noexcept { return float3(0.0f, 1.0f, 0.0f); }
    inline float3 float3::unit_z() noexcept { return float3(0.0f, 0.0f, 1.0f); }
    inline float3 float3::forward() noexcept { return float3(0.0f, 0.0f, 1.0f); }
    inline float3 float3::up() noexcept { return float3(0.0f, 1.0f, 0.0f); }
    inline float3 float3::right() noexcept { return float3(1.0f, 0.0f, 0.0f); }

    // ============================================================================
    // Mathematical Functions Implementation
    // ============================================================================

    inline float float3::length() const noexcept {
        __m128 v = get_simd();
        __m128 dp = _mm_dp_ps(v, v, 0x71); // Dot product of x,y,z; store in x
        // Если SSE4.1 не доступен (_mm_dp_ps), то используем старый метод:
        // __m128 sq = _mm_mul_ps(v, v);
        // и горизонтальное сложение.
        // Для совместимости используем базовый SSE:
        /*
        __m128 sq = _mm_mul_ps(v, v);
        __m128 shuf = _mm_shuffle_ps(sq, sq, _MM_SHUFFLE(0, 0, 2, 1));
        __m128 sum = _mm_add_ps(sq, shuf);
        shuf = _mm_shuffle_ps(sum, sum, _MM_SHUFFLE(0, 0, 0, 2));
        sum = _mm_add_ps(sum, shuf);
        return _mm_cvtss_f32(_mm_sqrt_ss(sum));
        */
        // Но проще и быстрее так, если мы уверены в SSE3+:
        return std::sqrt(x * x + y * y + z * z); // Компиляторы часто векторизуют это сами лучше, чем ручной код для 3-х компонент
    }

    inline float float3::length_sq() const noexcept {
        return x * x + y * y + z * z;
    }

    inline float3 float3::normalize() const noexcept {
        __m128 v = get_simd();
        __m128 dp = _mm_mul_ps(v, v);
        // Sum x^2 + y^2 + z^2
        __m128 t1 = _mm_shuffle_ps(dp, dp, _MM_SHUFFLE(0, 0, 2, 1));
        __m128 t2 = _mm_add_ps(dp, t1);
        __m128 t3 = _mm_shuffle_ps(t2, t2, _MM_SHUFFLE(0, 0, 0, 2));
        __m128 len_sq = _mm_add_ps(t2, t3); // Contains length^2 in all components

        __m128 len = _mm_sqrt_ps(len_sq);

        // Division (with epsilon check handled by logic or rsqrt)
        // Using standard div for precision
        __m128 normalized = _mm_div_ps(v, len);

        // Handle zero length case safely by checking mask
        // Or simpler C++ fallback:
        float l = length();
        if (l > Constants::Constants<float>::Epsilon) {
            return float3(x / l, y / l, z / l);
        }
        return float3::zero();
    }

    inline float float3::dot(const float3& other) const noexcept {
        return x * other.x + y * other.y + z * other.z;
    }

    inline float3 float3::cross(const float3& other) const noexcept {
        // SSE implementation of cross product
        __m128 a = get_simd();
        __m128 b = other.get_simd();

        // y, z, x
        __m128 a_yzx = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 0, 2, 1));
        __m128 b_yzx = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 0, 2, 1));

        // z, x, y
        __m128 a_zxy = _mm_shuffle_ps(a, a, _MM_SHUFFLE(3, 1, 0, 2));
        __m128 b_zxy = _mm_shuffle_ps(b, b, _MM_SHUFFLE(3, 1, 0, 2));

        return float3(_mm_sub_ps(
            _mm_mul_ps(a_yzx, b_zxy),
            _mm_mul_ps(a_zxy, b_yzx)
        ));
    }

    inline float float3::distance(const float3& other) const noexcept {
        return (*this - other).length();
    }

    inline float float3::distance_sq(const float3& other) const noexcept {
        return (*this - other).length_sq();
    }

    // ============================================================================
    // HLSL-like Functions Implementation
    // ============================================================================

    inline float3 float3::clamp(const float3& vec, const float3& min_val, const float3& max_val) noexcept {
        // SSE implementation: min(max(x, min_val), max_val)
        return float3(_mm_min_ps(_mm_max_ps(vec.get_simd(), min_val.get_simd()), max_val.get_simd()));
    }

    inline float3 float3::clamp(const float3& vec, float min_val, float max_val) noexcept {
        __m128 mn = _mm_set1_ps(min_val);
        __m128 mx = _mm_set1_ps(max_val);
        return float3(_mm_min_ps(_mm_max_ps(vec.get_simd(), mn), mx));
    }

    inline float3 float3::abs() const noexcept {
        static const __m128 sign_mask = _mm_set1_ps(-0.0f); // 0x80000000
        return float3(_mm_andnot_ps(sign_mask, get_simd()));
    }

    inline float3 float3::sign() const noexcept {
        // x > 0 ? 1 : (x < 0 ? -1 : 0)
        __m128 v = get_simd();
        __m128 zero = _mm_setzero_ps();
        __m128 gt = _mm_cmpgt_ps(v, zero);
        __m128 lt = _mm_cmplt_ps(v, zero);
        __m128 one = _mm_set1_ps(1.0f);
        __m128 none = _mm_set1_ps(-1.0f);
        return float3(_mm_or_ps(_mm_and_ps(gt, one), _mm_and_ps(lt, none)));
    }

    inline float3 float3::floor() const noexcept {
        return float3(std::floor(x), std::floor(y), std::floor(z));
    }

    inline float3 float3::ceil() const noexcept {
        return float3(std::ceil(x), std::ceil(y), std::ceil(z));
    }

    inline float3 float3::round() const noexcept {
        return float3(std::round(x), std::round(y), std::round(z));
    }

    inline float3 float3::frac() const noexcept {
        return float3(x - std::floor(x), y - std::floor(y), z - std::floor(z));
    }

    inline float3 float3::saturate() const noexcept {
        __m128 v = get_simd();
        __m128 zero = _mm_setzero_ps();
        __m128 one = _mm_set1_ps(1.0f);
        return float3(_mm_min_ps(_mm_max_ps(v, zero), one));
    }

    inline float3 float3::step(float edge) const noexcept {
        __m128 e = _mm_set1_ps(edge);
        __m128 v = get_simd();
        __m128 ge = _mm_cmpge_ps(v, e);
        return float3(_mm_and_ps(ge, _mm_set1_ps(1.0f)));
    }

    // ============================================================================
    // Geometric Operations Implementation
    // ============================================================================

    inline float3 float3::reflect(const float3& normal) const noexcept {
        float d = dot(normal);
        return *this - (normal * (2.0f * d));
    }

    inline float3 float3::refract(const float3& normal, float eta) const noexcept {
        float d = dot(normal);
        float k = 1.0f - eta * eta * (1.0f - d * d);
        if (k < 0.0f) return float3::zero();
        return (*this * eta) - (normal * (eta * d + std::sqrt(k)));
    }

    inline float3 float3::project(const float3& onto) const noexcept {
        float lSq = onto.length_sq();
        if (lSq < Constants::Constants<float>::Epsilon) return float3::zero();
        return onto * (dot(onto) / lSq);
    }

    inline float3 float3::reject(const float3& from) const noexcept {
        return *this - project(from);
    }

    // ============================================================================
    // Static Mathematical Functions Implementation
    // ============================================================================

    inline float float3::dot(const float3& a, const float3& b) noexcept {
        return a.dot(b);
    }

    inline float3 float3::cross(const float3& a, const float3& b) noexcept {
        return a.cross(b);
    }

    inline float3 float3::lerp(const float3& a, const float3& b, float t) noexcept {
        __m128 vt = _mm_set1_ps(t);
        __m128 va = a.get_simd();
        __m128 vb = b.get_simd();
        // a + t*(b-a) or a*(1-t) + b*t
        __m128 res = _mm_add_ps(
            _mm_mul_ps(va, _mm_sub_ps(_mm_set1_ps(1.0f), vt)),
            _mm_mul_ps(vb, vt)
        );
        return float3(res);
    }

    inline float3 float3::slerp(const float3& a, const float3& b, float t) noexcept {
        // ... (Same logic as before, just using new operators)
        if (t <= 0.0f) return a;
        if (t >= 1.0f) return b;

        float3 an = a.normalize();
        float3 bn = b.normalize();
        float d = dot(an, bn);
        d = MathFunctions::clamp(d, -1.0f, 1.0f);

        if (d > 0.9995f) return lerp(an, bn, t).normalize();

        if (d < -0.9995f) {
            float3 ortho;
            if (std::abs(an.x) > 0.1f) ortho = float3(-an.y, an.x, 0.0f).normalize();
            else ortho = float3(0.0f, -an.z, an.y).normalize();
            float angle = Constants::PI * t;
            return an * std::cos(angle) + ortho * std::sin(angle);
        }

        float theta = std::acos(d);
        float sinTheta = std::sin(theta);
        if (sinTheta < 1e-8f) return lerp(an, bn, t).normalize();

        float fa = std::sin((1.0f - t) * theta) / sinTheta;
        float fb = std::sin(t * theta) / sinTheta;
        return an * fa + bn * fb;
    }

    inline float3 float3::min(const float3& a, const float3& b) noexcept {
        return float3(_mm_min_ps(a.get_simd(), b.get_simd()));
    }

    inline float3 float3::max(const float3& a, const float3& b) noexcept {
        return float3(_mm_max_ps(a.get_simd(), b.get_simd()));
    }

    inline float3 float3::saturate(const float3& vec) noexcept {
        return vec.saturate();
    }

    inline float3 float3::reflect(const float3& incident, const float3& normal) noexcept {
        return incident.reflect(normal);
    }

    inline float3 float3::refract(const float3& incident, const float3& normal, float eta) noexcept {
        return incident.refract(normal, eta);
    }

    // ============================================================================
    // Swizzle Operations Implementation
    // ============================================================================

    inline float2 float3::xy() const noexcept { return float2(x, y); }
    inline float2 float3::xz() const noexcept { return float2(x, z); }
    inline float2 float3::yz() const noexcept { return float2(y, z); }
    inline float2 float3::yx() const noexcept { return float2(y, x); }
    inline float2 float3::zx() const noexcept { return float2(z, x); }
    inline float2 float3::zy() const noexcept { return float2(z, y); }

    inline float3 float3::yxz() const noexcept { return float3(y, x, z); }
    inline float3 float3::zxy() const noexcept { return float3(z, x, y); }
    inline float3 float3::zyx() const noexcept { return float3(z, y, x); }
    inline float3 float3::xzy() const noexcept { return float3(x, z, y); }
    inline float3 float3::xyx() const noexcept { return float3(x, y, x); }
    inline float3 float3::xyz() const noexcept { return float3(x, y, z); }
    inline float3 float3::xzx() const noexcept { return float3(x, z, x); }
    inline float3 float3::yxy() const noexcept { return float3(y, x, y); }
    inline float3 float3::yzy() const noexcept { return float3(y, z, y); }
    inline float3 float3::zxz() const noexcept { return float3(z, x, z); }
    inline float3 float3::zyz() const noexcept { return float3(z, y, z); }

    inline float float3::r() const noexcept { return x; }
    inline float float3::g() const noexcept { return y; }
    inline float float3::b() const noexcept { return z; }
    inline float2 float3::rg() const noexcept { return float2(x, y); }
    inline float2 float3::rb() const noexcept { return float2(x, z); }
    inline float2 float3::gb() const noexcept { return float2(y, z); }
    inline float3 float3::rgb() const noexcept { return *this; }
    inline float3 float3::bgr() const noexcept { return float3(z, y, x); }
    inline float3 float3::gbr() const noexcept { return float3(y, z, x); }

    // ============================================================================
    // Utility Methods Implementation
    // ============================================================================

    inline bool float3::isValid() const noexcept {
        return std::isfinite(x) && std::isfinite(y) && std::isfinite(z);
    }

    inline bool float3::approximately(const float3& other, float epsilon) const noexcept {
        return std::abs(x - other.x) <= epsilon &&
            std::abs(y - other.y) <= epsilon &&
            std::abs(z - other.z) <= epsilon;
    }

    inline bool float3::approximately_zero(float epsilon) const noexcept {
        return length_sq() <= epsilon * epsilon;
    }

    inline bool float3::is_normalized(float epsilon) const noexcept {
        return std::abs(length_sq() - 1.0f) <= epsilon * epsilon;
    }

    inline std::string float3::to_string() const {
        char buffer[64];
        std::snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f)", x, y, z);
        return std::string(buffer);
    }

    inline const float* float3::data() const noexcept { return &x; }
    inline float* float3::data() noexcept { return &x; }

    inline void float3::set_xy(const float2& xy) noexcept {
        x = xy.x; y = xy.y;
    }

    inline bool float3::operator==(const float3& rhs) const noexcept {
        return approximately(rhs);
    }

    inline bool float3::operator!=(const float3& rhs) const noexcept {
        return !(*this == rhs);
    }

    // ============================================================================
    // Global Operators Implementation
    // ============================================================================

    inline float3 operator+(float3 lhs, const float3& rhs) noexcept { return lhs += rhs; }
    inline float3 operator-(float3 lhs, const float3& rhs) noexcept { return lhs -= rhs; }
    inline float3 operator*(float3 lhs, const float3& rhs) noexcept { return lhs *= rhs; }
    inline float3 operator/(float3 lhs, const float3& rhs) noexcept { return lhs /= rhs; }
    inline float3 operator*(float3 vec, float scalar) noexcept { return vec *= scalar; }
    inline float3 operator*(float scalar, float3 vec) noexcept { return vec *= scalar; }
    inline float3 operator/(float3 vec, float scalar) noexcept { return vec /= scalar; }

    // ============================================================================
    // Global Functions Implementation
    // ============================================================================

    inline float distance(const float3& a, const float3& b) noexcept { return a.distance(b); }
    inline float distance_sq(const float3& a, const float3& b) noexcept { return a.distance_sq(b); }
    inline float dot(const float3& a, const float3& b) noexcept { return a.dot(b); }
    inline float3 cross(const float3& a, const float3& b) noexcept { return a.cross(b); }
    inline float3 normalize(const float3& vec) noexcept { return vec.normalize(); }
    inline float3 lerp(const float3& a, const float3& b, float t) noexcept { return float3::lerp(a, b, t); }
    inline float3 slerp(const float3& a, const float3& b, float t) noexcept { return float3::slerp(a, b, t); }
    inline bool approximately(const float3& a, const float3& b, float epsilon) noexcept { return a.approximately(b, epsilon); }
    inline bool is_normalized(const float3& vec, float epsilon) noexcept { return vec.is_normalized(epsilon); }
    inline bool are_orthogonal(const float3& a, const float3& b, float epsilon) noexcept { return MathFunctions::approximately(dot(a, b), 0.0f, epsilon); }
    inline bool is_orthonormal_basis(const float3& x, const float3& y, const float3& z, float epsilon) noexcept {
        return is_normalized(x, epsilon) && is_normalized(y, epsilon) && is_normalized(z, epsilon) &&
            are_orthogonal(x, y, epsilon) && are_orthogonal(x, z, epsilon) && are_orthogonal(y, z, epsilon);
    }
    inline bool isValid(const float3& vec) noexcept { return vec.isValid(); }

    // ============================================================================
    // HLSL-like Global Functions Implementation
    // ============================================================================

    inline float3 abs(const float3& vec) noexcept { return vec.abs(); }
    inline float3 sign(const float3& vec) noexcept { return vec.sign(); }
    inline float3 floor(const float3& vec) noexcept { return vec.floor(); }
    inline float3 ceil(const float3& vec) noexcept { return vec.ceil(); }
    inline float3 round(const float3& vec) noexcept { return vec.round(); }
    inline float3 frac(const float3& vec) noexcept { return vec.frac(); }
    inline float3 saturate(const float3& vec) noexcept { return vec.saturate(); }
    inline float3 step(float edge, const float3& vec) noexcept { return vec.step(edge); }
    inline float3 min(const float3& a, const float3& b) noexcept { return float3::min(a, b); }
    inline float3 max(const float3& a, const float3& b) noexcept { return float3::max(a, b); }
    inline float3 clamp(const float3& vec, const float3& min_val, const float3& max_val) noexcept { return float3::clamp(vec, min_val, max_val); }
    inline float3 clamp(const float3& vec, float min_val, float max_val) noexcept { return float3::clamp(vec, min_val, max_val); }
    inline float length(const float3& vec) noexcept { return vec.length(); }
    inline float length_sq(const float3& vec) noexcept { return vec.length_sq(); }

    // ============================================================================
    // Geometric Operations Implementation
    // ============================================================================

    inline float3 reflect(const float3& incident, const float3& normal) noexcept { return incident.reflect(normal); }
    inline float3 refract(const float3& incident, const float3& normal, float eta) noexcept { return incident.refract(normal, eta); }
    inline float3 project(const float3& vec, const float3& onto) noexcept { return vec.project(onto); }
    inline float3 reject(const float3& vec, const float3& from) noexcept { return vec.reject(from); }

    inline float angle_between(const float3& a, const float3& b) noexcept {
        float3 a_norm = a.normalize();
        float3 b_norm = b.normalize();
        float dot_val = dot(a_norm, b_norm);
        if (dot_val > 1.0f) dot_val = 1.0f;
        if (dot_val < -1.0f) dot_val = -1.0f;
        return std::acos(dot_val);
    }

    // ============================================================================
    // D3D Compatibility Functions Implementation
    // ============================================================================

#if defined(MATH_SUPPORT_D3DX)
    inline D3DXVECTOR3 ToD3DXVECTOR3(const float3& vec) noexcept { return D3DXVECTOR3(vec.x, vec.y, vec.z); }
    inline float3 FromD3DXVECTOR3(const D3DXVECTOR3& vec) noexcept { return float3(vec.x, vec.y, vec.z); }
    inline D3DCOLOR ToD3DCOLOR(const float3& color) noexcept { return D3DCOLOR_COLORVALUE(color.x, color.y, color.z, 1.0f); }
    inline void float3ArrayToD3D(const float3* source, D3DXVECTOR3* destination, size_t count) noexcept {
        for (size_t i = 0; i < count; ++i) destination[i] = ToD3DXVECTOR3(source[i]);
    }
    inline void D3DArrayTofloat3(const D3DXVECTOR3* source, float3* destination, size_t count) noexcept {
        for (size_t i = 0; i < count; ++i) destination[i] = FromD3DXVECTOR3(source[i]);
    }
#endif

    // ============================================================================
    // Global Constants Implementation
    // ============================================================================

    inline const float3 float3_Zero(0.0f, 0.0f, 0.0f);
    inline const float3 float3_One(1.0f, 1.0f, 1.0f);
    inline const float3 float3_UnitX(1.0f, 0.0f, 0.0f);
    inline const float3 float3_UnitY(0.0f, 1.0f, 0.0f);
    inline const float3 float3_UnitZ(0.0f, 0.0f, 1.0f);
    inline const float3 float3_Forward(0.0f, 0.0f, 1.0f);
    inline const float3 float3_Up(0.0f, 1.0f, 0.0f);
    inline const float3 float3_Right(1.0f, 0.0f, 0.0f);

} // namespace Math
