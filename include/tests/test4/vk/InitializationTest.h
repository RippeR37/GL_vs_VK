#pragma once

#include <base/vkx/ShaderModule.h>
#include <framework/VKTest.h>
#include <tests/test4/BaseInitializationTest.h>

namespace tests {
namespace test_vk {
class InitializationTest : public BaseInitializationTest, public framework::VKTest
{
  public:
    InitializationTest();

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
    void createPipelineCache();
    void createPipeline();

    void destroyPipeline();
    void destroyPipelineCache();
    void destroyPipelineLayout();
    void destroyShaders();
    void destroyFramebuffers();
    void destroyRenderPass();
    void destroyFences();
    void destroySemaphores();
    void destroyCommandBuffers();
    void destroyVbo();

    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStages() const;
    uint32_t getNextFrameIndex() const;
    void prepareCommandBuffer(std::size_t frameIndex) const;
    void submitCommandBuffer(std::size_t frameIndex) const;
    void presentFrame(std::size_t frameIndex) const;

    base::vkx::Buffer _vbo;
    base::vkx::Buffer _stagingBuffer;
    vk::CommandPool _cmdPool;
    std::vector<vk::CommandBuffer> _cmdBuffers;
    vk::CommandBuffer _uploadCmdBuffer;
    std::vector<vk::Fence> _fences;
    vk::Semaphore _acquireSemaphore;
    vk::Semaphore _renderSemaphore;
    vk::Semaphore _uploadSemaphore;
    vk::RenderPass _renderPass;
    std::vector<vk::Framebuffer> _framebuffers;
    vk::DescriptorSetLayout _setLayout;
    vk::PipelineLayout _pipelineLayout;
    vk::PipelineCache _pipelineCache;
    vk::Pipeline _pipeline;
    base::vkx::ShaderModule _vertexModule;
    base::vkx::ShaderModule _fragmentModule;
};
}
}
