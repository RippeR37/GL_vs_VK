#include <base/vkx/Application.h>

#include <base/ContainerUtils.h>

#include <GLFW/glfw3.h>

#include <algorithm>
#include <iostream>
#include <set>

namespace {
const std::vector<const char*> kInstanceLayers{};
const std::vector<const char*> kDebugInstanceLayers{{"VK_LAYER_LUNARG_standard_validation"},
                                                    {"VK_LAYER_LUNARG_monitor"}};
}

namespace base {
namespace vkx {
Application::Application(const std::string& name, const glm::vec2& windowSize, bool debugMode)
    : _name(name)
    , _instance(createInstance((debugMode ? kDebugInstanceLayers : kInstanceLayers)))
    , _deviceInfo(selectPhysicalDevice(instance().enumeratePhysicalDevices()))
    , _device(createDevice())
    , _queueManager(instance(), physicalDevice(), device())
    , _window(*this, windowSize, name)
    , _memory(device(), deviceInfo())
{
}

Application::~Application()
{
}

const std::string& Application::name() const
{
    return _name;
}

const vk::Instance& Application::instance() const
{
    return *_instance;
}

const vk::PhysicalDevice& Application::physicalDevice() const
{
    return deviceInfo().device;
}

const vk::Device& Application::device() const
{
    return *_device;
}

const DeviceInfo& Application::deviceInfo() const
{
    return _deviceInfo;
}

const QueueManager& Application::queues() const
{
    return _queueManager;
}

const Window& Application::window() const
{
    return _window;
}

const vkx::MemoryManager& Application::memory() const
{
    return _memory;
}

Window& Application::window()
{
    return _window;
}

vk::UniqueInstance Application::createInstance(const std::vector<const char*>& layers)
{
    initialize();

    std::vector<std::string> extensions = getRequiredExtensions();
    std::vector<const char*> extensionsView = viewOf(extensions);
    vk::ApplicationInfo applicationInfo{name().c_str(), VK_MAKE_VERSION(1, 0, 0), "LunarG SDK",
                                        VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0};
    vk::InstanceCreateInfo instanceInfo{{},
                                        &applicationInfo,
                                        static_cast<uint32_t>(layers.size()),
                                        layers.data(),
                                        static_cast<uint32_t>(extensionsView.size()),
                                        extensionsView.data()};

    return vk::createInstanceUnique(instanceInfo);
}

DeviceInfo Application::selectPhysicalDevice(const std::vector<vk::PhysicalDevice>& physicalDevices)
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

vk::UniqueDevice Application::createDevice()
{
    std::vector<std::string> extensions = {{VK_KHR_SWAPCHAIN_EXTENSION_NAME}};
    std::vector<const char*> extensionsView = viewOf(extensions);

    vk::PhysicalDeviceFeatures emptyFeatures{0u, 0u, 0u};
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = QueueManager::createInfos(instance(), physicalDevice());
    vk::DeviceCreateInfo deviceCreateInfo{
        {},      static_cast<uint32_t>(queueCreateInfos.size()), queueCreateInfos.data(), 0,
        nullptr, static_cast<uint32_t>(extensionsView.size()),   extensionsView.data(),   &emptyFeatures};

    return physicalDevice().createDeviceUnique(deviceCreateInfo);
}

std::vector<std::string> Application::getRequiredExtensions() const
{
    std::set<std::string> extensions;

    // GLFW-required extensions
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    if (!glfwExtensions) {
        throw std::system_error(vk::Result::eErrorExtensionNotPresent, "GLFW could not resolve needed extensions");
    }
    for (uint32_t extension = 0; extension < glfwExtensionCount; ++extension) {
        extensions.insert(glfwExtensions[extension]);
    }

    return std::vector<std::string>{extensions.begin(), extensions.end()};
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
