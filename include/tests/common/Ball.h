#pragma once

#include <glm/vec4.hpp>

namespace tests {
namespace common {
    struct Ball {
        glm::vec4 position;
        glm::vec4 color;
        glm::vec4 speed;
    };
}
}
