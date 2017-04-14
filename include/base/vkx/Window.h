#pragma once

#include <glm/vec2.hpp>
#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

struct GLFWwindow;

namespace base {
namespace vkx {

class Application;

class Window
{
  public:
    Window(const Application& application, const glm::uvec2& size, const std::string& title);
    Window(const Window&) = delete;
    virtual ~Window();

    Window& operator=(const Window&) = delete;

    void update();
    bool shouldClose() const;

    const std::string& title() const;
    void title(const std::string& title);

    const glm::uvec2& size() const;
    const vk::SurfaceKHR& surface() const;
    const vk::SwapchainKHR& swapchain() const;
    const std::vector<vk::Image>& swapchainImages() const;
    const std::vector<vk::ImageView> swapchainImageViews() const;

  private:
    GLFWwindow* createWindow();
    vk::SurfaceKHR createSurface();
    vk::SwapchainKHR createSwapchain();
    std::vector<vk::Image> querySwapchainImages();
    std::vector<vk::ImageView> createSwapchainImageViews();

    void destroySwapchainImageViews();
    void destroySwapchain();
    void destroySurface();
    void destroyWindow();

    vk::SurfaceFormatKHR getSupportedSwapchainSurfaceFormat() const;
    vk::PresentModeKHR getSupportedSwapchainPresentMode() const;

    const Application& _application;
    glm::uvec2 _size;
    std::string _title;
    GLFWwindow* _handle;
    vk::SurfaceKHR _surface;
    vk::SurfaceFormatKHR _swapchainSurfaceFormat;
    vk::SwapchainKHR _swapchain;
    std::vector<vk::Image> _swapchainImages;
    std::vector<vk::ImageView> _swapchainImageViews;
};
}
}
