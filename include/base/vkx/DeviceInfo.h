#pragma once

#include <vulkan/vulkan.hpp>

namespace vkx {
struct DeviceInfo
{
    DeviceInfo(vk::PhysicalDevice physicalDevice);

    vk::PhysicalDevice device;
    vk::PhysicalDeviceProperties properties;
    vk::PhysicalDeviceFeatures features;
};
}
