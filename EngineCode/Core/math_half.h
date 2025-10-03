///////////////////////////////////////////////////////////////
// Created: 26.09.2025
// Author: Nikolay Partas, DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <cstdint>
#include <cmath>
#include <type_traits>
#include <string>
///////////////////////////////////////////////////////////////
#include "math_constants.h"
#include "math_functions.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
    class half
    {
    public:
        using storage_type = std::uint16_t;

        // ============================================================================
        // Конструкторы
        // ============================================================================
        half() : data(0) {}
        
        half(float x) : data(float_to_half(x)) {}
        
        explicit half(storage_type bits) : data(bits) {}
        
        half(const half&) = default;

        // ============================================================================
        // Операторы присваивания
        // ============================================================================
        half& operator=(const half&) = default;
        
        half& operator=(float x)
        {
            data = float_to_half(x);
            return *this;
        }

        // ============================================================================
        // Конвертация в float
        // ============================================================================
        operator float() const
        {
            union {
                std::uint32_t s;
                float result;
            };

            s = (data & 0x8000) << 16;              // знак
            std::uint32_t e = (data >> 10) & 0x1F;  // экспонента
            std::uint32_t m = data & 0x03FF;        // мантисса

            if (e == 0)
            {
                // Денормализованные числа или ноль
                if (m == 0) return result;  // +/- 0

                // Денормализованное число - нормализуем
                while ((m & 0x0400) == 0)
                {
                    m <<= 1;
                    e--;
                }
                e++;
                m &= ~0x0400;
            }
            else if (e == 31)
            {
                // Бесконечность или NaN
                s |= 0x7F800000 | (m << 13);
                return result;
            }

            // Нормализованное число
            e += 112;  // смещение экспоненты: half(-15+127) -> float(112)
            s |= (e << 23) | (m << 13);

            return result;
        }

        // ============================================================================
        // Арифметические операторы
        // ============================================================================
        half operator+(half other) const
        {
            return half(float(*this) + float(other));
        }
        
        half operator-(half other) const
        {
            return half(float(*this) - float(other));
        }
        
        half operator*(half other) const
        {
            return half(float(*this) * float(other));
        }
        
        half operator/(half other) const
        {
            return half(float(*this) / float(other));
        }

        // ============================================================================
        // Арифметические операторы с присваиванием
        // ============================================================================
        half& operator+=(half other)
        {
            *this = *this + other;
            return *this;
        }
        
        half& operator-=(half other)
        {
            *this = *this - other;
            return *this;
        }
        
        half& operator*=(half other)
        {
            *this = *this * other;
            return *this;
        }
        
        half& operator/=(half other)
        {
            *this = *this / other;
            return *this;
        }

        // ============================================================================
        // Унарные операторы
        // ============================================================================
        half operator+() const
        {
            return *this;
        }
        
        half operator-() const
        {
            return half(-float(*this));
        }

        // ============================================================================
        // Операторы сравнения
        // ============================================================================
        bool operator==(half other) const
        {
            // Особые случаи: +0 и -0 считаются равными
            if ((data & 0x7FFF) == 0 && (other.data & 0x7FFF) == 0)
                return true;
            return data == other.data;
        }
        
        bool operator!=(half other) const
        {
            return !(*this == other);
        }
        
        bool operator<(half other) const
        {
            return float(*this) < float(other);
        }
        
        bool operator>(half other) const
        {
            return float(*this) > float(other);
        }
        
        bool operator<=(half other) const
        {
            return float(*this) <= float(other);
        }
        
        bool operator>=(half other) const
        {
            return float(*this) >= float(other);
        }

        // ============================================================================
        // Проверки специальных значений
        // ============================================================================
        bool is_zero() const
        {
            return (data & 0x7FFF) == 0;
        }
        
        bool isNan() const
        {
            return ((data & 0x7C00) == 0x7C00) && ((data & 0x03FF) != 0);
        }
        
        bool isInf() const
        {
            return ((data & 0x7C00) == 0x7C00) && ((data & 0x03FF) == 0);
        }
        
        bool is_denormal() const
        {
            return ((data & 0x7C00) == 0) && ((data & 0x03FF) != 0);
        }
        
        bool isFinite() const
        {
            return (data & 0x7C00) != 0x7C00;
        }
        
        bool is_normal() const
        {
            return ((data & 0x7C00) != 0) && ((data & 0x7C00) != 0x7C00);
        }

        // ============================================================================
        // Работа с битовым представлением
        // ============================================================================
        storage_type bits() const
        {
            return data;
        }
        
        static half from_bits(storage_type bits)
        {
            return half(bits);
        }
        
        // Получение компонентов
        int sign_bit() const { return (data >> 15) & 0x1; }
        int exponent() const { return (data >> 10) & 0x1F; }
        int mantissa() const { return data & 0x03FF; }

        // ============================================================================
        // Утилиты
        // ============================================================================
        bool isValid() const
        {
            return std::isfinite(float(*this));
        }
        
        bool approximately(half other, float epsilon = EPSILON) const
        {
            return Core::Math::approximately(float(*this), float(other), epsilon);
        }
        
        std::string to_string() const
        {
            return std::to_string(float(*this));
        }

    private:
        storage_type data;

        // ============================================================================
        // Внутренние преобразования
        // ============================================================================
        static storage_type float_to_half(float x)
        {
            if (x == 0.0f) return 0;  // быстрая проверка нуля
            
            union {
                float f;
                std::uint32_t i;
            };
            f = x;
            
            std::uint32_t sign = (i >> 16) & 0x8000;  // бит знака
            int exponent = ((i >> 23) & 0xFF) - 127;  // несмещенная экспонента
            std::uint32_t mantissa = i & 0x007FFFFF;  // мантисса
            
            // Обработка специальных случаев
            if (exponent == 128)
            {  // Infinity или NaN
                std::uint32_t result = sign | 0x7C00;  // infinity
                if (mantissa != 0) 
                    result |= (mantissa >> 13) | (mantissa == 0);  // NaN
                return static_cast<storage_type>(result);
            }
            
            // Денормализованные числа и нули
            if (exponent < -14)
            {
                if (exponent < -25) return sign;  // слишком малое число -> 0
                
                // Денормализованное число half
                mantissa |= 0x00800000;  // восстанавливаем скрытый бит
                int shift = 14 - exponent;
                mantissa += (1 << (shift - 1));  // округление
                return static_cast<storage_type>(sign | (mantissa >> shift));
            }
            
            // Нормализованное число
            exponent += 15;  // смещение half
            mantissa += 0x1000;  // округление (nearest even)
            
            if (mantissa & 0x00800000)
            {  // переполнение мантиссы
                mantissa = 0;
                exponent++;
            }
            
            if (exponent > 30)
            {  // переполнение экспоненты -> infinity
                return static_cast<storage_type>(sign | 0x7C00);
            }
            
            return static_cast<storage_type>(sign | (exponent << 10) | (mantissa >> 13));
        }
    };

    // ============================================================================
    // Бинарные операторы с float
    // ============================================================================
    inline half operator+(float lhs, half rhs)
    {
        return half(lhs + float(rhs));
    }
    
    inline half operator-(float lhs, half rhs)
    {
        return half(lhs - float(rhs));
    }
    
    inline half operator*(float lhs, half rhs)
    {
        return half(lhs * float(rhs));
    }
    
    inline half operator/(float lhs, half rhs)
    {
        return half(lhs / float(rhs));
    }

    inline half operator+(half lhs, float rhs)
    {
        return half(float(lhs) + rhs);
    }
    
    inline half operator-(half lhs, float rhs)
    {
        return half(float(lhs) - rhs);
    }
    
    inline half operator*(half lhs, float rhs)
    {
        return half(float(lhs) * rhs);
    }
    
    inline half operator/(half lhs, float rhs)
    {
        return half(float(lhs) / rhs);
    }

    // ============================================================================
    // Математические функции
    // ============================================================================
    inline half abs(half x)
    {
        return half(std::abs(float(x)));
    }
    
    inline half sqrt(half x)
    {
        return half(std::sqrt(float(x)));
    }
    
    inline half sin(half x)
    {
        return half(std::sin(float(x)));
    }
    
    inline half cos(half x)
    {
        return half(std::cos(float(x)));
    }
    
    inline half tan(half x)
    {
        return half(std::tan(float(x)));
    }
    
    inline half exp(half x)
    {
        return half(std::exp(float(x)));
    }
    
    inline half log(half x)
    {
        return half(std::log(float(x)));
    }
    
    inline half pow(half x, half y)
    {
        return half(std::pow(float(x), float(y)));
    }
    
    inline half floor(half x)
    {
        return half(std::floor(float(x)));
    }
    
    inline half ceil(half x)
    {
        return half(std::ceil(float(x)));
    }
    
    inline half round(half x)
    {
        return half(std::round(float(x)));
    }

    // ============================================================================
    // Функции сравнения
    // ============================================================================
    inline bool approximately(half a, half b, float epsilon = EPSILON)
    {
        return a.approximately(b, epsilon);
    }
    
    inline bool isValid(half x)
    {
        return x.isValid();
    }
    
    inline bool isFinite(half x)
    {
        return x.isFinite();
    }
    
    inline bool isNan(half x)
    {
        return x.isNan();
    }
    
    inline bool isInf(half x)
    {
        return x.isInf();
    }

    // ============================================================================
    // Полезные константы
    // ============================================================================
    static const half half_zero(0.0f);
    static const half half_one(1.0f);
    static const half half_max(65504.0f);        // максимальное значение half
    static const half half_min(6.10352e-5f);     // минимальное нормализованное положительное
    static const half half_epsilon(0.00097656f); // машинный epsilon
    static const half half_pi(PI);
    static const half half_two_pi(TWO_PI);
    static const half half_half_pi(HALF_PI);
} // namespace Core::Math
///////////////////////////////////////////////////////////////