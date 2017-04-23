#include <base/vkx/MemoryManager.h>

#include <base/vkx/DeviceInfo.h>

#include <exception>
#include <utility>

namespace base {
namespace vkx {
MemoryManager::MemoryManager(const vk::Device& device, const vkx::DeviceInfo& deviceInfo)
    : _device(device)
    , _deviceInfo(deviceInfo)
{
}

MemoryManager::MemoryManager(MemoryManager&& other)
    : MemoryManager(other._device, other._deviceInfo)
{
}

MemoryManager::~MemoryManager()
{
}

uint32_t MemoryManager::getMemoryTypeIndex(vk::MemoryPropertyFlags flags, uint32_t memoryTypeBits) const
{
    const vk::PhysicalDeviceMemoryProperties& deviceMemory = _deviceInfo.memory;

    for (uint32_t index = 0; index < deviceMemory.memoryTypeCount; ++index) {
        if ((memoryTypeBits & (1 << index)) && (deviceMemory.memoryTypes[index].propertyFlags & flags) == flags) {
            return index;
        }
    }

    throw std::system_error{vk::Result::eErrorFormatNotSupported,
                            "Device doesnt support memory with flags:" + vk::to_string(flags)};
}

vk::DeviceMemory MemoryManager::allocateHostVisibleMemory(const vk::MemoryRequirements& memoryRequirements) const
{
    auto flags = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    auto memoryTypeIndex = getMemoryTypeIndex(flags, memoryRequirements.memoryTypeBits);
    return _device.allocateMemory({memoryRequirements.size, memoryTypeIndex});
}

vk::DeviceMemory MemoryManager::allocateDeviceLocalMemory(const vk::MemoryRequirements& memoryRequirements) const
{
    auto flags = vk::MemoryPropertyFlagBits::eDeviceLocal;
    auto memoryTypeIndex = getMemoryTypeIndex(flags, memoryRequirements.memoryTypeBits);
    return _device.allocateMemory({memoryRequirements.size, memoryTypeIndex});
}

Buffer MemoryManager::createStagingBuffer(vk::DeviceSize size) const
{
    vk::Buffer buffer =
        _device.createBuffer({{}, size, vk::BufferUsageFlagBits::eTransferSrc, vk::SharingMode::eExclusive});
    vk::DeviceMemory memory = allocateHostVisibleMemory(_device.getBufferMemoryRequirements(buffer));
    _device.bindBufferMemory(buffer, memory, 0);

    return {buffer, memory, size, 0};
};

Buffer MemoryManager::copyToDeviceLocalMemory(const Buffer& sourceBuffer,
                                              vk::BufferUsageFlags usage,
                                              vk::CommandBuffer cmdBuffer,
                                              vk::Queue queue) const
{
    vk::BufferUsageFlags deviceLocalUsage = usage | vk::BufferUsageFlagBits::eTransferDst;
    vk::Buffer buffer = _device.createBuffer({{}, sourceBuffer.size, deviceLocalUsage, vk::SharingMode::eExclusive});
    vk::DeviceMemory memory = allocateDeviceLocalMemory(_device.getBufferMemoryRequirements(buffer));
    _device.bindBufferMemory(buffer, memory, 0);

    cmdBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr});
    cmdBuffer.copyBuffer(sourceBuffer.buffer, buffer, {vk::BufferCopy{sourceBuffer.offset, 0, sourceBuffer.size}});
    cmdBuffer.end();

    vk::Fence fence = _device.createFence({});
    queue.submit(vk::SubmitInfo{0, nullptr, nullptr, 1, &cmdBuffer, 0, nullptr}, fence);
    _device.waitForFences({{fence}}, VK_FALSE, UINT64_MAX);
    _device.destroyFence(fence);

    return {buffer, memory, sourceBuffer.size, 0};
}

void MemoryManager::destroyBuffer(Buffer& buffer) const
{
    _device.freeMemory(buffer.memory);
    _device.destroyBuffer(buffer.buffer);

    buffer.memory = vk::DeviceMemory{};
    buffer.buffer = vk::Buffer{};
}

void MemoryManager::destroyImage(Image& image) const
{
    _device.freeMemory(image.memory);
    _device.destroyImage(image.image);

    image.memory = vk::DeviceMemory{};
    image.image = vk::Image{};
}
}
}
