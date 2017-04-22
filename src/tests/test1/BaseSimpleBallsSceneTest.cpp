#include <tests/test1/BaseSimpleBallsSceneTest.h>

#include <base/Random.h>
#include <tests/common/SphereVerticesGenerator.h>

#include <glm/glm.hpp>

namespace tests {
void BaseSimpleBallsSceneTest::initTestState()
{
    // Balls
    {
        const std::size_t BALL_COUNT = 50000;
        const float SPEED_SCALE = 0.3f;

        _balls = std::vector<common::Ball>{};
        _balls.reserve(BALL_COUNT);

        for (size_t i = 0; i < BALL_COUNT; ++i) {
            auto position = base::random::getRandomVec4({-1.0, -1.0, -1.0, 0.0}, {1.0, 1.0, 1.0, 0.0});
            auto color = base::random::getRandomVec4({0.0, 0.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0});
            auto speed = base::random::getRandomVec4({-1.0, -1.0, -1.0, 0.0}, {1.0, 1.0, 1.0, 0.0});

            _balls.push_back({position, color, (glm::normalize(speed) * SPEED_SCALE)});
        }
    }

    // Vertices
    {
        const std::size_t SPHERE_SLICES = 4;
        const std::size_t SPHERE_STACKS = 4;

        common::SphereVerticesGenerator verticesGenerator{SPHERE_SLICES, SPHERE_STACKS};
        _vertices = verticesGenerator.vertices;
    }
}

void BaseSimpleBallsSceneTest::updateTestState(float frameTime)
{
    auto clampFloat = [](float& v, float min, float max) -> bool {
        if (v < min) {
            v = min;
            return true;
        } else if (v > max) {
            v = max;
            return true;
        }
        return false;
    };

    for (auto& ball : _balls) {
        ball.position += (frameTime * ball.speed);

        if (clampFloat(ball.position.x, -1.0, 1.0))
            ball.speed.x *= (-1.0);
        if (clampFloat(ball.position.y, -1.0, 1.0))
            ball.speed.y *= (-1.0);
        if (clampFloat(ball.position.z, -1.0, 1.0))
            ball.speed.z *= (-1.0);
    }
}

void BaseSimpleBallsSceneTest::destroyTestState()
{
    _balls.clear();
    _vertices.clear();
}

const std::vector<common::Ball>& BaseSimpleBallsSceneTest::balls() const
{
    return _balls;
}

const std::vector<glm::vec4>& BaseSimpleBallsSceneTest::vertices() const
{
    return _vertices;
}
}
