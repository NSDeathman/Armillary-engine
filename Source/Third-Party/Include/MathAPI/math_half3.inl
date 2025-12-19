/**
 * @file math_half3.inl
 * @brief Implementation of 3-dimensional half-precision vector class
 * @note Optimized for 3D graphics, normals, colors with SSE optimization
 */

namespace Math {

    // ============================================================================
    // Constructors Implementation
    // ============================================================================

    inline half3::half3() noexcept : x(half::from_bits(0)), y(half::from_bits(0)), z(half::from_bits(0)) {}

    inline half3::half3(half x, half y, half z) noexcept : x(x), y(y), z(z) {}

    inline half3::half3(half scalar) noexcept : x(scalar), y(scalar), z(scalar) {}

    inline half3::half3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}

    inline half3::half3(float scalar) noexcept : x(scalar), y(scalar), z(scalar) {}

    inline half3::half3(const half2& vec, half z) noexcept : x(vec.x), y(vec.y), z(z) {}

    inline half3::half3(const float3& vec) noexcept : x(vec.x), y(vec.y), z(vec.z) {}

    inline half3::half3(const float2& vec, float z) noexcept : x(vec.x), y(vec.y), z(z) {}

    // ============================================================================
    // Assignment Operators Implementation
    // ============================================================================

    inline half3& half3::operator=(const float3& vec) noexcept
    {
        x = vec.x;
        y = vec.y;
        z = vec.z;
        return *this;
    }

    inline half3& half3::operator=(half scalar) noexcept
    {
        x = y = z = scalar;
        return *this;
    }

    inline half3& half3::operator=(float scalar) noexcept
    {
        x = y = z = scalar;
        return *this;
    }

    // ============================================================================
    // Compound Assignment Operators Implementation
    // ============================================================================

    inline half3& half3::operator+=(const half3& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        z += rhs.z;
        return *this;
    }

    inline half3& half3::operator-=(const half3& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        z -= rhs.z;
        return *this;
    }

    inline half3& half3::operator*=(const half3& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        z *= rhs.z;
        return *this;
    }

    inline half3& half3::operator/=(const half3& rhs) noexcept
    {
        x /= rhs.x;
        y /= rhs.y;
        z /= rhs.z;
        return *this;
    }

    inline half3& half3::operator*=(half scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    inline half3& half3::operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        z *= scalar;
        return *this;
    }

    inline half3& half3::operator/=(half scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    inline half3& half3::operator/=(float scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        z /= scalar;
        return *this;
    }

    // ============================================================================
    // Unary Operators Implementation
    // ============================================================================

    inline half3 half3::operator+() const noexcept
    {
        return *this;
    }

    inline half3 half3::operator-() const noexcept
    {
        return half3(-x, -y, -z);
    }

    // ============================================================================
    // Access Operators Implementation
    // ============================================================================

    inline half& half3::operator[](int index) noexcept
    {
        return (&x)[index];
    }

    inline const half& half3::operator[](int index) const noexcept
    {
        return (&x)[index];
    }

    // ============================================================================
    // Conversion Operators Implementation
    // ============================================================================

    inline half3::operator float3() const noexcept
    {
        return float3(float(x), float(y), float(z));
    }

    // ============================================================================
    // Static Constructors Implementation
    // ============================================================================

    inline half3 half3::zero() noexcept
    {
        return half3(half::from_bits(0), half::from_bits(0), half::from_bits(0));
    }

    inline half3 half3::one() noexcept
    {
        return half3(half::from_bits(0x3C00), half::from_bits(0x3C00), half::from_bits(0x3C00));
    }

    inline half3 half3::unit_x() noexcept
    {
        return half3(half::from_bits(0x3C00), half::from_bits(0), half::from_bits(0));
    }

    inline half3 half3::unit_y() noexcept
    {
        return half3(half::from_bits(0), half::from_bits(0x3C00), half::from_bits(0));
    }

    inline half3 half3::unit_z() noexcept
    {
        return half3(half::from_bits(0), half::from_bits(0), half::from_bits(0x3C00));
    }

    inline half3 half3::forward() noexcept
    {
        return unit_z();
    }

    inline half3 half3::up() noexcept
    {
        return unit_y();
    }

    inline half3 half3::right() noexcept
    {
        return unit_x();
    }

    // ============================================================================
    // Mathematical Functions Implementation
    // ============================================================================

    inline half half3::length() const noexcept
    {
        float fx = float(x);
        float fy = float(y);
        float fz = float(z);
        return half(std::sqrt(fx * fx + fy * fy + fz * fz));
    }

    inline half half3::length_sq() const noexcept
    {
        float fx = float(x);
        float fy = float(y);
        float fz = float(z);
        return half(fx * fx + fy * fy + fz * fz);
    }

    inline half3 half3::normalize() const noexcept
    {
        half len = length();

        if (len.approximately_zero(Constants::Constants<float>::Epsilon * 10.0f))
            return half3::zero();

        half inv_len = half(1.0f) / len;
        return half3(x * inv_len, y * inv_len, z * inv_len);
    }

    inline half half3::dot(const half3& other) const noexcept
    {
        return half3::dot(*this, other);
    }

    inline half3 half3::cross(const half3& other) const noexcept
    {
        return half3::cross(*this, other);
    }

    inline half half3::distance(const half3& other) const noexcept
    {
        half3 diff = *this - other;
        return diff.length();
    }

    inline half half3::distance_sq(const half3& other) const noexcept
    {
        half3 diff = *this - other;
        return diff.length_sq();
    }

    // ============================================================================
    // HLSL-like Functions Implementation
    // ============================================================================

    inline half3 half3::abs() const noexcept
    {
        return half3(Math::abs(x), Math::abs(y), Math::abs(z));
    }

    inline half3 half3::sign() const noexcept
    {
        return half3(Math::sign(x), Math::sign(y), Math::sign(z));
    }

    inline half3 half3::floor() const noexcept
    {
        return half3(Math::floor(x), Math::floor(y), Math::floor(z));
    }

    inline half3 half3::ceil() const noexcept
    {
        return half3(Math::ceil(x), Math::ceil(y), Math::ceil(z));
    }

    inline half3 half3::round() const noexcept
    {
        return half3(Math::round(x), Math::round(y), Math::round(z));
    }

    inline half3 half3::frac() const noexcept
    {
        return half3(Math::frac(x), Math::frac(y), Math::frac(z));
    }

    inline half3 half3::saturate() const noexcept
    {
        return half3::saturate(*this);
    }

    inline half3 half3::step(half edge) const noexcept
    {
        return half3(Math::step(edge, x), Math::step(edge, y), Math::step(edge, z));
    }

    // ============================================================================
    // Geometric Operations Implementation
    // ============================================================================

    inline half3 half3::reflect(const half3& normal) const noexcept
    {
        return half3::reflect(*this, normal);
    }

    inline half3 half3::refract(const half3& normal, half eta) const noexcept
    {
        return half3::refract(*this, normal, eta);
    }

    inline half3 half3::project(const half3& onto) const noexcept
    {
        half onto_length_sq = onto.length_sq();

        if (onto_length_sq.approximately_zero(Constants::Constants<float>::Epsilon * 10.0f))
            return half3::zero();

        half dot_val = dot(onto);

        if (dot_val.is_finite() && onto_length_sq.is_finite()) {
            return onto * (dot_val / onto_length_sq);
        }
        return half3::zero();
    }

    inline half3 half3::reject(const half3& from) const noexcept
    {
        half3 projected = project(from);
        return *this - projected;
    }

    // ============================================================================
    // Static Mathematical Functions Implementation
    // ============================================================================

    inline half half3::dot(const half3& a, const half3& b) noexcept
    {
        float result = float(a.x) * float(b.x) +
            float(a.y) * float(b.y) +
            float(a.z) * float(b.z);
        return half(result);
    }

    inline half3 half3::cross(const half3& a, const half3& b) noexcept
    {
        float x = float(a.y) * float(b.z) - float(a.z) * float(b.y);
        float y = float(a.z) * float(b.x) - float(a.x) * float(b.z);
        float z = float(a.x) * float(b.y) - float(a.y) * float(b.x);

        return half3(half(x), half(y), half(z));
    }

    inline half3 half3::lerp(const half3& a, const half3& b, half t) noexcept
    {
        __m128 a_vec = _mm_set_ps(0.0f, float(a.z), float(a.y), float(a.x));
        __m128 b_vec = _mm_set_ps(0.0f, float(b.z), float(b.y), float(b.x));
        __m128 t_vec = _mm_set1_ps(float(t));
        __m128 one_minus_t = _mm_set1_ps(1.0f - float(t));

        __m128 part1 = _mm_mul_ps(a_vec, one_minus_t);
        __m128 part2 = _mm_mul_ps(b_vec, t_vec);
        __m128 result = _mm_add_ps(part1, part2);

        alignas(16) float temp[4];
        _mm_store_ps(temp, result);
        return half3(half(temp[0]), half(temp[1]), half(temp[2]));
    }

    inline half3 half3::lerp(const half3& a, const half3& b, float t) noexcept
    {
        return lerp(a, b, half(t));
    }

    inline half3 half3::saturate(const half3& vec) noexcept
    {
        return half3(
            Math::saturate(vec.x),
            Math::saturate(vec.y),
            Math::saturate(vec.z)
        );
    }

    inline half3 half3::min(const half3& a, const half3& b) noexcept
    {
        return half3(
            (a.x < b.x) ? a.x : b.x,
            (a.y < b.y) ? a.y : b.y,
            (a.z < b.z) ? a.z : b.z
        );
    }

    inline half3 half3::max(const half3& a, const half3& b) noexcept
    {
        return half3(
            (a.x > b.x) ? a.x : b.x,
            (a.y > b.y) ? a.y : b.y,
            (a.z > b.z) ? a.z : b.z
        );
    }

    inline half3 half3::reflect(const half3& incident, const half3& normal) noexcept
    {
        half dot_val = dot(incident, normal);
        return incident - half(2.0f) * dot_val * normal;
    }

    inline half3 half3::refract(const half3& incident, const half3& normal, half eta) noexcept
    {
        half dot_ni = dot(normal, incident);
        half k = half(1.0f) - eta * eta * (half(1.0f) - dot_ni * dot_ni);

        if (k < half(0.0f))
            return half3::zero(); // total internal reflection

        return incident * eta - normal * (eta * dot_ni + sqrt(k));
    }

    // ============================================================================
    // Color Operations Implementation
    // ============================================================================

    inline half half3::luminance() const noexcept
    {
        return half(0.2126f) * x + half(0.7152f) * y + half(0.0722f) * z;
    }

    inline half3 half3::rgb_to_grayscale() const noexcept
    {
        half luma = luminance();
        return half3(luma, luma, luma);
    }

    inline half3 half3::gamma_correct(half gamma) const noexcept
    {
        return half3(Math::pow(x, gamma), Math::pow(y, gamma), Math::pow(z, gamma));
    }

    inline half3 half3::srgb_to_linear() const noexcept
    {
        auto srgb_to_linear_channel = [](half channel) -> half {
            float c = float(channel);
            return (c <= 0.04045f) ? half(c / 12.92f) : half(std::pow((c + 0.055f) / 1.055f, 2.4f));
        };

        return half3(srgb_to_linear_channel(x), srgb_to_linear_channel(y), srgb_to_linear_channel(z));
    }

    inline half3 half3::linear_to_srgb() const noexcept
    {
        auto linear_to_srgb_channel = [](half channel) -> half {
            float c = float(channel);
            return (c <= 0.0031308f) ? half(c * 12.92f) : half(1.055f * std::pow(c, 1.0f / 2.4f) - 0.055f);
        };

        return half3(linear_to_srgb_channel(x), linear_to_srgb_channel(y), linear_to_srgb_channel(z));
    }

    // ============================================================================
    // Swizzle Operations Implementation
    // ============================================================================

    inline half2 half3::xy() const noexcept { return half2(x, y); }
    inline half2 half3::xz() const noexcept { return half2(x, z); }
    inline half2 half3::yz() const noexcept { return half2(y, z); }
    inline half2 half3::yx() const noexcept { return half2(y, x); }
    inline half2 half3::zx() const noexcept { return half2(z, x); }
    inline half2 half3::zy() const noexcept { return half2(z, y); }

    inline half3 half3::yxz() const noexcept { return half3(y, x, z); }
    inline half3 half3::zxy() const noexcept { return half3(z, x, y); }
    inline half3 half3::zyx() const noexcept { return half3(z, y, x); }
    inline half3 half3::xzy() const noexcept { return half3(x, z, y); }

    inline half half3::r() const noexcept { return x; }
    inline half half3::g() const noexcept { return y; }
    inline half half3::b() const noexcept { return z; }
    inline half2 half3::rg() const noexcept { return half2(x, y); }
    inline half2 half3::rb() const noexcept { return half2(x, z); }
    inline half2 half3::gb() const noexcept { return half2(y, z); }
    inline half3 half3::rgb() const noexcept { return *this; }
    inline half3 half3::bgr() const noexcept { return half3(z, y, x); }
    inline half3 half3::gbr() const noexcept { return half3(y, z, x); }

    // ============================================================================
    // Utility Methods Implementation
    // ============================================================================

    inline bool half3::is_valid() const noexcept
    {
        return x.is_finite() && y.is_finite() && z.is_finite();
    }

    inline bool half3::approximately(const half3& other, float epsilon) const noexcept
    {
        return x.approximately(other.x, epsilon) &&
            y.approximately(other.y, epsilon) &&
            z.approximately(other.z, epsilon);
    }

    inline bool half3::approximately_zero(float epsilon) const noexcept
    {
        return std::abs(float(x)) <= epsilon &&
            std::abs(float(y)) <= epsilon &&
            std::abs(float(z)) <= epsilon;
    }

    inline bool half3::is_normalized(float epsilon) const noexcept
    {
        half len_sq = length_sq();
        float len_sq_f = float(len_sq);
        return std::abs(len_sq_f - 1.0f) <= epsilon;
    }

    inline std::string half3::to_string() const
    {
        char buffer[64];
        std::snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f)", float(x), float(y), float(z));
        return std::string(buffer);
    }

    inline const half* half3::data() const noexcept
    {
        return &x;
    }

    inline half* half3::data() noexcept
    {
        return &x;
    }

    inline void half3::set_xy(const half2& xy) noexcept
    {
        x = xy.x;
        y = xy.y;
    }

    // ============================================================================
    // Comparison Operators Implementation
    // ============================================================================

    inline bool half3::operator==(const half3& rhs) const noexcept
    {
        return approximately(rhs);
    }

    inline bool half3::operator!=(const half3& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    // ============================================================================
    // Global Operators Implementation
    // ============================================================================

    inline half3 operator+(half3 lhs, const half3& rhs) noexcept
    {
        return lhs += rhs;
    }

    inline half3 operator-(half3 lhs, const half3& rhs) noexcept
    {
        return lhs -= rhs;
    }

    inline half3 operator*(half3 lhs, const half3& rhs) noexcept
    {
        return lhs *= rhs;
    }

    inline half3 operator/(half3 lhs, const half3& rhs) noexcept
    {
        return lhs /= rhs;
    }

    inline half3 operator*(half3 vec, half scalar) noexcept
    {
        return vec *= scalar;
    }

    inline half3 operator*(half scalar, half3 vec) noexcept
    {
        return vec *= scalar;
    }

    inline half3 operator/(half3 vec, half scalar) noexcept
    {
        return vec /= scalar;
    }

    inline half3 operator*(half3 vec, float scalar) noexcept
    {
        return vec *= scalar;
    }

    inline half3 operator*(float scalar, half3 vec) noexcept
    {
        return vec *= scalar;
    }

    inline half3 operator/(half3 vec, float scalar) noexcept
    {
        return vec /= scalar;
    }

    // ============================================================================
    // Mixed Type Operators Implementation
    // ============================================================================

    inline half3 operator+(const half3& lhs, const float3& rhs) noexcept
    {
        return half3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    inline half3 operator-(const half3& lhs, const float3& rhs) noexcept
    {
        return half3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    inline half3 operator*(const half3& lhs, const float3& rhs) noexcept
    {
        return half3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }

    inline half3 operator/(const half3& lhs, const float3& rhs) noexcept
    {
        return half3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
    }

    inline half3 operator+(const float3& lhs, const half3& rhs) noexcept
    {
        return half3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    }

    inline half3 operator-(const float3& lhs, const half3& rhs) noexcept
    {
        return half3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    }

    inline half3 operator*(const float3& lhs, const half3& rhs) noexcept
    {
        return half3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    }

    inline half3 operator/(const float3& lhs, const half3& rhs) noexcept
    {
        return half3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
    }

    // ============================================================================
    // Global Mathematical Functions Implementation
    // ============================================================================

    inline half distance(const half3& a, const half3& b) noexcept
    {
        return (b - a).length();
    }

    inline half distance_sq(const half3& a, const half3& b) noexcept
    {
        return (b - a).length_sq();
    }

    inline half dot(const half3& a, const half3& b) noexcept
    {
        return half3::dot(a, b);
    }

    inline half3 cross(const half3& a, const half3& b) noexcept
    {
        return half3::cross(a, b);
    }

    inline half3 normalize(const half3& vec) noexcept
    {
        return vec.normalize();
    }

    inline half3 lerp(const half3& a, const half3& b, half t) noexcept
    {
        return half3::lerp(a, b, t);
    }

    inline half3 lerp(const half3& a, const half3& b, float t) noexcept
    {
        return half3::lerp(a, b, t);
    }

    inline half3 saturate(const half3& vec) noexcept
    {
        return half3::saturate(vec);
    }

    inline half3 reflect(const half3& incident, const half3& normal) noexcept
    {
        return half3::reflect(incident, normal);
    }

    inline half3 refract(const half3& incident, const half3& normal, half eta) noexcept
    {
        return half3::refract(incident, normal, eta);
    }

    inline bool approximately(const half3& a, const half3& b, float epsilon) noexcept
    {
        return a.approximately(b, epsilon);
    }

    inline bool is_valid(const half3& vec) noexcept
    {
        return vec.is_valid();
    }

    inline bool is_normalized(const half3& vec, float epsilon) noexcept
    {
        return vec.is_normalized(epsilon);
    }

    // ============================================================================
    // HLSL-like Global Functions Implementation
    // ============================================================================

    inline half3 abs(const half3& vec) noexcept
    {
        return vec.abs();
    }

    inline half3 sign(const half3& vec) noexcept
    {
        return vec.sign();
    }

    inline half3 floor(const half3& vec) noexcept
    {
        return vec.floor();
    }

    inline half3 ceil(const half3& vec) noexcept
    {
        return vec.ceil();
    }

    inline half3 round(const half3& vec) noexcept
    {
        return vec.round();
    }

    inline half3 frac(const half3& vec) noexcept
    {
        return vec.frac();
    }

    inline half3 step(half edge, const half3& vec) noexcept
    {
        return vec.step(edge);
    }

    inline half3 min(const half3& a, const half3& b) noexcept
    {
        return half3::min(a, b);
    }

    inline half3 max(const half3& a, const half3& b) noexcept
    {
        return half3::max(a, b);
    }

    inline half3 clamp(const half3& vec, const half3& min_val, const half3& max_val) noexcept
    {
        return half3::min(half3::max(vec, min_val), max_val);
    }

    inline half3 clamp(const half3& vec, float min_val, float max_val) noexcept
    {
        return half3(
            Math::clamp(vec.x, min_val, max_val),
            Math::clamp(vec.y, min_val, max_val),
            Math::clamp(vec.z, min_val, max_val)
        );
    }

    inline half3 smoothstep(half edge0, half edge1, const half3& vec) noexcept
    {
        return half3(
            Math::smoothstep(edge0, edge1, vec.x),
            Math::smoothstep(edge0, edge1, vec.y),
            Math::smoothstep(edge0, edge1, vec.z)
        );
    }

    // ============================================================================
    // Geometric Operations Implementation
    // ============================================================================

    inline half3 project(const half3& vec, const half3& onto) noexcept
    {
        return vec.project(onto);
    }

    inline half3 reject(const half3& vec, const half3& from) noexcept
    {
        return vec.reject(from);
    }

    inline half angle_between(const half3& a, const half3& b) noexcept
    {
        half3 a_norm = a.normalize();
        half3 b_norm = b.normalize();

        if (!a_norm.is_normalized(0.01f) || !b_norm.is_normalized(0.01f)) {
            return half(0.0f);
        }

        half dot_val = dot(a_norm, b_norm);
        dot_val = Math::clamp(dot_val, -half(1.0f), half(1.0f));
        return half(std::acos(float(dot_val)));
    }

    // ============================================================================
    // Color Operations Implementation
    // ============================================================================

    inline half3 rgb_to_grayscale(const half3& rgb) noexcept
    {
        return rgb.rgb_to_grayscale();
    }

    inline half luminance(const half3& rgb) noexcept
    {
        return rgb.luminance();
    }

    inline half3 gamma_correct(const half3& color, half gamma) noexcept
    {
        return color.gamma_correct(gamma);
    }

    inline half3 srgb_to_linear(const half3& srgb) noexcept
    {
        return srgb.srgb_to_linear();
    }

    inline half3 linear_to_srgb(const half3& linear) noexcept
    {
        return linear.linear_to_srgb();
    }

    // ============================================================================
    // Type Conversion Functions Implementation
    // ============================================================================

    inline float3 to_float3(const half3& vec) noexcept
    {
        return float3(float(vec.x), float(vec.y), float(vec.z));
    }

    inline half3 to_half3(const float3& vec) noexcept
    {
        return half3(vec.x, vec.y, vec.z);
    }

    // ============================================================================
    // Utility Functions Implementation
    // ============================================================================

    inline half3 ensure_normalized(const half3& normal, const half3& fallback) noexcept
    {
        half len_sq = normal.length_sq();
        if (len_sq > half::epsilon())
        {
            half len = sqrt(len_sq);
            if (Math::abs(len - half_One) > half(0.01f))
            {
                return normal / len;
            }
            return normal;
        }
        return fallback;
    }

    // ============================================================================
    // Global Constants Implementation
    // ============================================================================

    inline const half3 half3_Zero(half_Zero);
    inline const half3 half3_One(half_One);
    inline const half3 half3_UnitX(half_One, half_Zero, half_Zero);
    inline const half3 half3_UnitY(half_Zero, half_One, half_Zero);
    inline const half3 half3_UnitZ(half_Zero, half_Zero, half_One);
    inline const half3 half3_Forward(half_Zero, half_Zero, half_One);
    inline const half3 half3_Up(half_Zero, half_One, half_Zero);
    inline const half3 half3_Right(half_One, half_Zero, half_Zero);
    inline const half3 half3_Red(half_One, half_Zero, half_Zero);
    inline const half3 half3_Green(half_Zero, half_One, half_Zero);
    inline const half3 half3_Blue(half_Zero, half_Zero, half_One);
    inline const half3 half3_White(half_One);
    inline const half3 half3_Black(half_Zero);
    inline const half3 half3_Yellow(half_One, half_One, half_Zero);
    inline const half3 half3_Cyan(half_Zero, half_One, half_One);
    inline const half3 half3_Magenta(half_One, half_Zero, half_One);

} // namespace Math
