#include <base/vkx/Window.h>

#include <base/vkx/Application.h>

#include <GLFW/glfw3.h>

namespace base {
namespace vkx {
Window::Window(const Application& application, const glm::uvec2& size, const std::string& windowTitle)
    : _application(application)
    , _size(size)
    , _title(windowTitle)
    , _handle(createWindow())
    , _surface(createSurface())
    , _swapchainSurfaceFormat(getSupportedSwapchainSurfaceFormat())
    , _swapchain(createSwapchain())
    , _swapchainImages(querySwapchainImages())
    , _swapchainImageViews(createSwapchainImageViews())
{
    title(_title);
}

Window::~Window()
{
    destroySwapchainImageViews();
    destroySwapchain();
    destroySurface();
    destroyWindow();
}

void Window::update()
{
    if (shouldClose()) {
        return;
    }

    glfwPollEvents();
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

const vk::SwapchainKHR& Window::swapchain() const
{
    return _swapchain;
}

const std::vector<vk::Image>& Window::swapchainImages() const
{
    return _swapchainImages;
}

const std::vector<vk::ImageView> Window::swapchainImageViews() const
{
    return _swapchainImageViews;
}

const vk::Format Window::swapchainImageFormat() const
{
    return _swapchainSurfaceFormat.format;
}

GLFWwindow* Window::createWindow()
{
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
    GLFWwindow* handle = glfwCreateWindow(size().x, size().y, title().c_str(), nullptr, nullptr);

    if (!handle) {
        throw std::system_error(vk::Result::eErrorInitializationFailed, "Could not create GLFW window");
    }

    return handle;
}

vk::SurfaceKHR Window::createSurface()
{
    VkSurfaceKHR surfaceHandle = VK_NULL_HANDLE;
    glfwCreateWindowSurface(static_cast<VkInstance>(_application.instance()), _handle, nullptr, &surfaceHandle);

    vk::SurfaceKHR surface{surfaceHandle};
    if (!_application.physicalDevice().getSurfaceSupportKHR(_application.queues().familyIndex(), surface)) {
        throw std::system_error(vk::Result::eErrorInitializationFailed,
                                "Created surface is not supported by chosen queue family");
    }

    return surface;
}

vk::SwapchainKHR Window::createSwapchain()
{
    vk::SurfaceCapabilitiesKHR surfaceCapabilities = _application.physicalDevice().getSurfaceCapabilitiesKHR(surface());

    if (!(surfaceCapabilities.supportedUsageFlags & vk::ImageUsageFlagBits::eColorAttachment)) {
        throw std::system_error(vk::Result::eErrorFormatNotSupported, "surface doesn't support ColorAttachment usage");
    }

    uint32_t minImageCount = std::min(3u, surfaceCapabilities.maxImageCount); // TODO: increase
    vk::PresentModeKHR presentMode = getSupportedSwapchainPresentMode();
    vk::Extent2D surfaceExtent = (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()
                                      ? surfaceCapabilities.currentExtent
                                      : vk::Extent2D{size().x, size().y});
    vk::SwapchainCreateInfoKHR swapchainInfo{{},
                                             surface(),
                                             minImageCount,
                                             _swapchainSurfaceFormat.format,
                                             _swapchainSurfaceFormat.colorSpace,
                                             surfaceExtent,
                                             1,
                                             vk::ImageUsageFlagBits::eColorAttachment,
                                             vk::SharingMode::eExclusive,
                                             0,
                                             nullptr,
                                             surfaceCapabilities.currentTransform,
                                             vk::CompositeAlphaFlagBitsKHR::eOpaque,
                                             presentMode,
                                             VK_TRUE,
                                             VK_NULL_HANDLE};

    return _application.device().createSwapchainKHR(swapchainInfo);
}

std::vector<vk::Image> Window::querySwapchainImages()
{
    return _application.device().getSwapchainImagesKHR(swapchain());
}

std::vector<vk::ImageView> Window::createSwapchainImageViews()
{
    std::vector<vk::ImageView> views;

    views.reserve(_swapchainImages.size());
    for (const vk::Image& image : swapchainImages()) {
        vk::ImageViewCreateInfo viewInfo{{},
                                         image,
                                         vk::ImageViewType::e2D,
                                         _swapchainSurfaceFormat.format,
                                         {},
                                         {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1}};
        views.push_back(_application.device().createImageView(viewInfo));
    }

    return views;
}

void Window::destroySwapchainImageViews()
{
    for (const vk::ImageView& imageView : swapchainImageViews()) {
        _application.device().destroyImageView(imageView);
    }
    _swapchainImageViews.clear();
}

void Window::destroySwapchain()
{
    _swapchainImages.clear();
    _application.device().destroySwapchainKHR(_swapchain);
}

void Window::destroySurface()
{
    _application.instance().destroySurfaceKHR(surface());
}

void Window::destroyWindow()
{
    glfwDestroyWindow(_handle);
}

vk::SurfaceFormatKHR Window::getSupportedSwapchainSurfaceFormat() const
{
    const std::vector<vk::SurfaceFormatKHR> usableFormats = {
        {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear},
        {vk::Format::eR8G8B8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear}};
    const std::vector<vk::SurfaceFormatKHR> supportedFormats =
        _application.physicalDevice().getSurfaceFormatsKHR(surface());

    for (const vk::SurfaceFormatKHR& usableFormat : usableFormats) {
        auto usableFormatIt = std::find(supportedFormats.begin(), supportedFormats.end(), usableFormat);
        if (usableFormatIt != supportedFormats.end()) {
            return *usableFormatIt;
        }
    }

    throw std::system_error{vk::Result::eErrorFormatNotSupported,
                            "PhysicalDevice doesn't suport any of necessary surface formats"};
}

vk::PresentModeKHR Window::getSupportedSwapchainPresentMode() const
{
    // For maximum performance we need to select one of these, otherwise we won't be able to
    // produce more frames then `maxImageCount * refreshRate`.
    const std::vector<vk::PresentModeKHR> usableModes = {{vk::PresentModeKHR::eImmediate},
                                                         {vk::PresentModeKHR::eMailbox}};
    const std::vector<vk::PresentModeKHR> supportedModes =
        _application.physicalDevice().getSurfacePresentModesKHR(surface());

    for (const vk::PresentModeKHR& usableMode : usableModes) {
        auto usableModeIt = std::find(supportedModes.begin(), supportedModes.end(), usableMode);
        if (usableModeIt != supportedModes.end()) {
            return *usableModeIt;
        }
    }

    throw std::system_error{vk::Result::eErrorFormatNotSupported,
                            "PhysicalDevice doesn't suport any of necessary surface present modes"};
}
}
}
