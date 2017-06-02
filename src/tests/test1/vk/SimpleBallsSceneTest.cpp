#include <tests/test1/vk/SimpleBallsSceneTest.h>

#include <base/ScopedTimer.h>

#include <glm/vec4.hpp>
#include <vulkan/vulkan.hpp>

#include <array>
#include <stdexcept>
#include <vector>

namespace tests {
namespace test_vk {
SimpleBallsSceneTest::SimpleBallsSceneTest(bool benchmarkMode, float benchmarkTime)
    : BaseBallsSceneTest()
    , VKTest("SimpleBallsSceneTest", benchmarkMode, benchmarkTime)
    , _semaphoreIndex(0u)
{
}

void SimpleBallsSceneTest::setup()
{
    VKTest::setup();
    initTestState();

    createCommandBuffers();
    createVbo();
    createSemaphores();
    createFences();
    createRenderPass();
    createFramebuffers();
    createShaders();
    createPipelineLayout();
    createPipeline();
}

void SimpleBallsSceneTest::run()
{
    while (!window().shouldClose()) {
        TIME_RESET("Frame times:");

        auto frameIndex = getNextFrameIndex();
        {
            TIME_IT("State update");
            updateTestState(static_cast<float>(window().frameTime()));
        }
        prepareCommandBuffer(frameIndex);
        submitCommandBuffer(frameIndex);
        presentFrame(frameIndex);

        window().update();

        if (processFrameTime(window().frameTime())) {
            break; // Benchmarking is complete
        }
    }
}

void SimpleBallsSceneTest::teardown()
{
    device().waitIdle();

    destroyPipeline();
    destroyPipelineLayout();
    destroyShaders();
    destroyFramebuffers();
    destroyRenderPass();
    destroyFences();
    destroySemaphores();
    destroyVbo();
    destroyCommandBuffers();

    destroyTestState();
    VKTest::teardown();
}

void SimpleBallsSceneTest::createCommandBuffers()
{
    vk::CommandPoolCreateFlags cmdPoolFlags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    _cmdPool = device().createCommandPool({cmdPoolFlags, queues().familyIndex()});
    _cmdBuffers = device().allocateCommandBuffers(
        {_cmdPool, vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(window().swapchainImages().size())});
}

void SimpleBallsSceneTest::createVbo()
{
    vk::DeviceSize size = vertices().size() * sizeof(vertices().front());
    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eVertexBuffer;

    base::vkx::Buffer stagingBuffer = memory().createStagingBuffer(size);
    {
        auto vboMemory = device().mapMemory(stagingBuffer.memory, stagingBuffer.offset, stagingBuffer.size, {});
        std::memcpy(vboMemory, vertices().data(), static_cast<std::size_t>(stagingBuffer.size));
        device().unmapMemory(stagingBuffer.memory);
    }
    _vbo = memory().copyToDeviceLocalMemory(stagingBuffer, usage, _cmdBuffers.front(), queues().queue());

    memory().destroyBuffer(stagingBuffer);
}

void SimpleBallsSceneTest::createSemaphores()
{
    for (std::size_t i = 0; i < window().swapchainImages().size(); ++i) {
        _acquireSemaphores.push_back(device().createSemaphore({}));
        _renderSemaphores.push_back(device().createSemaphore({}));
    }
}

void SimpleBallsSceneTest::createFences()
{
    _fences.resize(_cmdBuffers.size());
    for (vk::Fence& fence : _fences) {
        fence = device().createFence({vk::FenceCreateFlagBits::eSignaled});
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

    vk::RenderPassCreateInfo renderPassInfo{{}, 1, &attachment, 1, &subpassDesc, 0, nullptr};
    _renderPass = device().createRenderPass(renderPassInfo);
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

void SimpleBallsSceneTest::createShaders()
{
    _vertexModule = base::vkx::ShaderModule{device(), "resources/test1/shaders/vk_shader.vert.spv"};
    _fragmentModule = base::vkx::ShaderModule{device(), "resources/test1/shaders/vk_shader.frag.spv"};
}

void SimpleBallsSceneTest::createPipelineLayout()
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    vk::DescriptorSetLayoutCreateInfo setLayoutInfo{{}, static_cast<uint32_t>(bindings.size()), bindings.data()};
    _setLayout = device().createDescriptorSetLayout(setLayoutInfo);

    vk::PushConstantRange pushConstantRanges[] = {
        {vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::vec4)}, // position
        {vk::ShaderStageFlagBits::eFragment, sizeof(glm::vec4), sizeof(glm::vec4)} // color
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{{}, 1, &_setLayout, 2, &pushConstantRanges[0]};
    _pipelineLayout = device().createPipelineLayout(pipelineLayoutInfo);
}

void SimpleBallsSceneTest::createPipeline()
{
    // Shader stages
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = getShaderStages();

    // Vertex input state
    std::vector<vk::VertexInputBindingDescription> vertexBindingDescriptions{
        {0, sizeof(glm::vec4), vk::VertexInputRate::eVertex} // Binding #0 - vertex input data
    };
    std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescription{
        {0, 0, vk::Format::eR32G32B32A32Sfloat, 0} // Attribute #0 (from binding #0) - vec4
    };
    vk::PipelineVertexInputStateCreateInfo vertexInputState{{},
                                                            static_cast<uint32_t>(vertexBindingDescriptions.size()),
                                                            vertexBindingDescriptions.data(),
                                                            static_cast<uint32_t>(vertexAttributeDescription.size()),
                                                            vertexAttributeDescription.data()};

    // Input assembly state
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{{}, vk::PrimitiveTopology::eTriangleList, VK_FALSE};

    // Viewport state
    vk::Viewport viewport{0.0f, 0.0f, static_cast<float>(window().size().x), static_cast<float>(window().size().y),
                          0.0f, 1.0f};
    vk::Rect2D scissor{{0, 0}, {static_cast<uint32_t>(window().size().x), static_cast<uint32_t>(window().size().y)}};
    vk::PipelineViewportStateCreateInfo viewportState{{}, 1, &viewport, 1, &scissor};

    // Rasterization state
    vk::PipelineRasterizationStateCreateInfo rasterizationState{{},
                                                                VK_FALSE,
                                                                VK_FALSE,
                                                                vk::PolygonMode::eFill,
                                                                vk::CullModeFlagBits::eNone,
                                                                vk::FrontFace::eCounterClockwise,
                                                                VK_FALSE,
                                                                0.0f,
                                                                0.0f,
                                                                0.0f,
                                                                1.0f};

    // Multisample state
    vk::PipelineMultisampleStateCreateInfo multisampleState{
        {}, vk::SampleCountFlagBits::e1, VK_FALSE, 0.0f, nullptr, VK_FALSE, VK_FALSE};

    // ColorBlend state
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{VK_FALSE};
    colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendStateCreateInfo colorBlendState{
        {}, VK_FALSE, vk::LogicOp::eClear, 1, &colorBlendAttachmentState};

    // Pipeline creation
    vk::GraphicsPipelineCreateInfo pipelineInfo{{},
                                                static_cast<uint32_t>(shaderStages.size()),
                                                shaderStages.data(),
                                                &vertexInputState,
                                                &inputAssemblyState,
                                                nullptr,
                                                &viewportState,
                                                &rasterizationState,
                                                &multisampleState,
                                                nullptr,
                                                &colorBlendState,
                                                nullptr,
                                                _pipelineLayout,
                                                _renderPass,
                                                0};
    _pipeline = device().createGraphicsPipeline({}, pipelineInfo);
}

void SimpleBallsSceneTest::destroyPipeline()
{
    device().destroyPipeline(_pipeline);
}

void SimpleBallsSceneTest::destroyPipelineLayout()
{
    device().destroyPipelineLayout(_pipelineLayout);
    device().destroyDescriptorSetLayout(_setLayout);
}

void SimpleBallsSceneTest::destroyShaders()
{
    _vertexModule = {};
    _fragmentModule = {};
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
    _fences.clear();
}

void SimpleBallsSceneTest::destroySemaphores()
{
    for (const auto& acquireSemaphore : _acquireSemaphores) {
        device().destroySemaphore(acquireSemaphore);
    }
    for (const auto& renderSemaphore : _renderSemaphores) {
        device().destroySemaphore(renderSemaphore);
    }
    _acquireSemaphores.clear();
    _renderSemaphores.clear();
}

void SimpleBallsSceneTest::destroyVbo()
{
    memory().destroyBuffer(_vbo);
}

void SimpleBallsSceneTest::destroyCommandBuffers()
{
    device().destroyCommandPool(_cmdPool);
    _cmdBuffers.clear();
}

std::vector<vk::PipelineShaderStageCreateInfo> SimpleBallsSceneTest::getShaderStages() const
{
    std::vector<vk::PipelineShaderStageCreateInfo> stages;

    vk::PipelineShaderStageCreateInfo vertexStage{{}, vk::ShaderStageFlagBits::eVertex, _vertexModule, "main", nullptr};
    stages.push_back(vertexStage);

    vk::PipelineShaderStageCreateInfo fragmentStage{
        {}, vk::ShaderStageFlagBits::eFragment, _fragmentModule, "main", nullptr};
    stages.push_back(fragmentStage);

    return stages;
}

uint32_t SimpleBallsSceneTest::getNextFrameIndex() const
{
    TIME_IT("Frame image acquisition");

    _semaphoreIndex = (_semaphoreIndex + 1) % _acquireSemaphores.size();
    auto nextFrameAcquireStatus =
        device().acquireNextImageKHR(window().swapchain(), UINT64_MAX, _acquireSemaphores[_semaphoreIndex], {});

    if (nextFrameAcquireStatus.result != vk::Result::eSuccess) {
        throw std::system_error(nextFrameAcquireStatus.result, "Error during acquiring next frame index");
    }

    return nextFrameAcquireStatus.value;
}

void SimpleBallsSceneTest::prepareCommandBuffer(std::size_t frameIndex) const
{
    static const vk::ClearValue clearValue = vk::ClearColorValue{std::array<float, 4>{{0.0f, 0.0f, 0.0f, 1.0f}}};
    const vk::CommandBuffer& cmdBuffer = _cmdBuffers[frameIndex];

    vk::RenderPassBeginInfo renderPassInfo{
        _renderPass, _framebuffers[frameIndex], {{}, {window().size().x, window().size().y}}, 1, &clearValue};

    {
        TIME_IT("Fence waiting");
        device().waitForFences(1, &_fences[frameIndex], VK_FALSE, UINT64_MAX);
        device().resetFences(1, &_fences[frameIndex]);
    }

    {
        TIME_IT("CmdBuffer building");
        cmdBuffer.reset({});
        cmdBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr});
        {
            cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);
            cmdBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);
            cmdBuffer.bindVertexBuffers(0, {{_vbo.buffer}}, {{0}});

            for (const auto& ball : balls()) {
                cmdBuffer.pushConstants(_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::vec4),
                                        &ball.position);
                cmdBuffer.pushConstants(_pipelineLayout, vk::ShaderStageFlagBits::eFragment, sizeof(glm::vec4),
                                        sizeof(glm::vec4), &ball.color);
                cmdBuffer.draw(static_cast<uint32_t>(vertices().size()), 1, 0, 0);
            }

            cmdBuffer.endRenderPass();
        }
        cmdBuffer.end();
    }
}

void SimpleBallsSceneTest::submitCommandBuffer(std::size_t frameIndex) const
{
    TIME_IT("CmdBuffer submition");

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submits{1, &_acquireSemaphores[_semaphoreIndex], &waitStage, 1, &_cmdBuffers[frameIndex],
                           1, &_renderSemaphores[_semaphoreIndex]};
    queues().queue().submit(submits, _fences[frameIndex]);
}

void SimpleBallsSceneTest::presentFrame(std::size_t frameIndex) const
{
    TIME_IT("Frame presentation");

    uint32_t imageIndex = static_cast<uint32_t>(frameIndex);
    vk::PresentInfoKHR presentInfo{1,      &_renderSemaphores[_semaphoreIndex], 1, &window().swapchain(), &imageIndex,
                                   nullptr};
    queues().queue().presentKHR(presentInfo);
}
}
}
