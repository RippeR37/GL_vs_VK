#pragma once

#include <base/vkx/DeviceInfo.h>
#include <base/vkx/QueueManager.h>
#include <base/vkx/Window.h>

#include <glm/vec2.hpp>
#include <vulkan/vulkan.hpp>

#include <string>
#include <vector>

namespace base {
namespace vkx {
class Application
{
  public:
    Application(const std::string& name, const glm::vec2& windowSize, bool debugMode);
    Application(const Application&) = delete;
    virtual ~Application();

    Application& operator=(const Application&) = delete;

    const std::string& name() const;
    const vk::Instance& instance() const;
    const vk::PhysicalDevice& physicalDevice() const;
    const vk::Device& device() const;
    const vkx::DeviceInfo& deviceInfo() const;
    const vkx::QueueManager& queues() const;
    const vkx::Window& window() const;

    vkx::Window& window();

  private:
    vk::Instance createInstance(const std::vector<const char*>& layers);
    vkx::DeviceInfo selectPhysicalDevice(const std::vector<vk::PhysicalDevice>& physicalDevices);
    vk::Device createDevice();

    void destroyDevice();
    void destroyInstance();

    static void initialize();
    static void deinitialize();

    std::string _name;
    vk::Instance _instance;
    vkx::DeviceInfo _deviceInfo;
    vk::Device _device;
    vkx::QueueManager _queueManager;
    vkx::Window _window;
};
}
}
