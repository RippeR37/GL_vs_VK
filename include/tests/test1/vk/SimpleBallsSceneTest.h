#pragma once

#include <framework/VKTest.h>

#include <base/vkx/ShaderModule.h>
#include <tests/common/Ball.h>

#include <tuple>

namespace tests {
namespace test_vk {
class SimpleBallsSceneTest : public framework::VKTest
{
  public:
    SimpleBallsSceneTest();

    void setup() override;
    void run() override;
    void teardown() override;

  private:
    void createVbo();
    void createCommandBuffers();
    void createSemaphores();
    void createFences();
    void createRenderPass();
    void createFramebuffers();
    void createShaders();
    void createPipelineLayout();
    void createPipeline();

    void destroyPipeline();
    void destroyPipelineLayout();
    void destroyShaders();
    void destroyFramebuffers();
    void destroyRenderPass();
    void destroyFences();
    void destroySemaphores();
    void destroyCommandBuffers();
    void destroyVbo();

    std::tuple<vk::Buffer, vk::DeviceMemory> copyToDeviceLocalMemory(const vk::Buffer& srcBuffer,
                                                                     vk::DeviceSize size) const;
    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStages() const;
    uint32_t getNextFrameIndex() const;
    void prepareCommandBuffer(std::size_t frameIndex) const;
    void submitCommandBuffer(std::size_t frameIndex) const;
    void presentFrame(std::size_t frameIndex) const;

    void initState();
    void updateState();

    std::vector<common::Ball> _balls;
    vk::Buffer _vbo;
    vk::DeviceMemory _vboMemory;
    uint32_t _vertexCount;
    vk::CommandPool _cmdPool;
    std::vector<vk::CommandBuffer> _cmdBuffers;
    std::vector<vk::Fence> _fences;
    mutable std::size_t _semaphoreIndex;
    std::vector<vk::Semaphore> _acquireSemaphores;
    std::vector<vk::Semaphore> _renderSemaphores;
    vk::RenderPass _renderPass;
    std::vector<vk::Framebuffer> _framebuffers;
    vk::DescriptorSetLayout _setLayout;
    vk::PipelineLayout _pipelineLayout;
    vk::Pipeline _pipeline;
    base::vkx::ShaderModule _vertexModule;
    base::vkx::ShaderModule _fragmentModule;
};
}
}
