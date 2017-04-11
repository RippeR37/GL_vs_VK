#include <base/vkx/DeviceInfo.h>

namespace vkx {
DeviceInfo::DeviceInfo(vk::PhysicalDevice physicalDevice)
    : device(physicalDevice)
    , properties(device.getProperties())
    , features(device.getFeatures())
{
}
}