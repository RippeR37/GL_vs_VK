#include <base/vkx/DeviceInfo.h>

namespace base {
namespace vkx {
DeviceInfo::DeviceInfo(vk::PhysicalDevice physicalDevice)
    : device(physicalDevice)
    , properties(device.getProperties())
    , features(device.getFeatures())
{
}
}
}