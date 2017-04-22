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

glm::vec4 getRandomVec4(const glm::vec4& min, const glm::vec4& max)
{
    glm::vec4 result;

    result.x = getRandomRealFromRange(min.x, max.x);
    result.y = getRandomRealFromRange(min.y, max.y);
    result.z = getRandomRealFromRange(min.z, max.z);
    result.w = getRandomRealFromRange(min.w, max.w);

    return result;
}
}
}
