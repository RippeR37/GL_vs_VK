#include <tests/test2/vk/MultithreadedTerrainSceneTest.h>

#include <base/ScopedTimer.h>
#include <base/vkx/Utils.h>

#include <glm/vec4.hpp>
#include <vulkan/vulkan.hpp>

#include <thread>
#include <vector>

namespace tests {
namespace test_vk {
MultithreadedTerrainSceneTest::MultithreadedTerrainSceneTest(bool benchmarkMode, float benchmarkTime)
    : BaseTerrainSceneTest()
    , VKTest("MultithreadedTerrainSceneTest", benchmarkMode, benchmarkTime)
    , _semaphoreIndex(0u)
{
}

void MultithreadedTerrainSceneTest::setup()
{
    VKTest::setup();

    createCommandBuffers();
    createSecondaryCommandBuffers();
    createVbo();
    createIbo();
    createSemaphores();
    createFences();
    createRenderPass();
    createFramebuffers();
    createShaders();
    createPipelineLayout();
    createPipeline();
}

void MultithreadedTerrainSceneTest::run()
{
    while (!window().shouldClose()) {
        TIME_RESET("Frame times:");

        auto frameIndex = getNextFrameIndex();

        updateTestState(static_cast<float>(window().frameTime()));
        prepareCommandBuffer(frameIndex);
        submitCommandBuffer(frameIndex);
        presentFrame(frameIndex);

        window().update();

        if (processFrameTime(window().frameTime())) {
            break; // Benchmarking is complete
        }
    }
}

void MultithreadedTerrainSceneTest::teardown()
{
    device().waitIdle();

    destroyPipeline();
    destroyPipelineLayout();
    destroyShaders();
    destroyFramebuffers();
    destroyRenderPass();
    destroyFences();
    destroySemaphores();
    destroyIbo();
    destroyVbo();
    destroySecondaryCommandBuffers();
    destroyCommandBuffers();

    VKTest::teardown();
}

void MultithreadedTerrainSceneTest::createCommandBuffers()
{
    vk::CommandPoolCreateFlags cmdPoolFlags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    _cmdPool = device().createCommandPool({cmdPoolFlags, queues().familyIndex()});
    _cmdBuffers = device().allocateCommandBuffers(
        {_cmdPool, vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(window().swapchainImages().size())});
}

void MultithreadedTerrainSceneTest::createSecondaryCommandBuffers()
{
    // TODO: think on how to extend this for arbitrary std::thread::hardware_concurrency() value
    static const std::size_t threads = 4;

    _threadCmdPools.resize(threads);
    for (auto& sndCmdPool : _threadCmdPools) {
        vk::CommandPoolCreateFlags cmdPoolFlags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        sndCmdPool.cmdPool = device().createCommandPool({cmdPoolFlags, queues().familyIndex()});
        sndCmdPool.cmdBuffers =
            device().allocateCommandBuffers({sndCmdPool.cmdPool, vk::CommandBufferLevel::eSecondary,
                                             static_cast<uint32_t>(window().swapchainImages().size())});
    }
}

void MultithreadedTerrainSceneTest::createVbo()
{
    vk::DeviceSize size = terrain().vertices().size() * sizeof(terrain().vertices().front());
    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eVertexBuffer;

    base::vkx::Buffer stagingBuffer = memory().createStagingBuffer(size);
    {
        auto vboMemory = device().mapMemory(stagingBuffer.memory, stagingBuffer.offset, stagingBuffer.size, {});
        std::memcpy(vboMemory, terrain().vertices().data(), static_cast<std::size_t>(stagingBuffer.size));
        device().unmapMemory(stagingBuffer.memory);
    }
    _vbo = memory().copyToDeviceLocalMemory(stagingBuffer, usage, _cmdBuffers.front(), queues().queue());

    memory().destroyBuffer(stagingBuffer);
}

void MultithreadedTerrainSceneTest::createIbo()
{
    vk::DeviceSize size = terrain().indices().size() * sizeof(terrain().indices().front());
    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eIndexBuffer;

    base::vkx::Buffer stagingBuffer = memory().createStagingBuffer(size);
    {
        auto iboMemory = device().mapMemory(stagingBuffer.memory, stagingBuffer.offset, stagingBuffer.size, {});
        std::memcpy(iboMemory, terrain().indices().data(), static_cast<std::size_t>(stagingBuffer.size));
        device().unmapMemory(stagingBuffer.memory);
    }
    _ibo = memory().copyToDeviceLocalMemory(stagingBuffer, usage, _cmdBuffers.front(), queues().queue());

    memory().destroyBuffer(stagingBuffer);
}

void MultithreadedTerrainSceneTest::createSemaphores()
{
    for (std::size_t i = 0; i < window().swapchainImages().size(); ++i) {
        _acquireSemaphores.push_back(device().createSemaphore({}));
        _renderSemaphores.push_back(device().createSemaphore({}));
    }
}

void MultithreadedTerrainSceneTest::createFences()
{
    _fences.resize(_cmdBuffers.size());
    for (vk::Fence& fence : _fences) {
        fence = device().createFence({vk::FenceCreateFlagBits::eSignaled});
    }
}

void MultithreadedTerrainSceneTest::createRenderPass()
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

void MultithreadedTerrainSceneTest::createFramebuffers()
{
    _framebuffers.reserve(window().swapchainImages().size());
    for (const vk::ImageView& imageView : window().swapchainImageViews()) {
        vk::FramebufferCreateInfo framebufferInfo{{}, _renderPass, 1, &imageView, window().size().x, window().size().y,
                                                  1};
        _framebuffers.push_back(device().createFramebuffer(framebufferInfo));
    }
}

void MultithreadedTerrainSceneTest::createShaders()
{
    _vertexModule = base::vkx::ShaderModule{device(), "resources/test2/shaders/vk_shader.vert.spv"};
    _fragmentModule = base::vkx::ShaderModule{device(), "resources/test2/shaders/vk_shader.frag.spv"};
}

void MultithreadedTerrainSceneTest::createPipelineLayout()
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    vk::DescriptorSetLayoutCreateInfo setLayoutInfo{{}, static_cast<uint32_t>(bindings.size()), bindings.data()};
    _setLayout = device().createDescriptorSetLayout(setLayoutInfo);

    vk::PushConstantRange pushConstantRanges[] = {
        {vk::ShaderStageFlagBits::eVertex, 0, sizeof(glm::mat4)}, // MVP matrix
    };

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{{}, 1, &_setLayout, 1, pushConstantRanges};
    _pipelineLayout = device().createPipelineLayout(pipelineLayoutInfo);
}

void MultithreadedTerrainSceneTest::createPipeline()
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
                                                                vk::PolygonMode::eLine,
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

void MultithreadedTerrainSceneTest::destroyPipeline()
{
    device().destroyPipeline(_pipeline);
}

void MultithreadedTerrainSceneTest::destroyPipelineLayout()
{
    device().destroyPipelineLayout(_pipelineLayout);
    device().destroyDescriptorSetLayout(_setLayout);
}

void MultithreadedTerrainSceneTest::destroyShaders()
{
    _vertexModule = {};
    _fragmentModule = {};
}

void MultithreadedTerrainSceneTest::destroyFramebuffers()
{
    for (const vk::Framebuffer& framebuffer : _framebuffers) {
        device().destroyFramebuffer(framebuffer);
    }
}

void MultithreadedTerrainSceneTest::destroyRenderPass()
{
    device().destroyRenderPass(_renderPass);
}

void MultithreadedTerrainSceneTest::destroyFences()
{
    for (const vk::Fence& fence : _fences) {
        device().destroyFence(fence);
    }
    _fences.clear();
}

void MultithreadedTerrainSceneTest::destroySemaphores()
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

void MultithreadedTerrainSceneTest::destroyIbo()
{
    memory().destroyBuffer(_ibo);
}

void MultithreadedTerrainSceneTest::destroyVbo()
{
    memory().destroyBuffer(_vbo);
}

void MultithreadedTerrainSceneTest::destroySecondaryCommandBuffers()
{
    for (auto& threadCmdPool : _threadCmdPools) {
        device().destroyCommandPool(threadCmdPool.cmdPool);
        threadCmdPool.cmdBuffers.clear();
    }
    _threadCmdPools.clear();
}

void MultithreadedTerrainSceneTest::destroyCommandBuffers()
{
    device().destroyCommandPool(_cmdPool);
    _cmdBuffers.clear();
}

std::vector<vk::PipelineShaderStageCreateInfo> MultithreadedTerrainSceneTest::getShaderStages() const
{
    std::vector<vk::PipelineShaderStageCreateInfo> stages;

    vk::PipelineShaderStageCreateInfo vertexStage{{}, vk::ShaderStageFlagBits::eVertex, _vertexModule, "main", nullptr};
    stages.push_back(vertexStage);

    vk::PipelineShaderStageCreateInfo fragmentStage{
        {}, vk::ShaderStageFlagBits::eFragment, _fragmentModule, "main", nullptr};
    stages.push_back(fragmentStage);

    return stages;
}

uint32_t MultithreadedTerrainSceneTest::getNextFrameIndex() const
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

void MultithreadedTerrainSceneTest::prepareSecondaryCommandBuffer(std::size_t threadIndex, std::size_t frameIndex) const
{
    TIME_IT("CmdBuffer (secondary) building");

    // Update secondary command buffer
    const vk::CommandBuffer& cmdBuffer = _threadCmdPools[threadIndex].cmdBuffers[frameIndex];

    cmdBuffer.reset({});
    vk::CommandBufferInheritanceInfo inheritanceInfo{_renderPass, 0, _framebuffers[frameIndex], VK_FALSE, {}, {}};
    cmdBuffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eRenderPassContinue |
                                                   vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
                                               &inheritanceInfo});
    {
        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);
        cmdBuffer.bindVertexBuffers(0, {{_vbo.buffer}}, {{0}});
        cmdBuffer.bindIndexBuffer(_ibo.buffer, 0, vk::IndexType::eUint32);

        {
            glm::mat4 MVP = base::vkx::fixGLMatrix(currentMVP()); // flip Y and fix Z axes
            cmdBuffer.pushConstants(_pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(MVP), &MVP);
        }

        {
            auto indexSize = sizeof(terrain().indices().front());
            auto renderChunk = [&cmdBuffer, indexSize](std::size_t count, std::ptrdiff_t offset) {
                cmdBuffer.drawIndexed(static_cast<uint32_t>(count), 1, static_cast<uint32_t>(offset / indexSize), 0, 0);
            };
            // TODO: think on how to extend this beyond 4-threads
            terrain().executeLoD(currentPosition(), renderChunk, threadIndex);
        }
    }
    cmdBuffer.end();
}

void MultithreadedTerrainSceneTest::prepareCommandBuffer(std::size_t frameIndex) const
{
    static const vk::ClearValue clearValue = vk::ClearColorValue{std::array<float, 4>{{0.0f, 0.0f, 0.0f, 1.0f}}};
    vk::RenderPassBeginInfo renderPassInfo{
        _renderPass, _framebuffers[frameIndex], {{}, {window().size().x, window().size().y}}, 1, &clearValue};

    {
        TIME_IT("Fence waiting");
        device().waitForFences(1, &_fences[frameIndex], VK_FALSE, UINT64_MAX);
        device().resetFences(1, &_fences[frameIndex]);
    }

    {
        // Multithreaded version
        TIME_IT("CmdBuffer building");
        const vk::CommandBuffer& cmdBuffer = _cmdBuffers[frameIndex];
        cmdBuffer.reset({});
        cmdBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr});
        {
            cmdBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);

            std::vector<std::thread> threads(_threadCmdPools.size());
            std::vector<vk::CommandBuffer> threadedCommandBuffers;
            for (std::size_t threadIndex = 0; threadIndex < _threadCmdPools.size(); ++threadIndex) {
                threadedCommandBuffers.push_back(_threadCmdPools[threadIndex].cmdBuffers[frameIndex]);

                threads[threadIndex] =
                    std::move(std::thread(&MultithreadedTerrainSceneTest::prepareSecondaryCommandBuffer, this,
                                          threadIndex, frameIndex));
            }
            for (auto& thread : threads) {
                thread.join();
            }

            cmdBuffer.executeCommands(threadedCommandBuffers);
            cmdBuffer.endRenderPass();
        }
        cmdBuffer.end();
    }
}

void MultithreadedTerrainSceneTest::submitCommandBuffer(std::size_t frameIndex) const
{
    TIME_IT("CmdBuffer submition");

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submits{1, &_acquireSemaphores[_semaphoreIndex], &waitStage, 1, &_cmdBuffers[frameIndex],
                           1, &_renderSemaphores[_semaphoreIndex]};
    queues().queue().submit(submits, _fences[frameIndex]);
}

void MultithreadedTerrainSceneTest::presentFrame(std::size_t frameIndex) const
{
    TIME_IT("Frame presentation");

    uint32_t imageIndex = static_cast<uint32_t>(frameIndex);
    vk::PresentInfoKHR presentInfo{1,      &_renderSemaphores[_semaphoreIndex], 1, &window().swapchain(), &imageIndex,
                                   nullptr};
    queues().queue().presentKHR(presentInfo);
}
}
}
