#include <tests/test1/vk/SimpleBallsSceneTest.h>

#include <vulkan/vulkan.hpp>

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace tests {
namespace test_vk {
SimpleBallsSceneTest::SimpleBallsSceneTest() : VKTest("SimpleBallsSceneTest")
{
}

std::string versionString(uint32_t version)
{
    return std::to_string(VK_VERSION_MAJOR(version)) + "." + std::to_string(VK_VERSION_MINOR(version)) + "." +
           std::to_string(VK_VERSION_PATCH(version));
}

vk::Instance createInstance(const std::string& applicationName)
{
    const std::array<const char*, 1> layers{{"VK_LAYER_LUNARG_standard_validation"}};
    const vk::ApplicationInfo applicationInfo{applicationName.c_str(), VK_MAKE_VERSION(0, 1, 0), "LunarG SDK",
                                              VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0};
    const vk::InstanceCreateInfo instanceInfo{{}, &applicationInfo, 1, layers.data(), 0, nullptr};

    return vk::createInstance(instanceInfo);
}

std::vector<vk::PhysicalDeviceProperties> getPhysicalDeviceProperties(
    const std::vector<vk::PhysicalDevice>& physicalDevices)
{
    std::vector<vk::PhysicalDeviceProperties> physicalDeviceProperties;
    physicalDeviceProperties.reserve(physicalDevices.size());

    for (std::size_t i = 0; i < physicalDevices.size(); ++i) {
        const auto& physicalDevice = physicalDevices.at(i);
        physicalDeviceProperties.push_back(physicalDevice.getProperties());

        const auto& deviceProperties = physicalDeviceProperties.back();
        std::cout << "PhysicalDevice[" << i << "].properties:" << std::endl;
        std::cout << "  apiVersion:         " << versionString(deviceProperties.apiVersion) << std::endl;
        std::cout << "  driverVersion:      " << versionString(deviceProperties.driverVersion) << std::endl;
        std::cout << "  deviceID:           " << deviceProperties.deviceID << std::endl;
        std::cout << "  vendorID:           " << deviceProperties.vendorID << std::endl;
        std::cout << "  deviceName:         " << deviceProperties.deviceName << std::endl;
        std::cout << std::endl;
    }

    return physicalDeviceProperties;
}

std::vector<vk::PhysicalDeviceFeatures> getPhysicalDeviceFeatures(
    const std::vector<vk::PhysicalDevice>& physicalDevices)
{
    std::vector<vk::PhysicalDeviceFeatures> physicalDeviceFeatures;
    physicalDeviceFeatures.reserve(physicalDevices.size());

    for (const auto& physicalDevice : physicalDevices)
        physicalDeviceFeatures.push_back(physicalDevice.getFeatures());

    return physicalDeviceFeatures;
}

uint32_t getPhysicalDeviceQueueFamilyIndex(std::vector<vk::QueueFamilyProperties>& queueFamilyProperties)
{
    uint32_t queueFamilyIndex = 0;

    for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
        std::cout << "QueueFamiliy[" << i << "].properties:" << std::endl;
        std::cout << "  queueCount:          " << queueFamilyProperties[i].queueCount << std::endl;
        std::cout << "  timestampValidBits:  " << queueFamilyProperties[i].timestampValidBits << std::endl;
        std::cout << "  queueFlags:          " << std::endl;
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
            std::cout << "    VK_QUEUE_GRAPHICS_BIT" << std::endl;
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eCompute)
            std::cout << "    VK_QUEUE_COMPUTE_BIT" << std::endl;
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eTransfer)
            std::cout << "    VK_QUEUE_TRANSFER_BIT" << std::endl;
        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eSparseBinding)
            std::cout << "    VK_QUEUE_SPARSE_BINDING_BIT" << std::endl;
        std::cout << std::endl;

        if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics)
            queueFamilyIndex = i;
    }

    return queueFamilyIndex;
}

vk::Device createDevice(vk::PhysicalDevice physicalDevice, uint32_t queueFamilyIndex)
{
    const std::array<float, 1> queuePriorities{{1.0f}};
    const vk::PhysicalDeviceFeatures supportedFeatures = physicalDevice.getFeatures();
    const vk::DeviceQueueCreateInfo deviceQueueInfo{{}, queueFamilyIndex, 1, queuePriorities.data()};
    const vk::DeviceCreateInfo deviceInfo{{}, 1, &deviceQueueInfo, 0, nullptr, 0, nullptr, &supportedFeatures};

    return physicalDevice.createDevice(deviceInfo);
}

void SimpleBallsSceneTest::run()
{
    // Setup
    vk::Instance instance = createInstance("Vulkan application");
    std::vector<vk::PhysicalDevice> physicalDevices = instance.enumeratePhysicalDevices();
    std::vector<vk::PhysicalDeviceProperties> physicalDeviceProperties = getPhysicalDeviceProperties(physicalDevices);
    std::vector<vk::PhysicalDeviceFeatures> physicalDeviceFeatures = getPhysicalDeviceFeatures(physicalDevices);

    vk::PhysicalDevice& physicalDevice = physicalDevices.at(0);
    std::vector<vk::QueueFamilyProperties> queueFamilyProperties = physicalDevice.getQueueFamilyProperties();
    uint32_t queueFamilyIndex = getPhysicalDeviceQueueFamilyIndex(queueFamilyProperties);
    vk::Device device = createDevice(physicalDevice, queueFamilyIndex);

    // Logic
    // ...

    // Cleanup
    device.waitIdle();
    device.destroy();
    instance.destroy();
}
}
}
