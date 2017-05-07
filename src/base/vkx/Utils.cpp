#include <base/vkx/Utils.h>

namespace {
const glm::mat4 vkClipMatrix{1.0f, 0.0f,  0.0f, 0.0f, //
                             0.0f, -1.0f, 0.0f, 0.0f, //
                             0.0f, 0.0f,  0.5f, 0.0f, //
                             0.0f, 0.0f,  0.5f, 1.0f};
}

namespace base {
namespace vkx {
glm::mat4 fixGLMatrix(const glm::mat4& glMatrix)
{
    return vkClipMatrix * glMatrix;
}
}
}
