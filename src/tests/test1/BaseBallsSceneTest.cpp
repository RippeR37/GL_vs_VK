#include <tests/test1/BaseBallsSceneTest.h>

#include <base/Random.h>
#include <tests/common/SphereVerticesGenerator.h>

#include <glm/glm.hpp>

namespace {
const std::size_t kBallCount = 200000;
const std::size_t kSphereSlices = 3;
const std::size_t kSphereStacks = 3;
const std::size_t kUpdateRuns = 10;
}

namespace tests {
void BaseBallsSceneTest::initTestState()
{
    // Balls
    {
        const float SPEED_SCALE = 0.3f;

        _balls = std::vector<common::Ball>{};
        _balls.reserve(kBallCount);

        for (size_t i = 0; i < kBallCount; ++i) {
            auto position = base::random::getRandomVec4({-1.0, -1.0, -1.0, 0.0}, {1.0, 1.0, 1.0, 0.0});
            auto color = base::random::getRandomVec4({0.0, 0.0, 0.0, 1.0}, {1.0, 1.0, 1.0, 1.0});
            auto speed = base::random::getRandomVec4({-1.0, -1.0, -1.0, 0.0}, {1.0, 1.0, 1.0, 0.0});

            _balls.push_back({position, color, (glm::normalize(speed) * SPEED_SCALE)});
        }
    }

    // Vertices
    {
        common::SphereVerticesGenerator verticesGenerator{kSphereSlices, kSphereStacks};
        _vertices = verticesGenerator.vertices;
    }
}

void BaseBallsSceneTest::updateTestState(float frameTime)
{
    updateTestState(frameTime, 0, _balls.size());
}

void BaseBallsSceneTest::updateTestState(float frameTime, std::size_t rangeFrom, std::size_t rangeTo)
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

    for (std::size_t run = 0; run < kUpdateRuns; ++run) {
        for (std::size_t ballIndex = rangeFrom; ballIndex < rangeTo; ++ballIndex) {
            common::Ball& ball = _balls[ballIndex];

            ball.position += (frameTime * ball.speed);

            if (clampFloat(ball.position.x, -1.0, 1.0))
                ball.speed.x *= (-1.0);
            if (clampFloat(ball.position.y, -1.0, 1.0))
                ball.speed.y *= (-1.0);
            if (clampFloat(ball.position.z, -1.0, 1.0))
                ball.speed.z *= (-1.0);
        }
    }
}

void BaseBallsSceneTest::destroyTestState()
{
    _balls.clear();
    _vertices.clear();
}

const std::vector<common::Ball>& BaseBallsSceneTest::balls() const
{
    return _balls;
}

const std::vector<glm::vec4>& BaseBallsSceneTest::vertices() const
{
    return _vertices;
}
}
