#include <framework/VKTest.h>

#include <iostream>

namespace {
const bool kDebugEnabled = false;
}

namespace framework {
VKTest::VKTest(const std::string& testName, bool benchmarkMode, float benchmarkTime)
    : BenchmarkableTest(benchmarkMode, benchmarkTime)
    , base::vkx::Application("[VK] " + testName, {WINDOW_WIDTH, WINDOW_HEIGHT}, kDebugEnabled)
{
}

void VKTest::setup()
{
}

void VKTest::teardown()
{
}

void VKTest::printStatistics() const
{
    // These values are taken from http://pcidatabase.com/
    auto vendorName = [](uint32_t vendorId) -> std::string {
        switch (vendorId) {
        case 0x10DE:
            return "Nvidia";
        case 0x1002:
        case 0x1022:
            return "AMD";
        case 0x163C:
        case 0x8086:
        case 0x8087:
            return "Intel";
        default:
            return "UNKNOWN";
        }
    };

    auto version = [](uint32_t version) -> std::string {
        return std::to_string(VK_VERSION_MAJOR(version)) + "." + std::to_string(VK_VERSION_MINOR(version)) + "." +
               std::to_string(VK_VERSION_PATCH(version));
    };

    std::cout << "Hardware/software information" << std::endl;
    std::cout << "=============================" << std::endl;
    std::cout << "  Vendor:           " << vendorName(deviceInfo().properties.vendorID)
              << " (ID: " << deviceInfo().properties.vendorID << ")" << std::endl;
    std::cout << "  Device:           " << deviceInfo().properties.deviceName
              << " (ID: " << deviceInfo().properties.deviceID << ")" << std::endl;
    std::cout << "  Device type:      " << vk::to_string(deviceInfo().properties.deviceType) << std::endl;
    std::cout << "  API version:      " << version(deviceInfo().properties.apiVersion) << std::endl;
    std::cout << "  Driver version:   " << version(deviceInfo().properties.driverVersion) << std::endl;
    std::cout << std::endl;

    BenchmarkableTest::printStatistics();
}
}
