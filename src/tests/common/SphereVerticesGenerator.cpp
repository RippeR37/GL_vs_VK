#include <tests/common/SphereVerticesGenerator.h>

#include <cmath>

namespace tests {
namespace common {
SphereVerticesGenerator::SphereVerticesGenerator(std::size_t slices, std::size_t stacks, float radius)
    : vertices(generateVertices(slices, stacks, radius))
{
}

std::vector<glm::vec4> SphereVerticesGenerator::generateVertices(std::size_t slices, std::size_t stacks, float radius)
{
    const double PI = 3.141592653589793;
    const double sliceStep = 360.0 / (double)slices;
    const double stackStep = 180.0 / (double)stacks;
    std::vector<glm::vec4> result;

    auto degToRad = [PI](double deg) { return (PI / 180.0) * deg; };

    auto pointOf = [degToRad, radius](double theta, double phi) {
        return glm::vec4{radius * std::cos(degToRad(theta)) * std::cos(degToRad(phi)),
                         radius * std::cos(degToRad(theta)) * std::sin(degToRad(phi)),
                         radius * std::sin(degToRad(theta)), 1.0};
    };

    for (std::size_t stack = 0; stack < stacks; ++stack) {
        double theta = -90.0 + stack * stackStep;
        double thetaNext = -90.0 + (stack + 1) * stackStep;

        for (std::size_t slice = 0; slice < slices; ++slice) {
            double phi = slice * sliceStep;
            double phiNext = (slice + 1) * sliceStep;

            result.push_back(pointOf(theta, phi));
            result.push_back(pointOf(thetaNext, phi));
            result.push_back(pointOf(theta, phiNext));

            result.push_back(pointOf(theta, phiNext));
            result.push_back(pointOf(thetaNext, phi));
            result.push_back(pointOf(thetaNext, phiNext));
        }
    }

    return result;
}
}
}
