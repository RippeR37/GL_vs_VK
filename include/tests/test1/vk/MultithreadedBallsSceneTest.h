#pragma once

#include <base/vkx/ShaderModule.h>
#include <framework/VKTest.h>
#include <tests/common/Ball.h>
#include <tests/test1/BaseSimpleBallsSceneTest.h>

namespace tests {
namespace test_vk {
class MultithreadedBallsSceneTest : public BaseSimpleBallsSceneTest, public framework::VKTest
{
  public:
    MultithreadedBallsSceneTest();

    void setup() override;
    void run() override;
    void teardown() override;

  private:
    struct PerThreadSecondaryCommandPool
    {
        vk::CommandPool cmdPool;
        std::vector<vk::CommandBuffer> cmdBuffers;
    };

    void createVbo();
    void createCommandBuffers();
    void createSecondaryCommandBuffers();
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
    void destroySecondaryCommandBuffers();
    void destroyCommandBuffers();
    void destroyVbo();

    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStages() const;
    uint32_t getNextFrameIndex() const;

    void prepareSecondaryCommandBuffer(std::size_t frameIndex,
                                       std::size_t bufferIndex,
                                       std::size_t rangeFrom,
                                       std::size_t rangeTo);

    void prepareCommandBuffer(std::size_t frameIndex);
    void submitCommandBuffer(std::size_t frameIndex);
    void presentFrame(std::size_t frameIndex);

    unsigned int _threadWorkers;
    base::vkx::Buffer _vbo;
    vk::CommandPool _cmdPool;
    std::vector<vk::CommandBuffer> _cmdBuffers;
    std::vector<PerThreadSecondaryCommandPool> _threadCmdPools;
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
