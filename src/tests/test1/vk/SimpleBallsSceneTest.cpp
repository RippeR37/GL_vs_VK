#include <tests/test1/vk/SimpleBallsSceneTest.h>

#include <vulkan/vulkan.hpp>

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>

#include <GLFW/glfw3.h> // TODO: remove

namespace tests {
namespace test_vk {
SimpleBallsSceneTest::SimpleBallsSceneTest()
    : VKTest("SimpleBallsSceneTest")
{
}

void SimpleBallsSceneTest::setup()
{
    VKTest::setup();

    createCommandBuffers();
    createSemaphores();
    createFences();
    createRenderPass();
    createFramebuffers();
}

void SimpleBallsSceneTest::run()
{
    while (!window().shouldClose()) {
        auto frameIndex = getNextFrameIndex();

        prepareCommandBuffer(frameIndex);
        submitCommandBuffer(frameIndex);
        presentFrame(frameIndex);

        window().update();
    }
}

void SimpleBallsSceneTest::teardown()
{
    device().waitIdle();

    destroyFramebuffers();
    destroyRenderPass();
    destroyFences();
    destroySemaphores();
    destroyCommandBuffers();

    VKTest::teardown();
}

void SimpleBallsSceneTest::createCommandBuffers()
{
    _cmdPool = device().createCommandPool({vk::CommandPoolCreateFlagBits::eResetCommandBuffer, queues().familyIndex()});
    _cmdBuffers = device().allocateCommandBuffers(
        {_cmdPool, vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(window().swapchainImages().size())});
}

void SimpleBallsSceneTest::createSemaphores()
{
    _presentCompleteSemaphore = device().createSemaphore({});
    _renderCompleteSemaphore = device().createSemaphore({});
}

void SimpleBallsSceneTest::createFences()
{
    _fences.reserve(_cmdBuffers.size());
    for (const vk::CommandBuffer& cmdBuff : _cmdBuffers) {
        (void)cmdBuff; // unused
        _fences.push_back(device().createFence({vk::FenceCreateFlagBits::eSignaled}));
    }
}

void SimpleBallsSceneTest::createRenderPass()
{
    vk::AttachmentDescription attachment{{},
                                         window().swapchainImageFormat(),
                                         vk::SampleCountFlagBits::e1,
                                         vk::AttachmentLoadOp::eClear,
                                         vk::AttachmentStoreOp::eStore,
                                         vk::AttachmentLoadOp::eDontCare,
                                         vk::AttachmentStoreOp::eDontCare,
                                         vk::ImageLayout::eUndefined,
                                         vk::ImageLayout::ePresentSrcKHR};
    vk::AttachmentReference colorAttachment{0, vk::ImageLayout::eColorAttachmentOptimal};
    vk::SubpassDescription subpassDesc{
        {}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 1, &colorAttachment, nullptr, nullptr, 0, nullptr};
    // TODO: check if dependency doesn't have to be set here!

    _renderPass = device().createRenderPass({{}, 1, &attachment, 1, &subpassDesc, 0, nullptr});
}

void SimpleBallsSceneTest::createFramebuffers()
{
    _framebuffers.reserve(window().swapchainImages().size());
    for (const vk::ImageView& imageView : window().swapchainImageViews()) {
        vk::FramebufferCreateInfo framebufferInfo{{}, _renderPass, 1, &imageView, window().size().x, window().size().y,
                                                  1};
        _framebuffers.push_back(device().createFramebuffer(framebufferInfo));
    }
}

void SimpleBallsSceneTest::destroyFramebuffers()
{
    for (const vk::Framebuffer& framebuffer : _framebuffers) {
        device().destroyFramebuffer(framebuffer);
    }
}

void SimpleBallsSceneTest::destroyRenderPass()
{
    device().destroyRenderPass(_renderPass);
}

void SimpleBallsSceneTest::destroyFences()
{
    for (const vk::Fence& fence : _fences) {
        device().destroyFence(fence);
    }
}

void SimpleBallsSceneTest::destroySemaphores()
{
    device().destroySemaphore(_presentCompleteSemaphore);
    device().destroySemaphore(_renderCompleteSemaphore);
}

void SimpleBallsSceneTest::destroyCommandBuffers()
{
    device().destroyCommandPool(_cmdPool);
    _cmdBuffers.clear();
}

uint32_t SimpleBallsSceneTest::getNextFrameIndex() const
{
    auto nextFrameAcquireStatus =
        device().acquireNextImageKHR(window().swapchain(), UINT64_MAX, _presentCompleteSemaphore, {});

    if (nextFrameAcquireStatus.result != vk::Result::eSuccess) {
        throw std::system_error(nextFrameAcquireStatus.result, "Error during acquiring next frame index");
    }

    return nextFrameAcquireStatus.value;
}

void SimpleBallsSceneTest::prepareCommandBuffer(std::size_t frameIndex) const
{
    const vk::CommandBuffer& cmdBuffer = _cmdBuffers[frameIndex];

    device().waitForFences(1, &_fences[frameIndex], VK_TRUE, UINT64_MAX);
    device().resetFences(1, &_fences[frameIndex]);

    cmdBuffer.reset({});
    cmdBuffer.begin({{}, nullptr});

    double time = glfwGetTime();
    vk::ClearValue clearValue = vk::ClearColorValue{std::array<float, 4>{{
        (float)(0.5f + 0.5f * std::sin(time)), (float)(0.5f + 0.5f * std::cos(time)),
        (float)(0.5f + 0.5f * std::sin(time) * std::cos(time)), 1.0f,
    }}};

    vk::RenderPassBeginInfo renderPassInfo{
        _renderPass, _framebuffers[frameIndex], {{}, {window().size().x, window().size().y}}, 1, &clearValue};
    cmdBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
    cmdBuffer.endRenderPass();

    cmdBuffer.end();
}

void SimpleBallsSceneTest::submitCommandBuffer(std::size_t frameIndex) const
{
    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submits{1, &_presentCompleteSemaphore, &waitStage, 1, &_cmdBuffers[frameIndex],
                           1, &_renderCompleteSemaphore};
    queues().queue().submit(submits, _fences[frameIndex]);
}

void SimpleBallsSceneTest::presentFrame(std::size_t frameIndex) const
{
    vk::Result presentResult = vk::Result::eSuccess; // HACK: workaround for bug in unique_objects layer
    vk::PresentInfoKHR presentInfo{1, &_renderCompleteSemaphore, 1, &window().swapchain(), &frameIndex, &presentResult};
    vk::Result presentAllResults = queues().queue().presentKHR(presentInfo);

    if (presentAllResults != vk::Result::eSuccess) {
        throw std::system_error(presentAllResults, "Error during queuing frame presentation");
    }
}
}
}
