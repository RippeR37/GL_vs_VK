#pragma once

#include <base/vkx/DeviceInfo.h>

#include <vulkan/vulkan.hpp>

namespace base {
namespace vkx {

struct Buffer
{
    vk::Buffer buffer;
    vk::DeviceMemory memory;
    vk::DeviceSize size;
    vk::DeviceSize offset;
};

struct Image
{
    vk::Image image;
    vk::DeviceMemory memory;
    vk::DeviceSize size;
    vk::DeviceSize offset;
};

class MemoryManager
{
  public:
    MemoryManager(const vk::Device& device, const vkx::DeviceInfo& deviceInfo);
    MemoryManager(MemoryManager&& other);
    MemoryManager(const MemoryManager&) = delete;
    ~MemoryManager();

    MemoryManager& operator=(MemoryManager&& other) = delete;
    MemoryManager& operator=(const MemoryManager&) = delete;

    uint32_t getMemoryTypeIndex(vk::MemoryPropertyFlags flags, uint32_t memoryTypeBits) const;
    vk::DeviceMemory allocateHostVisibleMemory(const vk::MemoryRequirements& memoryRequirements) const;
    vk::DeviceMemory allocateDeviceLocalMemory(const vk::MemoryRequirements& memoryRequirements) const;

    Buffer createStagingBuffer(vk::DeviceSize size) const;

    Buffer copyToDeviceLocalMemory(const Buffer& buffer,
                                   vk::BufferUsageFlags usage,
                                   vk::CommandBuffer cmdBuffer,
                                   vk::Queue queue) const;
    Buffer copyToDeviceLocalMemoryAsync(const Buffer& buffer,
                                        vk::BufferUsageFlags usage,
                                        vk::CommandBuffer cmdBuffer,
                                        vk::Queue queue,
                                        vk::Semaphore semaphore) const;

    void destroyBuffer(Buffer& buffer) const;
    void destroyImage(Image& image) const;

  private:
    const vk::Device& _device;
    const vkx::DeviceInfo& _deviceInfo;
};
}
}
