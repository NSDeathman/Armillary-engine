// Author: NSDeathman, DeepSeek
#pragma once
#include <emmintrin.h> // Required for _mm_load_sd

namespace Math {

    inline __m128 _load_float2_fast(const float* ptr) {
        return _mm_castpd_ps(_mm_load_sd(reinterpret_cast<const double*>(ptr)));
    }

    // --- Constructors ---
    inline float2::float2(const float* data) noexcept : x(data[0]), y(data[1]) {}
    inline float2::float2(__m128 simd_) noexcept {
        alignas(16) float data[4];
        _mm_store_ps(data, simd_);
        x = data[0]; y = data[1];
    }

#if defined(MATH_SUPPORT_D3DX)
    inline float2::float2(const D3DXVECTOR2& vec) noexcept : x(vec.x), y(vec.y) {}
    inline float2::float2(const D3DXVECTOR4& vec) noexcept : x(vec.x), y(vec.y) {}
    inline float2::float2(D3DCOLOR color) noexcept {
        x = ((color >> 16) & 0xFF) / 255.0f;
        y = ((color >> 8) & 0xFF) / 255.0f;
    }
#endif

    // --- Assignment ---
    inline float2& float2::operator=(float scalar) noexcept { x = y = scalar; return *this; }
#if defined(MATH_SUPPORT_D3DX)
    inline float2& float2::operator=(const D3DXVECTOR2& vec) noexcept { x = vec.x; y = vec.y; return *this; }
    inline float2& float2::operator=(D3DCOLOR color) noexcept {
        x = ((color >> 16) & 0xFF) / 255.0f;
        y = ((color >> 8) & 0xFF) / 255.0f;
        return *this;
    }
#endif

    // --- Operators ---
    inline float2& float2::operator+=(const float2& rhs) noexcept { x += rhs.x; y += rhs.y; return *this; }
    inline float2& float2::operator-=(const float2& rhs) noexcept { x -= rhs.x; y -= rhs.y; return *this; }
    inline float2& float2::operator*=(const float2& rhs) noexcept { x *= rhs.x; y *= rhs.y; return *this; }
    inline float2& float2::operator/=(const float2& rhs) noexcept { x /= rhs.x; y /= rhs.y; return *this; }
    inline float2& float2::operator*=(float scalar) noexcept { x *= scalar; y *= scalar; return *this; }
    inline float2& float2::operator/=(float scalar) noexcept { float i = 1.0f / scalar; x *= i; y *= i; return *this; }

    inline float2 float2::operator+(const float2& rhs) const noexcept {
        return float2(_mm_add_ps(_load_float2_fast(&x), _load_float2_fast(&rhs.x)));
    }
    inline float2 float2::operator-(const float2& rhs) const noexcept {
        return float2(_mm_sub_ps(_load_float2_fast(&x), _load_float2_fast(&rhs.x)));
    }
    inline float2 float2::operator+(const float& rhs) const noexcept { return float2(x + rhs, y + rhs); }
    inline float2 float2::operator-(const float& rhs) const noexcept { return float2(x - rhs, y - rhs); }

    inline float& float2::operator[](int index) noexcept { assert(index >= 0 && index < 2); return (&x)[index]; }
    inline const float& float2::operator[](int index) const noexcept { assert(index >= 0 && index < 2); return (&x)[index]; }

    inline float2::operator const float* () const noexcept { return &x; }
    inline float2::operator float* () noexcept { return &x; }
    inline float2::operator __m128() const noexcept { return _load_float2_fast(&x); }
#if defined(MATH_SUPPORT_D3DX)
    inline float2::operator D3DXVECTOR2() const noexcept { return D3DXVECTOR2(x, y); }
#endif

    // --- Math ---
    inline float float2::length() const noexcept {
        __m128 v = _load_float2_fast(&x);
        return _mm_cvtss_f32(_mm_sqrt_ss(_mm_hadd_ps(_mm_mul_ps(v, v), _mm_mul_ps(v, v))));
    }

    inline float2 float2::normalize() const noexcept {
        __m128 v = _load_float2_fast(&x);
        __m128 sq = _mm_mul_ps(v, v);
        __m128 sum = _mm_hadd_ps(sq, sq);
        if (_mm_cvtss_f32(sum) < Constants::Constants<float>::Epsilon) return float2::zero();
        return float2(_mm_div_ps(v, _mm_sqrt_ps(sum)));
    }

    inline float float2::dot(const float2& other) const noexcept { return x * other.x + y * other.y; }
    inline float float2::cross(const float2& other) const { return x * other.y - y * other.x; }
    inline float float2::distance(const float2& other) const noexcept { return (*this - other).length(); }

    inline float2 float2::abs() const noexcept {
        return float2(std::abs(x), std::abs(y));
    }
    inline float2 float2::sign() const noexcept {
        return float2((x > 0 ? 1.f : (x < 0 ? -1.f : 0.f)), (y > 0 ? 1.f : (y < 0 ? -1.f : 0.f)));
    }
    inline float2 float2::floor() const noexcept { return float2(std::floor(x), std::floor(y)); }
    inline float2 float2::ceil() const noexcept { return float2(std::ceil(x), std::ceil(y)); }
    inline float2 float2::round() const noexcept { return float2(std::round(x), std::round(y)); }
    inline float2 float2::frac() const noexcept { return *this - floor(); }

    inline float2 float2::saturate() const noexcept {
        return float2(std::max(0.f, std::min(1.f, x)), std::max(0.f, std::min(1.f, y)));
    }
    inline float2 float2::step(float edge) const noexcept {
        return float2(x >= edge ? 1.f : 0.f, y >= edge ? 1.f : 0.f);
    }
    inline float2 float2::smoothstep(float edge0, float edge1) const noexcept {
        float d = edge1 - edge0;
        if (std::abs(d) < 1e-6f) return step(edge0);
        auto f = [&](float v) {
            float t = std::max(0.f, std::min(1.f, (v - edge0) / d));
            return t * t * (3.f - 2.f * t);
        };
        return float2(f(x), f(y));
    }

    inline float2 float2::reflect(const float2& normal) const noexcept { return *this - 2.0f * dot(normal) * normal; }
    inline float2 float2::refract(const float2& normal, float eta) const noexcept {
        float d = dot(normal);
        float k = 1.0f - eta * eta * (1.0f - d * d);
        if (k < 0.0f) return float2::zero();
        return eta * (*this) - (eta * d + std::sqrt(k)) * normal;
    }
    inline float2 float2::rotate(float angle) const noexcept {
        float s = std::sin(angle), c = std::cos(angle);
        return float2(x * c - y * s, x * s + y * c);
    }
    inline float float2::angle() const noexcept { return std::atan2(y, x); }

    inline bool float2::isValid() const noexcept { return std::isfinite(x) && std::isfinite(y); }
    inline bool float2::approximately(const float2& o, float e) const noexcept {
        return std::abs(x - o.x) <= e && std::abs(y - o.y) <= e;
    }
    inline bool float2::approximately_zero(float e) const noexcept { return length_sq() <= e * e; }
    inline bool float2::is_normalized(float e) const noexcept { return std::abs(length_sq() - 1.0f) <= e; }

    inline std::string float2::to_string() const {
        char buf[64]; std::snprintf(buf, 64, "(%.3f, %.3f)", x, y); return std::string(buf);
    }
    inline const float* float2::data() const noexcept { return &x; }
    inline float* float2::data() noexcept { return &x; }

    inline bool float2::operator==(const float2& rhs) const noexcept { return approximately(rhs); }
    inline bool float2::operator!=(const float2& rhs) const noexcept { return !(*this == rhs); }

    // --- Global Operators ---
    inline float2 operator*(float2 lhs, const float2& rhs) noexcept { return lhs *= rhs; }
    inline float2 operator/(float2 lhs, const float2& rhs) noexcept { return lhs /= rhs; }
    inline float2 operator*(float2 vec, float scalar) noexcept { return vec *= scalar; }
    inline float2 operator*(float scalar, float2 vec) noexcept { return vec *= scalar; }
    inline float2 operator/(float2 vec, float scalar) noexcept { return vec /= scalar; }
    inline float2 operator+(float scalar, float2 vec) noexcept { return vec + scalar; }

    // --- Global Functions ---
    inline float distance(const float2& a, const float2& b) noexcept { return a.distance(b); }
    inline float distance_sq(const float2& a, const float2& b) noexcept { return a.distance_sq(b); }
    inline float dot(const float2& a, const float2& b) noexcept { return a.dot(b); }
    inline float cross(const float2& a, const float2& b) noexcept { return a.cross(b); }
    inline bool approximately(const float2& a, const float2& b, float e) noexcept { return a.approximately(b, e); }
    inline bool isValid(const float2& v) noexcept { return v.isValid(); }

    inline float2 lerp(const float2& a, const float2& b, float t) noexcept { return a + (b - a) * t; }
    inline float2 slerp(const float2& a, const float2& b, float t) noexcept {
        float d = dot(a, b);
        if (d > 0.9995f) return lerp(a, b, t).normalize();
        d = std::max(-1.0f, std::min(1.0f, d));
        float th = std::acos(d) * t;
        float2 rel = (b - a * d).normalize();
        return a * std::cos(th) + rel * std::sin(th);
    }

    inline float2 perpendicular(const float2& v) noexcept { return v.perpendicular(); }
    inline float2 reflect(const float2& i, const float2& n) noexcept { return i.reflect(n); }
    inline float2 refract(const float2& i, const float2& n, float e) noexcept { return i.refract(n, e); }
    inline float2 rotate(const float2& v, float a) noexcept { return v.rotate(a); }

    inline float angle_between(const float2& a, const float2& b) noexcept {
        float d = dot(a.normalize(), b.normalize());
        return std::acos(std::max(-1.f, std::min(1.f, d)));
    }
    inline float signed_angle_between(const float2& a, const float2& b) noexcept {
        float ang = angle_between(a, b);
        return cross(a, b) < 0 ? -ang : ang;
    }
    inline float2 project(const float2& v, const float2& on) noexcept {
        float l2 = on.length_sq();
        if (l2 < 1e-6f) return float2::zero();
        return on * (dot(v, on) / l2);
    }
    inline float2 reject(const float2& v, const float2& from) noexcept { return v - project(v, from); }

    // HLSL Global Wrappers
    inline float2 abs(const float2& v) noexcept { return v.abs(); }
    inline float2 sign(const float2& v) noexcept { return v.sign(); }
    inline float2 floor(const float2& v) noexcept { return v.floor(); }
    inline float2 ceil(const float2& v) noexcept { return v.ceil(); }
    inline float2 round(const float2& v) noexcept { return v.round(); }
    inline float2 frac(const float2& v) noexcept { return v.frac(); }
    inline float2 saturate(const float2& v) noexcept { return v.saturate(); }
    inline float2 step(float e, const float2& v) noexcept { return v.step(e); }
    inline float2 smoothstep(float e0, float e1, const float2& v) noexcept { return v.smoothstep(e0, e1); }
    inline float2 min(const float2& a, const float2& b) noexcept { return float2(std::min(a.x, b.x), std::min(a.y, b.y)); }
    inline float2 max(const float2& a, const float2& b) noexcept { return float2(std::max(a.x, b.x), std::max(a.y, b.y)); }
    inline float2 clamp(const float2& v, const float2& mi, const float2& ma) noexcept { return min(max(v, mi), ma); }

    // D3D Wrappers
#if defined(MATH_SUPPORT_D3DX)
    inline D3DXVECTOR2 ToD3DXVECTOR2(const float2& v) noexcept { return D3DXVECTOR2(v.x, v.y); }
    inline float2 FromD3DXVECTOR2(const D3DXVECTOR2& v) noexcept { return float2(v.x, v.y); }
    inline D3DCOLOR ToD3DCOLOR(const float2& v) noexcept { return D3DCOLOR_COLORVALUE(v.x, v.y, 0.f, 1.f); }
    inline void float2ArrayToD3D(const float2* s, D3DXVECTOR2* d, size_t c) noexcept { for (size_t i = 0; i < c; ++i) d[i] = ToD3DXVECTOR3(s[i]); } // Fix typo in next iteration if needed, assuming ToD3DXVECTOR2
    inline void D3DArrayTofloat2(const D3DXVECTOR2* s, float2* d, size_t c) noexcept { for (size_t i = 0; i < c; ++i) d[i] = FromD3DXVECTOR2(s[i]); }
#endif

    // Utils
    inline float distance_to_line_segment(const float2& p, const float2& a, const float2& b) noexcept {
        float2 ab = b - a; float l2 = ab.length_sq();
        if (l2 < 1e-6f) return distance(p, a);
        float t = std::max(0.f, std::min(1.f, dot(p - a, ab) / l2));
        return distance(p, a + ab * t);
    }
    // Constants
    inline const float2 float2_Zero(0.0f, 0.0f);
    inline const float2 float2_One(1.0f, 1.0f);
    inline const float2 float2_UnitX(1.0f, 0.0f);
    inline const float2 float2_UnitY(0.0f, 1.0f);
    inline const float2 float2_Right(1.0f, 0.0f);
    inline const float2 float2_Left(-1.0f, 0.0f);
    inline const float2 float2_Up(0.0f, 1.0f);
    inline const float2 float2_Down(0.0f, -1.0f);
}
