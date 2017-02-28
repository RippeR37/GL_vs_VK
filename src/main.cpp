#include <base/gl/Window.h>

#include <gl/glew.h>
#include <vulkan/vulkan.h>

#include <algorithm>
#include <iostream>
#include <vector>

int test_gl()
{
    using namespace base::gl;

    Window window = base::gl::Window({ 800, 600}, "Hello world");
    window.create();

    while(!window.shouldClose()) {
        window.update();
    }

    Window::deinitialize();

    return 0;
}

void printVersion(std::ostream& stream, uint32_t version)
{
    stream << VK_VERSION_MAJOR(version) << "." << VK_VERSION_MINOR(version) << "." << VK_VERSION_PATCH(version);
}

VkResult createVkInstance(VkInstance& vkInstance)
{
    VkApplicationInfo applicationInfo;
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = "Vulkan Learning 0.1";
    applicationInfo.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    applicationInfo.pEngineName = "LunarG SDK";
    applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    const char* layers[] = { "VK_LAYER_LUNARG_standard_validation" };

    VkInstanceCreateInfo instanceCreateInfo;
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pNext = nullptr;
    instanceCreateInfo.flags = 0; // reserved for future
    instanceCreateInfo.pApplicationInfo = &applicationInfo;
    instanceCreateInfo.enabledLayerCount = 1;
    instanceCreateInfo.ppEnabledLayerNames = layers;
    instanceCreateInfo.enabledExtensionCount = 0;
    instanceCreateInfo.ppEnabledExtensionNames = nullptr;

    return vkCreateInstance(&instanceCreateInfo, nullptr, &vkInstance);
}

VkResult detectVkPhysicalDevices(VkInstance vkInstance, std::vector<VkPhysicalDevice>& vkPhysicalDevices)
{
    VkResult result;
    uint32_t deviceCount;

    if ((result = vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr)) != VK_SUCCESS)
        return result;

    vkPhysicalDevices = std::vector<VkPhysicalDevice>{};
    vkPhysicalDevices.resize(deviceCount);
    if ((result = vkEnumeratePhysicalDevices(vkInstance, &deviceCount, vkPhysicalDevices.data())) != VK_SUCCESS)
        return result;

    std::cout << "Found " << deviceCount << " physical devices:" << std::endl;
    std::cout << std::endl;

    return result;
}

void queryVkPhysicalDeviceProperties(const std::vector<VkPhysicalDevice>& devices,
    std::vector<VkPhysicalDeviceProperties>& deviceProperties)
{
    deviceProperties.resize(devices.size());

    for (size_t i = 0; i < devices.size(); ++i) {
        vkGetPhysicalDeviceProperties(devices[i], &deviceProperties[i]);

        std::cout << "PhysicalDevice[" << i << "].properties:" << std::endl;
        std::cout << "  apiVersion:         ";
        printVersion(std::cout, deviceProperties[i].apiVersion);
        std::cout << std::endl;
        std::cout << "  driverVersion:      ";
        printVersion(std::cout, deviceProperties[i].driverVersion);
        std::cout << std::endl;
        std::cout << "  deviceID:           " << deviceProperties[i].deviceID << std::endl;
        std::cout << "  vendorID:           " << deviceProperties[i].vendorID << std::endl;
        std::cout << "  deviceName:         " << deviceProperties[i].deviceName << std::endl;
        std::cout << "  pipelineCacheUUID:  " << deviceProperties[i].pipelineCacheUUID << std::endl;
    }

    std::cout << std::endl;
}

void queryVkPhysicalDeviceFeatures(const std::vector<VkPhysicalDevice>& devices,
    std::vector<VkPhysicalDeviceFeatures>& deviceFeatures)
{
    deviceFeatures.resize(devices.size());

    for (size_t i = 0; i < devices.size(); ++i) {
        vkGetPhysicalDeviceFeatures(devices[i], &deviceFeatures[i]);
    }
}

uint32_t queryVkPhysicalDeviceQueueFamilyProperties(VkPhysicalDevice device,
    std::vector<VkQueueFamilyProperties>& queueFamilyProperties)
{
    uint32_t count = 0;
    uint32_t resultIndex = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, nullptr);

    queueFamilyProperties.resize(count);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &count, queueFamilyProperties.data());

    std::cout << "Found " << count << " queue families." << std::endl;

    for (size_t i = 0; i < count; ++i) {
        std::cout << "QueueFamiliy[" << i << "].properties:" << std::endl;

        std::cout << "  queueCount:          " << queueFamilyProperties[i].queueCount << std::endl;
        std::cout << "  timestampValidBits:  " << queueFamilyProperties[i].timestampValidBits << std::endl;
        std::cout << "  queueFlags:          " << std::endl;
        std::cout << "    VK_QUEUE_GRAPHICS_BIT:        " << ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) ? 1 : 0) << std::endl;
        std::cout << "    VK_QUEUE_COMPUTE_BIT:         " << ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) ? 1 : 0) << std::endl;
        std::cout << "    VK_QUEUE_TRANSFER_BIT:        " << ((queueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) ? 1 : 0) << std::endl;
        std::cout << "    VK_QUEUE_SPARSE_BINDING_BIT:  " << ((queueFamilyProperties[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) ? 1 : 0) << std::endl;

        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            resultIndex = i;
    }

    std::cout << "Returing queue family index: " << resultIndex << std::endl;
    std::cout << std::endl;

    return resultIndex;
}

VkResult createVkDevice(VkPhysicalDevice physicalDevice, VkDevice* device, uint32_t queueFamilyIndex)
{
    VkPhysicalDeviceFeatures supportedFeatures;
    vkGetPhysicalDeviceFeatures(physicalDevice, &supportedFeatures);

    const float queuePriorities[] = { 1.0f };

    VkDeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.pNext = nullptr;
    queueCreateInfo.flags = 0;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = queuePriorities;

    VkDeviceCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext = nullptr;
    createInfo.flags = 0; // 0 for now
    createInfo.queueCreateInfoCount = 1;
    createInfo.pQueueCreateInfos = &queueCreateInfo;
    createInfo.enabledLayerCount = 0;
    createInfo.ppEnabledLayerNames = nullptr;
    createInfo.enabledExtensionCount = 0;
    createInfo.ppEnabledExtensionNames = nullptr;
    createInfo.pEnabledFeatures = &supportedFeatures;

    return vkCreateDevice(physicalDevice, &createInfo, nullptr, device);
}

int test_vk()
{
    VkInstance vkInstance;
    std::vector<VkPhysicalDevice> vkPhysicalDevices;
    std::vector<VkPhysicalDeviceProperties> vkPhysicalDeviceProperties;
    std::vector<VkPhysicalDeviceFeatures> vkPhysicalDeviceFeatures;
    std::vector<VkQueueFamilyProperties> vkQueueFamilyProperties;
    uint32_t queueFamilyIndex;
    VkPhysicalDevice vkPhysicalDevice;
    VkDevice vkDevice;

    if (createVkInstance(vkInstance) != VK_SUCCESS) {
        std::cerr << "Vulkan instance could not have been created!" << std::endl;
        return -1;
    }

    if (detectVkPhysicalDevices(vkInstance, vkPhysicalDevices) != VK_SUCCESS) {
        std::cerr << "Vulkan physical device could not have been created!" << std::endl;
        return -1;
    }

    queryVkPhysicalDeviceProperties(vkPhysicalDevices, vkPhysicalDeviceProperties);
    queryVkPhysicalDeviceFeatures(vkPhysicalDevices, vkPhysicalDeviceFeatures);

    std::cout << "Selecting first physical device...\n"
              << std::endl;
    vkPhysicalDevice = vkPhysicalDevices.front();

    queueFamilyIndex = queryVkPhysicalDeviceQueueFamilyProperties(vkPhysicalDevice, vkQueueFamilyProperties);

    if (createVkDevice(vkPhysicalDevice, &vkDevice, queueFamilyIndex) != VK_SUCCESS) {
        std::cerr << "Vulkan logic device could not have been created!" << std::endl;
        return -1;
    }

    vkDeviceWaitIdle(vkDevice);
    vkDestroyDevice(vkDevice, nullptr);
    vkDestroyInstance(vkInstance, nullptr);

    return 0;
}

int main()
{
    bool testing_gl = true;

    if (testing_gl) {
        return test_gl();
    } else {
        return test_vk();
    }
}
