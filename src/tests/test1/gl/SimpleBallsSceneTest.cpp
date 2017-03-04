#include <tests/test1/gl/SimpleBallsSceneTest.h>

namespace tests {
namespace gl {
    SimpleBallsSceneTest::SimpleBallsSceneTest()
        : GLTest("SimpleBallsSceneTest")
    {
    }

    void SimpleBallsSceneTest::run()
    {
        while (!window_.shouldClose()) {
            window_.update();
        }
    }
}
}
