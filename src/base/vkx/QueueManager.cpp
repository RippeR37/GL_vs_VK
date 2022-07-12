#include <base/vkx/QueueManager.h>

#include <GLFW/glfw3.h>

namespace base {
namespace vkx {
std::vector<vk::DeviceQueueCreateInfo> QueueManager::createInfos(const vk::Instance& instance,
                                                                 const vk::PhysicalDevice& device)
{
    static const std::array<float, 1> priorities = {1.0f};

    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = device.getQueueFamilyProperties();
    std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

    // According to spec, each queue supporting GRAPHICS operations also supports TRANSFER operations,
    // so let's look only for GRAPHICS flag.
    for (uint32_t queueFamilyIndex = 0; queueFamilyIndex < queueFamilyProperties.size(); ++queueFamilyIndex) {
        if (!glfwGetPhysicalDevicePresentationSupport(static_cast<const VkInstance>(instance),
                                                      static_cast<const VkPhysicalDevice>(device), queueFamilyIndex)) {
            // Skip queue family which cannot be used for presentation
            continue;
        }

        if (queueFamilyProperties[queueFamilyIndex].queueFlags & vk::QueueFlagBits::eGraphics) {
            queueCreateInfos.push_back({{}, queueFamilyIndex, static_cast<uint32_t>(priorities.size()), priorities.data()});
        }
    }

    if (queueCreateInfos.empty()) {
        throw std::system_error(vk::Result::eErrorInitializationFailed,
                                "Unable to find queue supporting graphics operations");
    }

    return queueCreateInfos;
}

QueueManager::QueueManager(const vk::Instance& instance,
                           const vk::PhysicalDevice& physicalDevice,
                           const vk::Device& device)
    : _familyIndex(chooseFamilyIndex(instance, physicalDevice))
    , _queue(createQueue(device))
{
}

QueueManager::~QueueManager()
{
}

uint32_t QueueManager::familyIndex() const
{
    return _familyIndex;
}

const vk::Queue& QueueManager::queue() const
{
    return _queue;
}

uint32_t QueueManager::chooseFamilyIndex(const vk::Instance& instance, const vk::PhysicalDevice& physicalDevice) const
{
    return createInfos(instance, physicalDevice).front().queueFamilyIndex;
}

vk::Queue QueueManager::createQueue(const vk::Device& device)
{
    return device.getQueue(familyIndex(), 0);
}
}
}
