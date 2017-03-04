#pragma once

#include <framework/GLTest.h>

namespace tests {
namespace gl {
    class SimpleBallsSceneTest : public framework::GLTest {
    public:
        SimpleBallsSceneTest();

        void run() override;
    };
}
}
