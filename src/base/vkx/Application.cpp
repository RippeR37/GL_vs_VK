#include <base/vkx/Application.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>

namespace {
const std::vector<const char*> kInstanceLayers{};
const std::vector<const char*> kDebugInstanceLayers{{"VK_LAYER_LUNARG_standard_validation"}};
}

namespace base {
namespace vkx {
Application::Application(const std::string& name, const glm::vec2& windowSize, bool debugMode)
    : _name(name)
    , _instance(createInstance((debugMode ? kDebugInstanceLayers : kInstanceLayers)))
    , _deviceInfo(selectPhysicalDevice(instance().enumeratePhysicalDevices()))
    , _device(createDevice())
    , _queueManager(instance(), physicalDevice(), device())
    , _window(instance(), windowSize, name)
{
}

Application::~Application()
{
    destroyDevice();
    destroyInstance();
}

const std::string& Application::name() const
{
    return _name;
}

const vk::Instance& Application::instance() const
{
    return _instance;
}

const vk::PhysicalDevice& Application::physicalDevice() const
{
    return deviceInfo().device;
}

const vk::Device& Application::device() const
{
    return _device;
}

const vkx::DeviceInfo& Application::deviceInfo() const
{
    return _deviceInfo;
}

const vkx::QueueManager& Application::queues() const
{
    return _queueManager;
}

vk::Instance Application::createInstance(const std::vector<const char*>& layers)
{
    initialize();

    // GLFW-required extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (!glfwExtensions) {
        throw std::system_error(vk::Result::eErrorExtensionNotPresent, "GLFW could not resolve needed extensions");
    }

    vk::ApplicationInfo applicationInfo{name().c_str(), VK_MAKE_VERSION(0, 1, 0), "LunarG SDK",
                                        VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0};
    vk::InstanceCreateInfo instanceInfo{
        {}, &applicationInfo, layers.size(), layers.data(), glfwExtensionCount, glfwExtensions};

    return vk::createInstance(instanceInfo);
}

vkx::DeviceInfo Application::selectPhysicalDevice(const std::vector<vk::PhysicalDevice>& physicalDevices)
{
    const auto deviceScore = [](const vk::PhysicalDevice& device) {
        switch (device.getProperties().deviceType) {
        case vk::PhysicalDeviceType::eOther:
            return 0;
            break;
        case vk::PhysicalDeviceType::eIntegratedGpu:
            return 100;
            break;
        case vk::PhysicalDeviceType::eDiscreteGpu:
            return 1000;
            break;
        case vk::PhysicalDeviceType::eVirtualGpu:
            return 50;
            break;
        case vk::PhysicalDeviceType::eCpu:
            return 10;
            break;
        }
        return -1;
    };

    const auto deviceComparator = [&deviceScore](const vk::PhysicalDevice& lhs, const vk::PhysicalDevice& rhs) {
        return deviceScore(lhs) < deviceScore(rhs);
    };

    return {*std::max_element(std::begin(physicalDevices), std::end(physicalDevices), deviceComparator)};
}

vk::Device Application::createDevice()
{
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos =
        vkx::QueueManager::createInfos(instance(), physicalDevice());
    vk::DeviceCreateInfo deviceCreateInfo{{},      queueCreateInfos.size(), queueCreateInfos.data(), 0, nullptr, 0,
                                          nullptr, &deviceInfo().features};

    return physicalDevice().createDevice(deviceCreateInfo);
}

void Application::destroyDevice()
{
    _device.waitIdle();
    _device.destroy();
}

void Application::destroyInstance()
{
    _instance.destroy();
    deinitialize();
}

void Application::initialize()
{
    if (!glfwInit()) {
        throw std::system_error(vk::Result::eErrorInitializationFailed, "Could not initialize GLFW");
    }
}

void Application::deinitialize()
{
    glfwTerminate();
}
}
}
