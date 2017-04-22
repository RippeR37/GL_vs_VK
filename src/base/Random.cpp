#include <base/Random.h>

#include <glm/vec4.hpp>

#include <random>

namespace base {
namespace random {
std::mt19937& getDefaultGenerator()
{
    static std::random_device rd;
    static std::mt19937 generator{rd()};
    return generator;
}
}
}
