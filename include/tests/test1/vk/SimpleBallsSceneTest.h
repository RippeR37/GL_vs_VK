#pragma once

#include <framework/VKTest.h>

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
    void createCommandBuffers();
    void createSemaphores();
    void createFences();
    void createRenderPass();
    void createFramebuffers();

    void destroyFramebuffers();
    void destroyRenderPass();
    void destroyFences();
    void destroySemaphores();
    void destroyCommandBuffers();

    uint32_t getNextFrameIndex() const;
    void prepareCommandBuffer(std::size_t frameIndex) const;
    void submitCommandBuffer(std::size_t frameIndex) const;
    void presentFrame(std::size_t frameIndex) const;

    vk::CommandPool _cmdPool;
    std::vector<vk::CommandBuffer> _cmdBuffers;
    std::vector<vk::Fence> _fences;
    vk::Semaphore _presentCompleteSemaphore;
    vk::Semaphore _renderCompleteSemaphore;
    vk::RenderPass _renderPass;
    std::vector<vk::Framebuffer> _framebuffers;
};
}
}
