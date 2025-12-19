// Description: 4-dimensional vector class with comprehensive 
//              mathematical operations and SSE optimization
//              Supports both 4D vectors and homogeneous coordinates
// Author: NSDeathman, DeepSeek

#pragma once

namespace Math
{
    // ============================================================================
    // Constructors Implementation
    // ============================================================================

    inline float4::float4() noexcept : simd_(_mm_setzero_ps()) {}

    inline float4::float4(float x, float y, float z, float w) noexcept : simd_(_mm_set_ps(w, z, y, x)) {}

    inline float4::float4(float scalar) noexcept : simd_(_mm_set1_ps(scalar)) {}

    inline float4::float4(const float2& vec, float z, float w) noexcept
        : simd_(_mm_set_ps(w, z, vec.y, vec.x)) {}

    inline float4::float4(const float3& vec, float w) noexcept
        : simd_(_mm_set_ps(w, vec.z, vec.y, vec.x)) {}

    inline float4::float4(const float* data) noexcept : simd_(_mm_loadu_ps(data)) {}

    inline float4::float4(__m128 simd_val) noexcept : simd_(simd_val) {}

#if defined(MATH_SUPPORT_D3DX)
    inline float4::float4(const D3DXVECTOR4& vec) noexcept : simd_(_mm_loadu_ps(&vec.x)) {}

    inline float4::float4(const D3DXVECTOR3& vec, float w) noexcept
        : simd_(_mm_set_ps(w, vec.z, vec.y, vec.x)) {}

    inline float4::float4(const D3DXVECTOR2& vec, float z, float w) noexcept
        : simd_(_mm_set_ps(w, z, vec.y, vec.x)) {}

    inline float4::float4(D3DCOLOR color) noexcept {
        float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
        float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
        float b = static_cast<float>(color & 0xFF) / 255.0f;
        float a = static_cast<float>((color >> 24) & 0xFF) / 255.0f;
        simd_ = _mm_set_ps(a, b, g, r);
    }
#endif

    // ============================================================================
    // Assignment Operators Implementation
    // ============================================================================

    inline float4& float4::operator=(float scalar) noexcept {
        simd_ = _mm_set1_ps(scalar);
        return *this;
    }

    inline float4& float4::operator=(const float3& xyz) noexcept {
        // Сохраняем w компонент, обновляем xyz
        __m128 xyz_vec = _mm_set_ps(0.0f, xyz.z, xyz.y, xyz.x);
        simd_ = _mm_blend_ps(xyz_vec, simd_, 0x8); // сохраняем w из текущего значения
        return *this;
    }

#if defined(MATH_SUPPORT_D3DX)
    inline float4& float4::operator=(const D3DXVECTOR4& vec) noexcept {
        simd_ = _mm_loadu_ps(&vec.x);
        return *this;
    }

    inline float4& float4::operator=(D3DCOLOR color) noexcept {
        float r = static_cast<float>((color >> 16) & 0xFF) / 255.0f;
        float g = static_cast<float>((color >> 8) & 0xFF) / 255.0f;
        float b = static_cast<float>(color & 0xFF) / 255.0f;
        float a = static_cast<float>((color >> 24) & 0xFF) / 255.0f;
        simd_ = _mm_set_ps(a, b, g, r);
        return *this;
    }
#endif

    // ============================================================================
    // Compound Assignment Operators Implementation
    // ============================================================================

    inline float4& float4::operator+=(const float4& rhs) noexcept {
        simd_ = _mm_add_ps(simd_, rhs.simd_);
        return *this;
    }

    inline float4& float4::operator-=(const float4& rhs) noexcept {
        simd_ = _mm_sub_ps(simd_, rhs.simd_);
        return *this;
    }

    inline float4& float4::operator*=(const float4& rhs) noexcept {
        simd_ = _mm_mul_ps(simd_, rhs.simd_);
        return *this;
    }

    inline float4& float4::operator/=(const float4& rhs) noexcept {
        simd_ = _mm_div_ps(simd_, rhs.simd_);
        return *this;
    }

    inline float4& float4::operator*=(float scalar) noexcept {
        __m128 scalar_vec = _mm_set1_ps(scalar);
        simd_ = _mm_mul_ps(simd_, scalar_vec);
        return *this;
    }

    inline float4& float4::operator/=(float scalar) noexcept {
        __m128 inv_scalar = _mm_set1_ps(1.0f / scalar);
        simd_ = _mm_mul_ps(simd_, inv_scalar);
        return *this;
    }

    // ============================================================================
    // Unary Operators Implementation
    // ============================================================================

    inline float4 float4::operator+() const noexcept {
        return *this;
    }

    inline float4 float4::operator-() const noexcept {
        __m128 neg = _mm_set1_ps(-1.0f);
        return float4(_mm_mul_ps(simd_, neg));
    }

    // ============================================================================
    // Access Operators Implementation
    // ============================================================================

    inline float& float4::operator[](int index) noexcept {
        return (&x)[index];
    }

    inline const float& float4::operator[](int index) const noexcept {
        return (&x)[index];
    }

    // ============================================================================
    // Conversion Operators Implementation
    // ============================================================================

    inline float4::operator const float* () const noexcept { return &x; }

    inline float4::operator float* () noexcept { return &x; }

    inline float4::operator __m128() const noexcept { return simd_; }

#if defined(MATH_SUPPORT_D3DX)
    inline float4::operator D3DXVECTOR4() const noexcept {
        D3DXVECTOR4 result;
        _mm_storeu_ps(&result.x, simd_);
        return result;
    }
#endif

    // ============================================================================
    // Static Constructors Implementation
    // ============================================================================

    inline float4 float4::zero() noexcept {
        return float4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    inline float4 float4::one() noexcept {
        return float4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    inline float4 float4::unit_x() noexcept {
        return float4(1.0f, 0.0f, 0.0f, 0.0f);
    }

    inline float4 float4::unit_y() noexcept {
        return float4(0.0f, 1.0f, 0.0f, 0.0f);
    }

    inline float4 float4::unit_z() noexcept {
        return float4(0.0f, 0.0f, 1.0f, 0.0f);
    }

    inline float4 float4::unit_w() noexcept {
        return float4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    inline float4 float4::from_rgba(uint8_t r, uint8_t g, uint8_t b, uint8_t a) noexcept {
        return float4(
            static_cast<float>(r) / 255.0f,
            static_cast<float>(g) / 255.0f,
            static_cast<float>(b) / 255.0f,
            static_cast<float>(a) / 255.0f
        );
    }

    inline float4 float4::from_color(float r, float g, float b, float a) noexcept {
        return float4(r, g, b, a);
    }

    // ============================================================================
    // Mathematical Functions Implementation
    // ============================================================================

    inline float float4::length() const noexcept {
        __m128 squared = _mm_mul_ps(simd_, simd_);
        __m128 sum = _mm_hadd_ps(squared, squared);
        sum = _mm_hadd_ps(sum, sum);
        return _mm_cvtss_f32(_mm_sqrt_ss(sum));
    }

    inline float float4::length_sq() const noexcept {
        __m128 squared = _mm_mul_ps(simd_, simd_);
        __m128 sum = _mm_hadd_ps(squared, squared);
        sum = _mm_hadd_ps(sum, sum);
        return _mm_cvtss_f32(sum);
    }

    inline float4 float4::normalize() const noexcept {
        __m128 squared = _mm_mul_ps(simd_, simd_);
        __m128 sum = _mm_hadd_ps(squared, squared);
        sum = _mm_hadd_ps(sum, sum);

        __m128 len_vec = _mm_sqrt_ps(sum);
        __m128 mask = _mm_cmpgt_ps(len_vec, _mm_set1_ps(Constants::Constants<float>::Epsilon));
        __m128 inv_len = _mm_div_ps(_mm_set1_ps(1.0f), len_vec);
        inv_len = _mm_and_ps(inv_len, mask); // если длина 0, то inv_len = 0

        return float4(_mm_mul_ps(simd_, inv_len));
    }

    inline float float4::dot(const float4& other) const noexcept {
        return float4::dot(*this, other);
    }

    inline float float4::dot3(const float4& other) const noexcept {
        return float4::dot3(*this, other);
    }

    inline float4 float4::cross(const float4& other) const noexcept {
        return float4::cross(*this, other);
    }

    inline float float4::distance(const float4& other) const noexcept {
        __m128 diff = _mm_sub_ps(simd_, other.simd_);
        __m128 squared = _mm_mul_ps(diff, diff);
        __m128 sum = _mm_hadd_ps(squared, squared);
        sum = _mm_hadd_ps(sum, sum);
        return _mm_cvtss_f32(_mm_sqrt_ss(sum));
    }

    inline float float4::distance_sq(const float4& other) const noexcept {
        return (*this - other).length_sq();
    }

    // ============================================================================
    // HLSL-like Functions Implementation
    // ============================================================================

    inline float4 float4::abs() const noexcept {
        __m128 mask = _mm_set1_ps(-0.0f); // -0.0f = 0x80000000
        return float4(_mm_andnot_ps(mask, simd_));
    }

    inline float4 float4::sign() const noexcept {
        __m128 zero = _mm_setzero_ps();
        __m128 one = _mm_set1_ps(1.0f);
        __m128 neg_one = _mm_set1_ps(-1.0f);

        __m128 gt_zero = _mm_cmpgt_ps(simd_, zero);
        __m128 lt_zero = _mm_cmplt_ps(simd_, zero);

        __m128 result = _mm_and_ps(gt_zero, one);
        result = _mm_or_ps(result, _mm_and_ps(lt_zero, neg_one));

        return float4(result);
    }

    inline float4 float4::floor() const noexcept {
#ifdef __SSE4_1__
        return float4(_mm_floor_ps(simd_));
#else
        alignas(16) float temp[4];
        _mm_store_ps(temp, simd_);
        return float4(std::floor(temp[0]), std::floor(temp[1]), std::floor(temp[2]), std::floor(temp[3]));
#endif
    }

    inline float4 float4::ceil() const noexcept {
#ifdef __SSE4_1__
        return float4(_mm_ceil_ps(simd_));
#else
        alignas(16) float temp[4];
        _mm_store_ps(temp, simd_);
        return float4(std::ceil(temp[0]), std::ceil(temp[1]), std::ceil(temp[2]), std::ceil(temp[3]));
#endif
    }

    inline float4 float4::round() const noexcept {
#ifdef __SSE4_1__
        return float4(_mm_round_ps(simd_, _MM_FROUND_TO_NEAREST_INT | _MM_FROUND_NO_EXC));
#else
        alignas(16) float temp[4];
        _mm_store_ps(temp, simd_);
        return float4(std::round(temp[0]), std::round(temp[1]), std::round(temp[2]), std::round(temp[3]));
#endif
    }

    inline float4 float4::frac() const noexcept {
        alignas(16) float temp[4];
        _mm_store_ps(temp, simd_);
        return float4(
            temp[0] - std::floor(temp[0]),
            temp[1] - std::floor(temp[1]),
            temp[2] - std::floor(temp[2]),
            temp[3] - std::floor(temp[3])
        );
    }

    inline float4 float4::saturate() const noexcept {
        return float4::saturate(*this);
    }

    inline float4 float4::step(float edge) const noexcept {
        __m128 edge_vec = _mm_set1_ps(edge);
        __m128 one = _mm_set1_ps(1.0f);
        __m128 zero = _mm_setzero_ps();

        __m128 cmp = _mm_cmpge_ps(simd_, edge_vec);
        return float4(_mm_or_ps(_mm_and_ps(cmp, one), _mm_andnot_ps(cmp, zero)));
    }

    // ============================================================================
    // Color Operations Implementation
    // ============================================================================

    inline float float4::luminance() const noexcept {
        __m128 weights = _mm_set_ps(0.0f, 0.0722f, 0.7152f, 0.2126f);
        __m128 mul = _mm_mul_ps(simd_, weights);
        __m128 sum = _mm_hadd_ps(mul, mul);
        sum = _mm_hadd_ps(sum, sum);
        return _mm_cvtss_f32(sum);
    }

    inline float float4::brightness() const noexcept {
        __m128 sum = _mm_hadd_ps(simd_, simd_);
        sum = _mm_hadd_ps(sum, sum);
        return _mm_cvtss_f32(_mm_mul_ss(sum, _mm_set1_ps(1.0f / 3.0f)));
    }

    inline float4 float4::premultiply_alpha() const noexcept {
        __m128 alpha = _mm_shuffle_ps(simd_, simd_, _MM_SHUFFLE(3, 3, 3, 3));
        __m128 result = _mm_mul_ps(simd_, alpha);
        result = _mm_blend_ps(result, simd_, 0x8);
        return float4(result);
    }

    inline float4 float4::unpremultiply_alpha() const noexcept {
        // Проверяем alpha на 0
        __m128 alpha = _mm_shuffle_ps(simd_, simd_, _MM_SHUFFLE(3, 3, 3, 3));
        __m128 zero = _mm_setzero_ps();
        __m128 alpha_is_zero = _mm_cmpeq_ps(alpha, zero);

        // Если alpha = 0, возвращаем оригинальный вектор
        if (_mm_movemask_ps(alpha_is_zero) != 0) {
            return *this;
        }

        // Иначе делим RGB на alpha
        __m128 inv_alpha = _mm_div_ps(_mm_set1_ps(1.0f), alpha);

        // Умножаем все компоненты на inverse alpha
        __m128 result = _mm_mul_ps(simd_, inv_alpha);

        // Но восстанавливаем оригинальный alpha (не делить alpha на саму себя)
        result = _mm_blend_ps(result, simd_, 0x8); // маска 1000 - сохраняем оригинальный alpha

        return float4(result);
    }

    inline float4 float4::grayscale() const noexcept {
        float lum = luminance();
        return float4(lum, lum, lum, w);
    }

    // ============================================================================
    // Geometric Operations Implementation
    // ============================================================================

    inline float3 float4::project() const noexcept {
        __m128 w_vec = _mm_shuffle_ps(simd_, simd_, _MM_SHUFFLE(3, 3, 3, 3));
        __m128 mask = _mm_cmpneq_ps(w_vec, _mm_setzero_ps());
        __m128 inv_w = _mm_div_ps(_mm_set1_ps(1.0f), w_vec);
        inv_w = _mm_and_ps(inv_w, mask);

        __m128 projected = _mm_mul_ps(simd_, inv_w);
        return float3(_mm_cvtss_f32(projected),
            _mm_cvtss_f32(_mm_shuffle_ps(projected, projected, _MM_SHUFFLE(1, 1, 1, 1))),
            _mm_cvtss_f32(_mm_shuffle_ps(projected, projected, _MM_SHUFFLE(2, 2, 2, 2))));
    }

    inline float4 float4::to_homogeneous() const noexcept {
        return float4(x, y, z, 1.0f);
    }

    // ============================================================================
    // Static Mathematical Functions Implementation
    // ============================================================================

    inline float float4::dot(const float4& a, const float4& b) noexcept {
        __m128 mul = _mm_mul_ps(a.simd_, b.simd_);
        __m128 sum = _mm_hadd_ps(mul, mul);
        sum = _mm_hadd_ps(sum, sum);
        return _mm_cvtss_f32(sum);
    }

    inline float float4::dot3(const float4& a, const float4& b) noexcept {
        // Простая и надежная реализация без сложных масок
        __m128 a3 = a.simd_;
        __m128 b3 = b.simd_;

        // Умножаем все компоненты
        __m128 mul = _mm_mul_ps(a3, b3);

        // Суммируем только x, y, z (игнорируем w)
        // Используем горизонтальное сложение
        __m128 sum_xy = _mm_hadd_ps(mul, mul);        // x+y, z+w, x+y, z+w
        __m128 sum_xyz = _mm_hadd_ps(sum_xy, sum_xy); // x+y+z+w, x+y+z+w, x+y+z+w, x+y+z+w

        // Теперь вычитаем w компонент чтобы получить только x+y+z
        __m128 w_component = _mm_shuffle_ps(mul, mul, _MM_SHUFFLE(3, 3, 3, 3)); // w, w, w, w
        __m128 result = _mm_sub_ps(sum_xyz, w_component);

        return _mm_cvtss_f32(result);
    }

    inline float4 float4::cross(const float4& a, const float4& b) noexcept {
        __m128 a_vec = a.simd_;
        __m128 b_vec = b.simd_;

        // a.y * b.z - a.z * b.y
        // a.z * b.x - a.x * b.z  
        // a.x * b.y - a.y * b.x
        // w = 0

        __m128 a_y = _mm_shuffle_ps(a_vec, a_vec, _MM_SHUFFLE(3, 3, 0, 1)); // a.y, a.y, a.y, a.y
        __m128 a_z = _mm_shuffle_ps(a_vec, a_vec, _MM_SHUFFLE(3, 3, 1, 2)); // a.z, a.z, a.z, a.z  
        __m128 a_x = _mm_shuffle_ps(a_vec, a_vec, _MM_SHUFFLE(3, 3, 2, 0)); // a.x, a.x, a.x, a.x

        __m128 b_y = _mm_shuffle_ps(b_vec, b_vec, _MM_SHUFFLE(3, 3, 0, 1)); // b.y, b.y, b.y, b.y
        __m128 b_z = _mm_shuffle_ps(b_vec, b_vec, _MM_SHUFFLE(3, 3, 1, 2)); // b.z, b.z, b.z, b.z
        __m128 b_x = _mm_shuffle_ps(b_vec, b_vec, _MM_SHUFFLE(3, 3, 2, 0)); // b.x, b.x, b.x, b.x

        __m128 result_x = _mm_sub_ps(_mm_mul_ps(a_y, b_z), _mm_mul_ps(a_z, b_y));
        __m128 result_y = _mm_sub_ps(_mm_mul_ps(a_z, b_x), _mm_mul_ps(a_x, b_z));
        __m128 result_z = _mm_sub_ps(_mm_mul_ps(a_x, b_y), _mm_mul_ps(a_y, b_x));

        __m128 result = _mm_set_ps(0.0f,
            _mm_cvtss_f32(result_z),
            _mm_cvtss_f32(result_y),
            _mm_cvtss_f32(result_x));

        return float4(result);
    }

    inline float4 float4::lerp(const float4& a, const float4& b, float t) noexcept {
        __m128 t_vec = _mm_set1_ps(t);
        __m128 one_minus_t = _mm_set1_ps(1.0f - t);

        __m128 part1 = _mm_mul_ps(a.simd_, one_minus_t);
        __m128 part2 = _mm_mul_ps(b.simd_, t_vec);

        return float4(_mm_add_ps(part1, part2));
    }

    inline float4 float4::min(const float4& a, const float4& b) noexcept {
        return float4(_mm_min_ps(a.simd_, b.simd_));
    }

    inline float4 float4::max(const float4& a, const float4& b) noexcept {
        return float4(_mm_max_ps(a.simd_, b.simd_));
    }

    inline float4 float4::saturate(const float4& vec) noexcept {
        __m128 zero = _mm_setzero_ps();
        __m128 one = _mm_set1_ps(1.0f);
        __m128 result = _mm_max_ps(vec.simd_, zero);
        result = _mm_min_ps(result, one);
        return float4(result);
    }

    // ============================================================================
    // Swizzle Operations Implementation
    // ============================================================================

    inline float2 float4::xy() const noexcept { return float2(x, y); }
    inline float2 float4::xz() const noexcept { return float2(x, z); }
    inline float2 float4::xw() const noexcept { return float2(x, w); }
    inline float2 float4::yz() const noexcept { return float2(y, z); }
    inline float2 float4::yw() const noexcept { return float2(y, w); }
    inline float2 float4::zw() const noexcept { return float2(z, w); }

    inline float3 float4::xyz() const noexcept { return float3(x, y, z); }
    inline float3 float4::xyw() const noexcept { return float3(x, y, w); }
    inline float3 float4::xzw() const noexcept { return float3(x, z, w); }
    inline float3 float4::yzw() const noexcept { return float3(y, z, w); }

    inline float4 float4::yxzw() const noexcept { return float4(y, x, z, w); }
    inline float4 float4::zxyw() const noexcept { return float4(z, x, y, w); }
    inline float4 float4::zyxw() const noexcept { return float4(z, y, x, w); }
    inline float4 float4::wzyx() const noexcept { return float4(w, z, y, x); }

    // Color swizzles
    inline float float4::r() const noexcept { return x; }
    inline float float4::g() const noexcept { return y; }
    inline float float4::b() const noexcept { return z; }
    inline float float4::a() const noexcept { return w; }
    inline float2 float4::rg() const noexcept { return float2(x, y); }
    inline float2 float4::rb() const noexcept { return float2(x, z); }
    inline float2 float4::ra() const noexcept { return float2(x, w); }
    inline float2 float4::gb() const noexcept { return float2(y, z); }
    inline float2 float4::ga() const noexcept { return float2(y, w); }
    inline float2 float4::ba() const noexcept { return float2(z, w); }

    inline float3 float4::rgb() const noexcept { return float3(x, y, z); }
    inline float3 float4::rga() const noexcept { return float3(x, y, w); }
    inline float3 float4::rba() const noexcept { return float3(x, z, w); }
    inline float3 float4::gba() const noexcept { return float3(y, z, w); }

    inline float4 float4::grba() const noexcept { return float4(y, x, z, w); }
    inline float4 float4::brga() const noexcept { return float4(z, x, y, w); }
    inline float4 float4::bgra() const noexcept { return float4(z, y, x, w); }
    inline float4 float4::abgr() const noexcept { return float4(w, z, y, x); }

    // ============================================================================
    // Utility Methods Implementation
    // ============================================================================

    inline bool float4::isValid() const noexcept {
        __m128 zero = _mm_setzero_ps();
        __m128 inf = _mm_set1_ps(std::numeric_limits<float>::infinity());

        __m128 is_nan = _mm_cmpunord_ps(simd_, simd_);
        __m128 is_inf = _mm_cmpeq_ps(_mm_andnot_ps(_mm_set1_ps(-0.0f), simd_), inf);

        return (_mm_movemask_ps(_mm_or_ps(is_nan, is_inf)) == 0);
    }

    inline bool float4::approximately(const float4& other, float epsilon) const noexcept {
        __m128 diff = _mm_sub_ps(simd_, other.simd_);
        __m128 abs_diff = _mm_andnot_ps(_mm_set1_ps(-0.0f), diff);
        __m128 epsilon_vec = _mm_set1_ps(epsilon);
        __m128 cmp = _mm_cmple_ps(abs_diff, epsilon_vec);
        return (_mm_movemask_ps(cmp) & 0xF) == 0xF;
    }

    inline bool float4::approximately_zero(float epsilon) const noexcept {
        return length_sq() <= epsilon * epsilon;
    }

    inline bool float4::is_normalized(float epsilon) const noexcept {
        float len_sq = length_sq();
        return std::isfinite(len_sq) && MathFunctions::approximately(len_sq, 1.0f, epsilon);
    }

    inline std::string float4::to_string() const {
        char buffer[64];
        std::snprintf(buffer, sizeof(buffer), "(%.3f, %.3f, %.3f, %.3f)", x, y, z, w);
        return std::string(buffer);
    }

    inline const float* float4::data() const noexcept { return &x; }

    inline float* float4::data() noexcept { return &x; }

    inline void float4::set_xyz(const float3& xyz) noexcept {
        __m128 xyz_vec = _mm_set_ps(0.0f, xyz.z, xyz.y, xyz.x);
        simd_ = _mm_blend_ps(simd_, xyz_vec, 0x7); // заменяем xyz, сохраняем w
    }

    inline void float4::set_xy(const float2& xy) noexcept {
        __m128 xy_vec = _mm_set_ps(0.0f, 0.0f, xy.y, xy.x);
        simd_ = _mm_blend_ps(simd_, xy_vec, 0x3); // заменяем xy, сохраняем zw
    }

    inline void float4::set_zw(const float2& zw) noexcept {
        __m128 zw_vec = _mm_set_ps(zw.y, zw.x, 0.0f, 0.0f);  // w=zw.y, z=zw.x, y=0, x=0

        // Маска 0xC (1100) заменяет компоненты w и z
        simd_ = _mm_blend_ps(simd_, zw_vec, 0xC); // заменяем z и w, сохраняем x и y
    }

    // ============================================================================
    // SSE-specific Methods Implementation
    // ============================================================================

    inline __m128 float4::get_simd() const noexcept { return simd_; }

    inline void float4::set_simd(__m128 new_simd) noexcept { simd_ = new_simd; }

    inline float4 float4::load_unaligned(const float* data) noexcept {
        return float4(_mm_loadu_ps(data));
    }

    inline float4 float4::load_aligned(const float* data) noexcept {
        return float4(_mm_load_ps(data));
    }

    inline void float4::store_unaligned(float* data) const noexcept {
        _mm_storeu_ps(data, simd_);
    }

    inline void float4::store_aligned(float* data) const noexcept {
        _mm_store_ps(data, simd_);
    }

    // ============================================================================
    // Comparison Operators Implementation
    // ============================================================================

    inline bool float4::operator==(const float4& rhs) const noexcept {
        return approximately(rhs);
    }

    inline bool float4::operator!=(const float4& rhs) const noexcept {
        return !(*this == rhs);
    }

    // ============================================================================
    // Binary Operators Implementation
    // ============================================================================

    inline float4 operator+(float4 lhs, const float4& rhs) noexcept { return lhs += rhs; }

    inline float4 operator-(float4 lhs, const float4& rhs) noexcept { return lhs -= rhs; }

    inline float4 operator*(float4 lhs, const float4& rhs) noexcept { return lhs *= rhs; }

    inline float4 operator/(float4 lhs, const float4& rhs) noexcept { return lhs /= rhs; }

    inline float4 operator*(float4 vec, float scalar) noexcept { return vec *= scalar; }

    inline float4 operator*(float scalar, float4 vec) noexcept { return vec *= scalar; }

    inline float4 operator/(float4 vec, float scalar) noexcept { return vec /= scalar; }

    // ============================================================================
    // Global Mathematical Functions Implementation
    // ============================================================================

    inline float distance(const float4& a, const float4& b) noexcept {
        return (b - a).length();
    }

    inline float distance_sq(const float4& a, const float4& b) noexcept {
        return (b - a).length_sq();
    }

    inline float dot(const float4& a, const float4& b) noexcept {
        return float4::dot(a, b);
    }

    inline float dot3(const float4& a, const float4& b) noexcept {
        return float4::dot3(a, b);
    }

    inline float4 cross(const float4& a, const float4& b) noexcept {
        return float4::cross(a, b);
    }

    inline float4 normalize(const float4& vec) noexcept {
        return vec.normalize();
    }

    inline float4 lerp(const float4& a, const float4& b, float t) noexcept {
        return float4::lerp(a, b, t);
    }

    inline float4 saturate(const float4& vec) noexcept {
        return float4::saturate(vec);
    }

    inline float4 floor(const float4& vec) noexcept {
        return vec.floor();
    }

    inline float4 ceil(const float4& vec) noexcept {
        return vec.ceil();
    }

    inline float4 round(const float4& vec) noexcept {
        return vec.round();
    }

    inline bool approximately(const float4& a, const float4& b, float epsilon) noexcept {
        return a.approximately(b, epsilon);
    }

    inline bool is_normalized(const float4& vec, float epsilon) noexcept {
        return vec.is_normalized(epsilon);
    }

    inline bool isValid(const float4& vec) noexcept {
        return vec.isValid();
    }

    // ============================================================================
    // HLSL-like Global Functions Implementation
    // ============================================================================

    inline float4 abs(const float4& vec) noexcept {
        return vec.abs();
    }

    inline float4 sign(const float4& vec) noexcept {
        return vec.sign();
    }

    inline float4 frac(const float4& vec) noexcept {
        return vec.frac();
    }

    inline float4 step(float edge, const float4& vec) noexcept {
        return vec.step(edge);
    }

    inline float4 min(const float4& a, const float4& b) noexcept {
        return float4::min(a, b);
    }

    inline float4 max(const float4& a, const float4& b) noexcept {
        return float4::max(a, b);
    }

    inline float4 clamp(const float4& vec, const float4& min_val, const float4& max_val) noexcept {
        return float4::min(float4::max(vec, min_val), max_val);
    }

    // ============================================================================
    // Color Operations Implementation
    // ============================================================================

    inline float luminance(const float4& color) noexcept {
        return color.luminance();
    }

    inline float brightness(const float4& color) noexcept {
        return color.brightness();
    }

    inline float4 premultiply_alpha(const float4& color) noexcept {
        return color.premultiply_alpha();
    }

    inline float4 unpremultiply_alpha(const float4& color) noexcept {
        return color.unpremultiply_alpha();
    }

    inline float4 grayscale(const float4& color) noexcept {
        return color.grayscale();
    }

    // ============================================================================
    // Geometric Operations Implementation
    // ============================================================================

    inline float3 project(const float4& vec) noexcept {
        return vec.project();
    }

    inline float4 to_homogeneous(const float4& vec) noexcept {
        return vec.to_homogeneous();
    }

    // ============================================================================
    // D3D Compatibility Functions Implementation
    // ============================================================================

#if defined(MATH_SUPPORT_D3DX)

    inline D3DXVECTOR4 ToD3DXVECTOR4(const float4& vec) noexcept {
        return D3DXVECTOR4(vec.x, vec.y, vec.z, vec.w);
    }

    inline float4 FromD3DXVECTOR4(const D3DXVECTOR4& vec) noexcept {
        return float4(vec.x, vec.y, vec.z, vec.w);
    }

    inline D3DCOLOR ToD3DCOLOR(const float4& color) noexcept {
        return D3DCOLOR_COLORVALUE(color.x, color.y, color.z, color.w);
    }

    inline void float4ArrayToD3D(const float4* source, D3DXVECTOR4* destination, size_t count) noexcept {
        for (size_t i = 0; i < count; ++i)
            destination[i] = ToD3DXVECTOR4(source[i]);
    }

    inline void D3DArrayToFloat4(const D3DXVECTOR4* source, float4* destination, size_t count) noexcept {
        for (size_t i = 0; i < count; ++i)
            destination[i] = FromD3DXVECTOR4(source[i]);
    }

#endif

    // ============================================================================
    // Global Constants Implementation
    // ============================================================================

    inline const float4 float4_Zero(0.0f, 0.0f, 0.0f, 0.0f);
    inline const float4 float4_One(1.0f, 1.0f, 1.0f, 1.0f);
    inline const float4 float4_UnitX(1.0f, 0.0f, 0.0f, 0.0f);
    inline const float4 float4_UnitY(0.0f, 1.0f, 0.0f, 0.0f);
    inline const float4 float4_UnitZ(0.0f, 0.0f, 1.0f, 0.0f);
    inline const float4 float4_UnitW(0.0f, 0.0f, 0.0f, 1.0f);

    // Color constants
    inline const float4 float4_Red(1.0f, 0.0f, 0.0f, 1.0f);
    inline const float4 float4_Green(0.0f, 1.0f, 0.0f, 1.0f);
    inline const float4 float4_Blue(0.0f, 0.0f, 1.0f, 1.0f);
    inline const float4 float4_White(1.0f, 1.0f, 1.0f, 1.0f);
    inline const float4 float4_Black(0.0f, 0.0f, 0.0f, 1.0f);
    inline const float4 float4_Transparent(0.0f, 0.0f, 0.0f, 0.0f);
    inline const float4 float4_Yellow(1.0f, 1.0f, 0.0f, 1.0f);
    inline const float4 float4_Cyan(0.0f, 1.0f, 1.0f, 1.0f);
    inline const float4 float4_Magenta(1.0f, 0.0f, 1.0f, 1.0f);

} // namespace Math
