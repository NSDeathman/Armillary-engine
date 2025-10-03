///////////////////////////////////////////////////////////////
// Created: 29.09.2025
// Author: DeepSeek, NS_Deathman
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include <limits>
#include "math_defines.h"
#include "math_constants.h"
///////////////////////////////////////////////////////////////
namespace Core::Math
{
	// ============================================================================
    // Вспомогательные функции для сравнения чисел с плавающей точкой
    // ============================================================================

    // Базовый шаблон для абсолютного сравнения
    template<typename T>
    constexpr bool approximatelyAbsolute(T a, T b, T epsilon)
    {
        static_assert(std::is_floating_point_v<T>, "approximately requires floating point types");
        return std::abs(a - b) <= epsilon;
    }

    // Базовый шаблон для относительного сравнения (более надежный для больших чисел)
    template<typename T>
    constexpr bool approximatelyRelative(T a, T b, T epsilon)
    {
        static_assert(std::is_floating_point_v<T>, "approximately requires floating point types");
        
        // Защита от деления на ноль
        if (a == b) return true;
        if (a == 0 || b == 0) return std::abs(a - b) <= epsilon;
        
        // Относительная ошибка
        return std::abs(a - b) <= epsilon * max(std::abs(a), std::abs(b));
    }

    // Комбинированный подход (абсолютный + относительный)
    template<typename T>
    constexpr bool approximatelyCombined(T a, T b, T absEpsilon, T relEpsilon)
    {
        static_assert(std::is_floating_point_v<T>, "approximately requires floating point types");
        
        // Абсолютная разница для чисел близких к нулю
        T diff = std::abs(a - b);
        if (diff <= absEpsilon)
            return true;
            
        // Относительная разница для больших чисел
        T largest = std::max(std::abs(a), std::abs(b));
        return diff <= largest * relEpsilon;
    }

    // ============================================================================
    // Основные функции сравнения (как в Unity и других движках)
    // ============================================================================

    // Основная функция approximately (использует комбинированный подход)
    template<typename T>
    constexpr bool approximately(T a, T b, T epsilon = Constants<T>::Epsilon)
    {
        return approximatelyCombined(a, b, epsilon, epsilon);
    }

    // Специализации для float и double
    inline bool approximately(float a, float b, float epsilon = EPSILON)
    {
        return approximatelyCombined(a, b, epsilon, epsilon);
    }

    inline bool approximately(double a, double b, double epsilon = Constants<double>::Epsilon)
    {
        return approximatelyCombined(a, b, epsilon, epsilon);
    }

    // Быстрая версия для float (использует только абсолютную разницу)
    inline bool approximatelyFast(float a, float b, float epsilon = EPSILON)
    {
        return std::abs(a - b) <= epsilon;
    }

    // Версия для сравнения с нулем
    template<typename T>
    constexpr bool approximatelyZero(T value, T epsilon = Constants<T>::Epsilon)
    {
        return std::abs(value) <= epsilon;
    }

    // ============================================================================
    // Специализированные функции сравнения
    // ============================================================================

    // Сравнение с учетом единицы измерения (ULPs - Units in Last Place)
    // Более надежно, но медленнее
    inline bool approximatelyULP(float a, float b, int maxUlps = 4)
    {
        // Преобразуем float в int для сравнения битовых представлений
        int32_t intA = *reinterpret_cast<const int32_t*>(&a);
        int32_t intB = *reinterpret_cast<const int32_t*>(&b);
        
        // Учитываем знак (делаем числа положительными)
        if (intA < 0) intA = 0x80000000 - intA;
        if (intB < 0) intB = 0x80000000 - intB;
        
        return std::abs(intA - intB) <= maxUlps;
    }

    // Сравнение углов (учитывает цикличность)
    inline bool approximatelyAngle(float a, float b, float epsilon = EPSILON)
    {
        float diff = std::abs(a - b);
        diff = fmin(diff, TWO_PI - diff);  // Учитываем цикличность углов
        return diff <= epsilon;
    }

    // Сравнение для цветов (учитывает gamma пространство)
    inline bool approximatelyColor(float a, float b, float epsilon = EPSILON)
    {
        // Для цветов используем более строгое сравнение
        return approximately(a, b, epsilon * 0.1f);
    }

    // ============================================================================
    // Функции для векторных типов
    // ============================================================================


    // ============================================================================
    // Функции для матриц
    // ============================================================================



    // ============================================================================
    // Функции для half типов
    // ============================================================================


    // ============================================================================
    // Дополнительные функции сравнения
    // ============================================================================

    // Строгое неравенство с учетом эпсилон
    template<typename T>
    constexpr bool GreaterThan(T a, T b, T epsilon = Constants<T>::Epsilon)
    {
        return a > b + epsilon;
    }

    template<typename T>
    constexpr bool LessThan(T a, T b, T epsilon = Constants<T>::Epsilon)
    {
        return a < b - epsilon;
    }

    template<typename T>
    constexpr bool GreaterThanOrEqual(T a, T b, T epsilon = Constants<T>::Epsilon)
    {
        return a >= b - epsilon;
    }

    template<typename T>
    constexpr bool LessThanOrEqual(T a, T b, T epsilon = Constants<T>::Epsilon)
    {
        return a <= b + epsilon;
    }

    // ============================================================================
    // Утилиты для отладки и валидации
    // ============================================================================

    // Проверка на валидные числа (не NaN и не Infinity)
    template<typename T>
    constexpr bool isValid(T value)
    {
        return std::isfinite(value);
    }

    // Assert версии для отладки
#ifdef MATH_DEBUG
    template<typename T>
    inline void AssertValid(const T& value, const char* message = "Invalid value detected")
    {
        MATH_ASSERT(isValid(value), message);
    }

    inline void AssertNormalized(const float3& vec, const char* message = "Vector is not normalized")
    {
        MATH_ASSERT(IsNormalized(vec), message);
    }

    inline void AssertOrthogonal(const float3& a, const float3& b, const char* message = "Vectors are not orthogonal")
    {
        MATH_ASSERT(AreOrthogonal(a, b), message);
    }
#else
    template<typename T>
    inline void AssertValid(const T&, const char* = "") {}
    
    //inline void AssertNormalized(const float3&, const char* = "") {}
    //inline void AssertOrthogonal(const float3&, const float3&, const char* = "") {}
#endif
} // namespace Core::Math
///////////////////////////////////////////////////////////////