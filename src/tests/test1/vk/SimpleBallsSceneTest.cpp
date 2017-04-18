#include <tests/test1/vk/SimpleBallsSceneTest.h>

#include <tests/common/SphereVerticesGenerator.h>

#include <glm/vec4.hpp>
#include <vulkan/vulkan.hpp>

#include <array>
#include <iostream>
#include <stdexcept>
#include <vector>

namespace tests {
namespace test_vk {
SimpleBallsSceneTest::SimpleBallsSceneTest()
    : VKTest("SimpleBallsSceneTest")
{
}

void SimpleBallsSceneTest::setup()
{
    VKTest::setup();

    createVbo();
    createCommandBuffers();
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

    destroyPipeline();
    destroyPipelineLayout();
    destroyShaders();
    destroyFramebuffers();
    destroyRenderPass();
    destroyFences();
    destroySemaphores();
    destroyCommandBuffers();
    destroyVbo();

    VKTest::teardown();
}

void SimpleBallsSceneTest::createVbo()
{
    // Create VBO data
    common::SphereVerticesGenerator verticesGenerator{15, 15};
    _vertexCount = static_cast<uint32_t>(verticesGenerator.vertices.size());
    vk::DeviceSize vboSize = verticesGenerator.vertices.size() * sizeof(verticesGenerator.vertices.front());

    // Create VBO object
    vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eTransferSrc | vk::BufferUsageFlagBits::eTransferDst |
                                 vk::BufferUsageFlagBits::eVertexBuffer;
    vk::BufferCreateInfo vboInfo{{}, vboSize, usage, vk::SharingMode::eExclusive};
    _vbo = device().createBuffer(vboInfo);
    // TODO: move VBO to device-local memory with staging buffers!

    // Allocate VBO device memory
    {
        uint32_t memoryTypeIndex = 0;
        vk::MemoryPropertyFlags requiredMemoryProperties =
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
        for (uint32_t i = 0; i < deviceInfo().memory.memoryTypeCount; ++i) {
            if ((deviceInfo().memory.memoryTypes[i].propertyFlags & requiredMemoryProperties) ==
                requiredMemoryProperties) {
                memoryTypeIndex = i;
                break;
            }
        }

        // TODO: fix this to get right ammount of necessary memory allocated!
        vk::MemoryAllocateInfo deviceMemoryInfo{vboSize * 2, memoryTypeIndex};
        _vboMemory = device().allocateMemory(deviceMemoryInfo);
    }

    // Write VBO's memory
    void* memory = device().mapMemory(_vboMemory, 0, vboSize, {});
    std::memcpy(memory, verticesGenerator.vertices.data(), static_cast<std::size_t>(vboSize));
    device().unmapMemory(_vboMemory);

    // Bind VBO with it's memory
    device().bindBufferMemory(_vbo, _vboMemory, 0);
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

void SimpleBallsSceneTest::createShaders()
{
    _vertexModule = base::vkx::ShaderModule{device(), "resources/test1/shaders/vk_shader.vert.spv"};
    _fragmentModule = base::vkx::ShaderModule{device(), "resources/test1/shaders/vk_shader.frag.spv"};
}

void SimpleBallsSceneTest::createPipelineLayout()
{
    // Bindings
    std::vector<vk::DescriptorSetLayoutBinding> bindings;
    {
        // TODO: might want to use eUniformBufferDynamic to change this quickly?
        // vk::DescriptorSetLayoutBinding uboPosition{0, vk::DescriptorType::eUniformBuffer, 1,
        // vk::ShaderStageFlagBits::eVertex, nullptr}; bindings.push_back(uboPosition);
    }

    vk::DescriptorSetLayoutCreateInfo setLayoutInfo{{}, bindings.size(), bindings.data()};
    _setLayout = device().createDescriptorSetLayout(setLayoutInfo);

    // TODO: add push constants!
    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{{}, 1, &_setLayout, 0, nullptr};
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
                                                            vertexBindingDescriptions.size(),
                                                            vertexBindingDescriptions.data(),
                                                            vertexAttributeDescription.size(),
                                                            vertexAttributeDescription.data()};

    // Input assembly state
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{{}, vk::PrimitiveTopology::eTriangleList, VK_TRUE};

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
                                                                vk::CullModeFlagBits::eNone, // TODO: add culling
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
        {}, VK_FALSE, vk::LogicOp::eNoOp, 1, &colorBlendAttachmentState};

    // Pipeline creation
    vk::GraphicsPipelineCreateInfo pipelineInfo{{},
                                                shaderStages.size(),
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

void SimpleBallsSceneTest::destroyVbo()
{
    device().destroyBuffer(_vbo);
    device().freeMemory(_vboMemory);
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
    auto nextFrameAcquireStatus =
        device().acquireNextImageKHR(window().swapchain(), UINT64_MAX, _presentCompleteSemaphore, {});

    if (nextFrameAcquireStatus.result != vk::Result::eSuccess) {
        throw std::system_error(nextFrameAcquireStatus.result, "Error during acquiring next frame index");
    }

    return nextFrameAcquireStatus.value;
}

void SimpleBallsSceneTest::prepareCommandBuffer(std::size_t frameIndex) const
{
    static const vk::ClearValue clearValue = vk::ClearColorValue{std::array<float, 4>{{0.0f, 0.0f, 0.0f, 1.0f}}};
    const vk::CommandBuffer& cmdBuffer = _cmdBuffers[frameIndex];

    device().waitForFences(1, &_fences[frameIndex], VK_TRUE, UINT64_MAX);
    device().resetFences(1, &_fences[frameIndex]);

    cmdBuffer.reset({});
    cmdBuffer.begin({{}, nullptr});
    {
        vk::RenderPassBeginInfo renderPassInfo{
            _renderPass, _framebuffers[frameIndex], {{}, {window().size().x, window().size().y}}, 1, &clearValue};
        cmdBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, _pipeline);
        // cmdBuffer.bindDescriptorSets(bindPoint, _pipelineLayout, firstSet, descriptorSets, dynamicOffsts);
        cmdBuffer.bindVertexBuffers(0, {{_vbo}}, {{0}});

        for (int i = 0; i < 10000; ++i) {
            cmdBuffer.draw(_vertexCount, 1, 0, 0);
        }

        cmdBuffer.endRenderPass();
    }
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
