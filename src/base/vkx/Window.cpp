#include <base/vkx/Window.h>

#include <GLFW/glfw3.h>

namespace vkx {
Window::Window(const vk::Instance& instance, const glm::uvec2& size, const std::string& windowTitle)
    : _instance(instance)
    , _size(size)
    , _title(windowTitle)
    , _handle(createWindow())
    , _surface(createSurface())
{
    title(_title);
}

Window::~Window()
{
    destroyWindow();
}

bool Window::shouldClose() const
{
    return (glfwWindowShouldClose(_handle) != 0);
}

const std::string& Window::title() const
{
    return _title;
}

void Window::title(const std::string& title)
{
    _title = title;
    glfwSetWindowTitle(_handle, _title.c_str());
}

const glm::uvec2& Window::size() const
{
    return _size;
}

const vk::SurfaceKHR& Window::surface() const
{
    return _surface;
}

GLFWwindow* Window::createWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    GLFWwindow* handle = glfwCreateWindow(size().x, size().y, title().c_str(), nullptr, nullptr);

    if (!handle) {
        throw std::system_error(vk::Result::eErrorInitializationFailed, "Could not create GLFW window");
    }

    return handle;
}

vk::SurfaceKHR Window::createSurface()
{
    VkSurfaceKHR surface = VK_NULL_HANDLE;
    glfwCreateWindowSurface(static_cast<VkInstance>(_instance), _handle, nullptr, &surface);

    return vk::SurfaceKHR{surface};
}

void Window::destroySurface()
{
    _instance.destroySurfaceKHR(surface());
}

void Window::destroyWindow()
{
    glfwDestroyWindow(_handle);
}
}
