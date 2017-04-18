#include <base/vkx/ShaderModule.h>

#include <base/File.h>

#include <cassert>

namespace base {
namespace vkx {
ShaderModule::ShaderModule()
    : _device()
    , _handle()
{
}

ShaderModule::ShaderModule(const vk::Device& device, const std::string& shaderPath)
    : ShaderModule()
{
    _device = device;
    create(shaderPath);
}

ShaderModule::ShaderModule(ShaderModule&& other)
    : ShaderModule()
{
    std::swap(_device, other._device);
    std::swap(_handle, other._handle);
}

ShaderModule::~ShaderModule()
{
    destroy();
}

ShaderModule& ShaderModule::operator=(ShaderModule&& other)
{
    destroy();
    std::swap(_device, other._device);
    std::swap(_handle, other._handle);

    return *this;
}

ShaderModule::operator const vk::ShaderModule() const
{
    return _handle;
}

void ShaderModule::create(const std::string& path)
{
    assert(!_handle);

    std::string shaderCode = base::File::readBinary(path, true);
    _handle = _device.createShaderModule({{}, shaderCode.size(), reinterpret_cast<const uint32_t*>(shaderCode.data())});
}
void ShaderModule::destroy()
{
    if (_handle) {
        _device.destroyShaderModule(_handle);

        _device = {};
        _handle = {};
    }
}
}
}
