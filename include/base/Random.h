#pragma once

#include <glm/vec4.hpp>

#include <random>
#include <type_traits>

namespace base {
namespace random {
std::mt19937& getDefaultGenerator()
{
    static std::random_device rd;
    static std::mt19937 generator{rd()};
    return generator;
}

template <typename T>
T getRandomInteger()
{
    static_assert(std::is_integral<T>::value, "base::getRandom<T> - T must be an integral type!");
    static_assert(!std::is_same<typename std::make_unsigned<T>::type, char>::value,
                  "base::getRandom<T> - T must not be character type");

    std::uniform_int_distribution<T> distribution;
    return distribution(getDefaultGenerator());
}

template <typename T>
T getRandomReal()
{
    static_assert(std::is_floating_point<T>::value, "base::getRandom<T> - T must be an integral type!");

    std::uniform_real_distribution<T> distribution;
    return distribution(getDefaultGenerator());
}

template <typename T>
T getRandomIntegerFromRange(const T& min, const T& max)
{
    static_assert(std::is_integral<T>::value, "base::getRandom<T> - T must be an integral type!");
    static_assert(!std::is_same<typename std::make_unsigned<T>::type, char>::value,
                  "base::getRandom<T> - T must not be character type");

    std::uniform_int_distribution<T> distribution{min, max};
    return distribution(getDefaultGenerator());
}

template <typename T>
T getRandomRealFromRange(const T& min, const T& max)
{
    static_assert(std::is_floating_point<T>::value, "base::getRandom<T> - T must be an integral type!");

    std::uniform_real_distribution<T> distribution{min, max};
    return distribution(getDefaultGenerator());
}

// Specializations

template <>
bool getRandomInteger<bool>()
{
    return getRandomIntegerFromRange<short>(0, 1) != 0;
}
}
}
