#pragma once

#include <base/vkx/DeviceInfo.h>

#include <vulkan/vulkan.hpp>

#include <vector>

namespace base {
namespace vkx {
class QueueManager
{
  public:
    static std::vector<vk::DeviceQueueCreateInfo> createInfos(const vk::Instance& instance,
                                                              const vk::PhysicalDevice& device);

  public:
    QueueManager(const vk::Instance& instance, const vk::PhysicalDevice& physicalDevice, const vk::Device& device);
    ~QueueManager();

    uint32_t familyIndex() const;
    const vk::Queue& queue() const;

  private:
    uint32_t chooseFamilyIndex(const vk::Instance& instance, const vk::PhysicalDevice& physicalDevice) const;
    vk::Queue createQueue(const vk::Device& device);

  private:
    uint32_t _familyIndex;
    vk::Queue _queue;
};
}
}
