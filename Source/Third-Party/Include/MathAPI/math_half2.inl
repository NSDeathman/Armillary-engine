/**
 * @file math_half2.inl
 * @brief Inline implementations for 2-dimensional half-precision vector class
 * @note Optimized for texture coordinates and memory-constrained applications
 */

namespace Math {

    // ============================================================================
    // Constructors Implementation
    // ============================================================================

    inline half2::half2() noexcept : x(half::from_bits(0)), y(half::from_bits(0)) {}

    inline half2::half2(half x, half y) noexcept : x(x), y(y) {}

    inline half2::half2(half scalar) noexcept : x(scalar), y(scalar) {}

    inline half2::half2(float x, float y) noexcept : x(x), y(y) {}

    inline half2::half2(float scalar) noexcept : x(scalar), y(scalar) {}

    inline half2::half2(const float2& vec) noexcept : x(vec.x), y(vec.y) {}

    // ============================================================================
    // Assignment Operators Implementation
    // ============================================================================

    inline half2& half2::operator=(const float2& vec) noexcept
    {
        x = vec.x;
        y = vec.y;
        return *this;
    }

    inline half2& half2::operator=(half scalar) noexcept
    {
        x = y = scalar;
        return *this;
    }

    inline half2& half2::operator=(float scalar) noexcept
    {
        x = y = scalar;
        return *this;
    }

    // ============================================================================
    // Compound Assignment Operators Implementation
    // ============================================================================

    inline half2& half2::operator+=(const half2& rhs) noexcept
    {
        x += rhs.x;
        y += rhs.y;
        return *this;
    }

    inline half2& half2::operator-=(const half2& rhs) noexcept
    {
        x -= rhs.x;
        y -= rhs.y;
        return *this;
    }

    inline half2& half2::operator*=(const half2& rhs) noexcept
    {
        x *= rhs.x;
        y *= rhs.y;
        return *this;
    }

    inline half2& half2::operator/=(const half2& rhs) noexcept
    {
        x /= rhs.x;
        y /= rhs.y;
        return *this;
    }

    inline half2& half2::operator*=(half scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    inline half2& half2::operator*=(float scalar) noexcept
    {
        x *= scalar;
        y *= scalar;
        return *this;
    }

    inline half2& half2::operator/=(half scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    inline half2& half2::operator/=(float scalar) noexcept
    {
        x /= scalar;
        y /= scalar;
        return *this;
    }

    // ============================================================================
    // Unary Operators Implementation
    // ============================================================================

    inline half2 half2::operator+() const noexcept
    {
        return *this;
    }

    inline half2 half2::operator-() const noexcept
    {
        return half2(-x, -y);
    }

    // ============================================================================
    // Access Operators Implementation
    // ============================================================================

    inline half& half2::operator[](int index) noexcept
    {
        return (&x)[index];
    }

    inline const half& half2::operator[](int index) const noexcept
    {
        return (&x)[index];
    }

    // ============================================================================
    // Conversion Operators Implementation
    // ============================================================================

    inline half2::operator float2() const noexcept
    {
        return float2(float(x), float(y));
    }

    // ============================================================================
    // Static Constructors Implementation
    // ============================================================================

    inline half2 half2::zero() noexcept
    {
        return half2(half::from_bits(0), half::from_bits(0));
    }

    inline half2 half2::one() noexcept
    {
        return half2(half::from_bits(0x3C00), half::from_bits(0x3C00));
    }

    inline half2 half2::unit_x() noexcept
    {
        return half2(half::from_bits(0x3C00), half::from_bits(0));
    }

    inline half2 half2::unit_y() noexcept
    {
        return half2(half::from_bits(0), half::from_bits(0x3C00));
    }

    inline half2 half2::uv(half u, half v) noexcept
    {
        return half2(u, v);
    }

    // ============================================================================
    // Mathematical Functions Implementation
    // ============================================================================

    inline half half2::length() const noexcept
    {
        return sqrt(length_sq());
    }

    inline half half2::length_sq() const noexcept
    {
        return x * x + y * y;
    }

    inline half2 half2::normalize() const noexcept
    {
        half len = length();
        if (len.is_zero() || !len.is_finite()) {
            return half2::zero();
        }
        return half2(x / len, y / len);
    }

    inline half half2::dot(const half2& other) const noexcept
    {
        return half2::dot(*this, other);
    }

    inline half2 half2::perpendicular() const noexcept
    {
        return half2(-y, x);
    }

    inline half half2::distance(const half2& other) const noexcept
    {
        return (*this - other).length();
    }

    inline half half2::distance_sq(const half2& other) const noexcept
    {
        return (*this - other).length_sq();
    }

    // ============================================================================
    // HLSL-like Functions Implementation
    // ============================================================================

    inline half2 half2::abs() const noexcept
    {
        return half2(Math::abs(x), Math::abs(y));
    }

    inline half2 half2::sign() const noexcept
    {
        return half2(Math::sign(x), Math::sign(y));
    }

    inline half2 half2::floor() const noexcept
    {
        return half2(Math::floor(x), Math::floor(y));
    }

    inline half2 half2::ceil() const noexcept
    {
        return half2(Math::ceil(x), Math::ceil(y));
    }

    inline half2 half2::round() const noexcept
    {
        return half2(Math::round(x), Math::round(y));
    }

    inline half2 half2::frac() const noexcept
    {
        return half2(Math::frac(x), Math::frac(y));
    }

    inline half2 half2::saturate() const noexcept
    {
        return half2::saturate(*this);
    }

    inline half2 half2::step(half edge) const noexcept
    {
        return half2(Math::step(edge, x), Math::step(edge, y));
    }

    // ============================================================================
    // Static Mathematical Functions Implementation
    // ============================================================================

    inline half half2::dot(const half2& a, const half2& b) noexcept
    {
        return a.x * b.x + a.y * b.y;
    }

    inline half2 half2::lerp(const half2& a, const half2& b, half t) noexcept
    {
        return half2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
    }

    inline half2 half2::lerp(const half2& a, const half2& b, float t) noexcept
    {
        return half2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);
    }

    inline half2 half2::saturate(const half2& vec) noexcept
    {
        return half2(
            Math::saturate(vec.x),
            Math::saturate(vec.y)
        );
    }

    inline half2 half2::min(const half2& a, const half2& b) noexcept
    {
        return half2(
            (a.x < b.x) ? a.x : b.x,
            (a.y < b.y) ? a.y : b.y
        );
    }

    inline half2 half2::max(const half2& a, const half2& b) noexcept
    {
        return half2(
            (a.x > b.x) ? a.x : b.x,
            (a.y > b.y) ? a.y : b.y
        );
    }

    // ============================================================================
    // Swizzle Operations Implementation
    // ============================================================================

    inline half2 half2::yx() const noexcept
    {
        return half2(y, x);
    }

    inline half2 half2::xx() const noexcept
    {
        return half2(x, x);
    }

    inline half2 half2::yy() const noexcept
    {
        return half2(y, y);
    }

    // ============================================================================
    // Texture Coordinate Accessors Implementation
    // ============================================================================

    inline half half2::u() const noexcept
    {
        return x;
    }

    inline half half2::v() const noexcept
    {
        return y;
    }

    inline void half2::set_u(half u) noexcept
    {
        x = u;
    }

    inline void half2::set_v(half v) noexcept
    {
        y = v;
    }

    // ============================================================================
    // Utility Methods Implementation
    // ============================================================================

    inline bool half2::is_valid() const noexcept
    {
        return x.is_finite() && y.is_finite();
    }

    inline bool half2::approximately(const half2& other, float epsilon) const noexcept
    {
        return x.approximately(other.x, epsilon) &&
            y.approximately(other.y, epsilon);
    }

    inline bool half2::approximately_zero(float epsilon) const noexcept
    {
        return x.approximately_zero(epsilon) &&
            y.approximately_zero(epsilon);
    }

    inline bool half2::is_normalized(float epsilon) const noexcept
    {
        half len_sq = length_sq();
        float adjusted_epsilon = std::max(epsilon, 0.01f);
        return MathFunctions::approximately(float(len_sq), 1.0f, adjusted_epsilon);
    }

    inline std::string half2::to_string() const
    {
        char buffer[64];
        std::snprintf(buffer, sizeof(buffer), "(%.3f, %.3f)", float(x), float(y));
        return std::string(buffer);
    }

    inline const half* half2::data() const noexcept
    {
        return &x;
    }

    inline half* half2::data() noexcept
    {
        return &x;
    }

    // ============================================================================
    // Comparison Operators Implementation
    // ============================================================================

    inline bool half2::operator==(const half2& rhs) const noexcept
    {
        return approximately(rhs);
    }

    inline bool half2::operator!=(const half2& rhs) const noexcept
    {
        return !(*this == rhs);
    }

    // ============================================================================
    // Binary Operators Implementation
    // ============================================================================

    inline half2 operator+(half2 lhs, const half2& rhs) noexcept
    {
        return lhs += rhs;
    }

    inline half2 operator-(half2 lhs, const half2& rhs) noexcept
    {
        return lhs -= rhs;
    }

    inline half2 operator*(half2 lhs, const half2& rhs) noexcept
    {
        return lhs *= rhs;
    }

    inline half2 operator/(half2 lhs, const half2& rhs) noexcept
    {
        return lhs /= rhs;
    }

    inline half2 operator*(half2 vec, half scalar) noexcept
    {
        return vec *= scalar;
    }

    inline half2 operator*(half scalar, half2 vec) noexcept
    {
        return vec *= scalar;
    }

    inline half2 operator/(half2 vec, half scalar) noexcept
    {
        return vec /= scalar;
    }

    inline half2 operator*(half2 vec, float scalar) noexcept
    {
        return vec *= scalar;
    }

    inline half2 operator*(float scalar, half2 vec) noexcept
    {
        return vec *= scalar;
    }

    inline half2 operator/(half2 vec, float scalar) noexcept
    {
        return vec /= scalar;
    }

    // ============================================================================
    // Mixed Type Operators (half2 <-> float2) Implementation
    // ============================================================================

    inline half2 operator+(const half2& lhs, const float2& rhs) noexcept
    {
        return half2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    inline half2 operator-(const half2& lhs, const float2& rhs) noexcept
    {
        return half2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    inline half2 operator*(const half2& lhs, const float2& rhs) noexcept
    {
        return half2(lhs.x * rhs.x, lhs.y * rhs.y);
    }

    inline half2 operator/(const half2& lhs, const float2& rhs) noexcept
    {
        return half2(lhs.x / rhs.x, lhs.y / rhs.y);
    }

    inline half2 operator+(const float2& lhs, const half2& rhs) noexcept
    {
        return half2(lhs.x + rhs.x, lhs.y + rhs.y);
    }

    inline half2 operator-(const float2& lhs, const half2& rhs) noexcept
    {
        return half2(lhs.x - rhs.x, lhs.y - rhs.y);
    }

    inline half2 operator*(const float2& lhs, const half2& rhs) noexcept
    {
        return half2(lhs.x * rhs.x, lhs.y * rhs.y);
    }

    inline half2 operator/(const float2& lhs, const half2& rhs) noexcept
    {
        return half2(lhs.x / rhs.x, lhs.y / rhs.y);
    }

    // ============================================================================
    // Global Mathematical Functions Implementation
    // ============================================================================

    inline half distance(const half2& a, const half2& b) noexcept
    {
        return (b - a).length();
    }

    inline half distance_sq(const half2& a, const half2& b) noexcept
    {
        return (b - a).length_sq();
    }

    inline half dot(const half2& a, const half2& b) noexcept
    {
        return half2::dot(a, b);
    }

    inline half2 normalize(const half2& vec) noexcept
    {
        return vec.normalize();
    }

    inline half2 lerp(const half2& a, const half2& b, half t) noexcept
    {
        return half2::lerp(a, b, t);
    }

    inline half2 lerp(const half2& a, const half2& b, float t) noexcept
    {
        return half2::lerp(a, b, t);
    }

    inline half2 saturate(const half2& vec) noexcept
    {
        return half2::saturate(vec);
    }

    inline bool approximately(const half2& a, const half2& b, float epsilon) noexcept
    {
        return a.approximately(b, epsilon);
    }

    inline bool is_valid(const half2& vec) noexcept
    {
        return vec.is_valid();
    }

    inline bool is_normalized(const half2& vec, float epsilon) noexcept
    {
        return vec.is_normalized(epsilon);
    }

    inline bool is_inf(const half2& vec) noexcept
    {
        return vec.is_inf();
    }

    inline bool is_negative_inf(const half2& vec) noexcept
    {
        return vec.is_negative_inf();
    }

    inline bool is_positive_inf(const half2& vec) noexcept
    {
        return vec.is_positive_inf();
    }

    inline bool is_negative(const half2& vec) noexcept
    {
        return vec.is_negative();
    }

    inline bool is_all_negative(const half2& vec) noexcept
    {
        return vec.is_all_negative();
    }

    inline bool is_positive(const half2& vec) noexcept
    {
        return vec.is_positive();
    }

    inline bool is_all_positive(const half2& vec) noexcept
    {
        return vec.is_all_positive();
    }

    inline bool is_nan(const half2& vec) noexcept
    {
        return vec.is_nan();
    }

    inline bool is_all_nan(const half2& vec) noexcept
    {
        return vec.is_all_nan();
    }

    inline bool is_finite(const half2& vec) noexcept
    {
        return vec.is_finite();
    }

    inline bool is_all_finite(const half2& vec) noexcept
    {
        return vec.is_all_finite();
    }

    inline bool is_zero(const half2& vec) noexcept
    {
        return vec.is_zero();
    }

    inline bool is_all_zero(const half2& vec) noexcept
    {
        return vec.is_all_zero();
    }

    // ============================================================================
    // HLSL-like Global Functions Implementation
    // ============================================================================

    inline half2 abs(const half2& vec) noexcept
    {
        return vec.abs();
    }

    inline half2 sign(const half2& vec) noexcept
    {
        return vec.sign();
    }

    inline half2 floor(const half2& vec) noexcept
    {
        return vec.floor();
    }

    inline half2 ceil(const half2& vec) noexcept
    {
        return vec.ceil();
    }

    inline half2 round(const half2& vec) noexcept
    {
        return vec.round();
    }

    inline half2 frac(const half2& vec) noexcept
    {
        return vec.frac();
    }

    inline half2 step(half edge, const half2& vec) noexcept
    {
        return vec.step(edge);
    }

    inline half2 min(const half2& a, const half2& b) noexcept
    {
        return half2::min(a, b);
    }

    inline half2 max(const half2& a, const half2& b) noexcept
    {
        return half2::max(a, b);
    }

    inline half2 clamp(const half2& vec, const half2& min_val, const half2& max_val) noexcept
    {
        return half2::min(half2::max(vec, min_val), max_val);
    }

    inline half2 clamp(const half2& vec, float min_val, float max_val) noexcept
    {
        return half2(
            Math::clamp(vec.x, min_val, max_val),
            Math::clamp(vec.y, min_val, max_val)
        );
    }

    inline half2 smoothstep(half edge0, half edge1, const half2& vec) noexcept
    {
        return half2(
            Math::smoothstep(edge0, edge1, vec.x),
            Math::smoothstep(edge0, edge1, vec.y)
        );
    }

    // ============================================================================
    // Geometric Operations Implementation
    // ============================================================================

    inline half2 perpendicular(const half2& vec) noexcept
    {
        return vec.perpendicular();
    }

    inline half cross(const half2& a, const half2& b) noexcept
    {
        return a.x * b.y - a.y * b.x;
    }

    inline half angle(const half2& vec) noexcept
    {
        return half(std::atan2(float(vec.y), float(vec.x)));
    }

    inline half angle_between(const half2& a, const half2& b) noexcept
    {
        half2 a_norm = a.normalize();
        half2 b_norm = b.normalize();
        half dot_val = dot(a_norm, b_norm);
        dot_val = Math::clamp(dot_val, -half_One, half_One);
        return half(std::acos(float(dot_val)));
    }

    // ============================================================================
    // Type Conversion Functions Implementation
    // ============================================================================

    inline float2 to_float2(const half2& vec) noexcept
    {
        return float2(float(vec.x), float(vec.y));
    }

    inline half2 to_half2(const float2& vec) noexcept
    {
        return half2(vec.x, vec.y);
    }

    // ============================================================================
    // Global Constants Implementation
    // ============================================================================

    inline const half2 half2_Zero(half_Zero);
    inline const half2 half2_One(half_One);
    inline const half2 half2_UnitX(half_One, half_Zero);
    inline const half2 half2_UnitY(half_Zero, half_One);
    inline const half2 half2_UV_Zero(half_Zero, half_Zero);
    inline const half2 half2_UV_One(half_One, half_One);
    inline const half2 half2_UV_Half(half(0.5f), half(0.5f));
    inline const half2 half2_Right(half_One, half_Zero);
    inline const half2 half2_Left(-half_One, half_Zero);
    inline const half2 half2_Up(half_Zero, half_One);
    inline const half2 half2_Down(half_Zero, -half_One);

} // namespace Math
