#pragma once

#include <glm/vec2.hpp>
#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

struct GLFWwindow;

namespace base {
namespace vkx {
class Window
{
  public:
    Window(const vk::Instance& instance, const glm::uvec2& size, const std::string& title);
    Window(const Window&) = delete;
    virtual ~Window();

    Window& operator=(const Window&) = delete;

    void update();
    bool shouldClose() const;

    const std::string& title() const;
    void title(const std::string& title);

    const glm::uvec2& size() const;
    const vk::SurfaceKHR& surface() const;

  private:
    GLFWwindow* createWindow();
    vk::SurfaceKHR createSurface();

    void destroySurface();
    void destroyWindow();

    const vk::Instance& _instance;
    glm::uvec2 _size;
    std::string _title;
    GLFWwindow* _handle;
    vk::SurfaceKHR _surface;
};
}
}
