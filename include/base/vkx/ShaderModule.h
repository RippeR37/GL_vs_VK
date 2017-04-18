#pragma once

#include <vulkan/vulkan.hpp>

namespace base {
namespace vkx {
class ShaderModule
{
  public:
    ShaderModule();
    ShaderModule(const vk::Device& device, const std::string& shaderPath);
    ShaderModule(ShaderModule&& other);
    ShaderModule(const ShaderModule&) = delete;

    ~ShaderModule();

    ShaderModule& operator=(ShaderModule&& other);
    ShaderModule& operator=(const ShaderModule&) = delete;

    operator const vk::ShaderModule() const;

  private:
    void create(const std::string& path);
    void destroy();

    vk::Device _device;
    vk::ShaderModule _handle;
};
}
}
