#include <tests/test3/vk/MultithreadedShadowMappingSceneTest.h>

#include <base/ScopedTimer.h>
#include <base/vkx/Utils.h>

#include <glm/vec4.hpp>
#include <vulkan/vulkan.hpp>

#include <iostream>
#include <stdexcept>
#include <thread>

namespace {
const std::vector<vk::Format> kDepthFormatCandidates{vk::Format::eD24UnormS8Uint, vk::Format::eD32Sfloat,
                                                     vk::Format::eD16Unorm, vk::Format::eD16UnormS8Uint};
const std::vector<vk::Format> kDepthFormatsWithStencilAspect{vk::Format::eD24UnormS8Uint, vk::Format::eD16UnormS8Uint};

vk::Format findOptimalTilingDepthFormat(const vk::PhysicalDevice& physicalDevice)
{
    for (auto format : kDepthFormatCandidates) {
        vk::FormatProperties properties = physicalDevice.getFormatProperties(format);
        vk::FormatFeatureFlags wantedFeatures = vk::FormatFeatureFlagBits::eDepthStencilAttachment;

        if ((properties.optimalTilingFeatures & wantedFeatures) == wantedFeatures) {
            return format;
        }
    }

    throw std::system_error(vk::Result::eErrorFormatNotSupported, "None of cadidate depth formats are supported");
}

vk::ImageAspectFlags getImageDepthFormatAspect(const vk::Format& format)
{
    vk::ImageAspectFlags result = vk::ImageAspectFlagBits::eDepth;

    for (const auto& withStencilFormat : kDepthFormatsWithStencilAspect) {
        if (format == withStencilFormat) {
            result |= vk::ImageAspectFlagBits::eStencil;
        }
    }

    return result;
}
}

namespace tests {
namespace test_vk {
MultithreadedShadowMappingSceneTest::MultithreadedShadowMappingSceneTest(bool benchmarkMode, float benchmarkTime)
    : BaseShadowMappingSceneTest()
    , VKTest("MultithreadedShadowMappingSceneTest", benchmarkMode, benchmarkTime)
    , _semaphoreIndex(0u)
{
}

void MultithreadedShadowMappingSceneTest::setup()
{
    VKTest::setup();

    createCommandBuffers();
    createSecondaryCommandBuffers();
    createVbos();
    createSemaphores();
    createFences();

    prepareShadowmapPass();
    prepareRenderPass();
}

void MultithreadedShadowMappingSceneTest::run()
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

void MultithreadedShadowMappingSceneTest::teardown()
{
    device().waitIdle();

    destroyPass(_shadowmapPass);
    destroyPass(_renderPass);

    destroyFences();
    destroySemaphores();
    destroyVbos();
    destroySecondaryCommandBuffers();
    destroyCommandBuffers();

    VKTest::teardown();
}

void MultithreadedShadowMappingSceneTest::prepareShadowmapPass()
{
    _shadowmapPass.depthBuffer = createDepthBuffer(shadowmapSize(), vk::ImageUsageFlagBits::eDepthStencilAttachment |
                                                                        vk::ImageUsageFlagBits::eSampled);
    _shadowmapPass.renderPass = createShadowmapRenderPass();
    _shadowmapPass.framebuffers =
        createFramebuffers(_shadowmapPass.renderPass, std::vector<vk::ImageView>(window().swapchainImages().size()),
                           _shadowmapPass.depthBuffer.view, shadowmapSize());
    _shadowmapPass.program = createProgram("resources/test3/shaders/vk/shadowmap");
    _shadowmapPass.descriptorSetLayout = createShadowmapDescriptorSetLayout();
    _shadowmapPass.pipelineLayout = createPipelineLayout({_shadowmapPass.descriptorSetLayout}, false);
    _shadowmapPass.pipeline = createPipeline(_shadowmapPass.program, shadowmapSize(), _shadowmapPass.pipelineLayout,
                                             _shadowmapPass.renderPass, false);
}

void MultithreadedShadowMappingSceneTest::prepareRenderPass()
{
    _renderPass.depthBuffer = createDepthBuffer(window().size(), vk::ImageUsageFlagBits::eDepthStencilAttachment);
    _renderPass.renderPass = createRenderRenderPass();
    _renderPass.framebuffers = createFramebuffers(_renderPass.renderPass, window().swapchainImageViews(),
                                                  _renderPass.depthBuffer.view, window().size());
    _renderPass.program = createProgram("resources/test3/shaders/vk/render");
    _renderPass.descriptorSetLayout = createRenderDescriptorSetLayout();
    _renderPass.descriptorPool = createRenderDescriptorPool();
    _renderPass.descriptorSet = createRenderDescriptorSet(_renderPass.descriptorPool, _renderPass.descriptorSetLayout);
    _renderPass.sampler = createRenderShadowmapSampler();
    _renderPass.pipelineLayout = createPipelineLayout({_renderPass.descriptorSetLayout}, true);
    _renderPass.pipeline =
        createPipeline(_renderPass.program, window().size(), _renderPass.pipelineLayout, _renderPass.renderPass, true);

    setRenderDescriptorSet(_renderPass.descriptorSet);
}

void MultithreadedShadowMappingSceneTest::destroyPass(VkPass& pass)
{
    destroyPipeline(pass.pipeline);
    destroyPipelineLayout(pass.pipelineLayout);
    destroySampler(pass.sampler);
    destroyDescriptorSetLayout(pass.descriptorSetLayout);
    destroyDescriptorPool(pass.descriptorPool);
    destroyProgram(pass.program);
    destroyFramebuffers(pass.framebuffers);
    destroyRenderPass(pass.renderPass);
    destroyDepthBuffer(pass.depthBuffer);
}

void MultithreadedShadowMappingSceneTest::createCommandBuffers()
{
    vk::CommandPoolCreateFlags cmdPoolFlags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    _cmdPool = device().createCommandPool({cmdPoolFlags, queues().familyIndex()});
    _cmdBuffers = device().allocateCommandBuffers(
        {_cmdPool, vk::CommandBufferLevel::ePrimary, static_cast<uint32_t>(window().swapchainImages().size())});
}

void MultithreadedShadowMappingSceneTest::createSecondaryCommandBuffers()
{
    _threadCmdPools.resize(std::thread::hardware_concurrency());
    for (auto& sndCmdPool : _threadCmdPools) {
        vk::CommandPoolCreateFlags cmdPoolFlags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        sndCmdPool.cmdPool = device().createCommandPool({cmdPoolFlags, queues().familyIndex()});
        sndCmdPool.shadowmapCmdBuffers =
            device().allocateCommandBuffers({sndCmdPool.cmdPool, vk::CommandBufferLevel::eSecondary,
                                             static_cast<uint32_t>(window().swapchainImages().size())});
        sndCmdPool.renderCmdBuffers =
            device().allocateCommandBuffers({sndCmdPool.cmdPool, vk::CommandBufferLevel::eSecondary,
                                             static_cast<uint32_t>(window().swapchainImages().size())});
    }
}

void MultithreadedShadowMappingSceneTest::createVbos()
{
    _vkRenderObjects.resize(renderObjects().size());

    for (std::size_t index = 0; index < renderObjects().size(); ++index) {
        const common::RenderObject& renderObject = renderObjects()[index];
        auto& vkRenderObject = _vkRenderObjects[index];

        vkRenderObject.modelMatrix = renderObject.modelMatrix;
        vkRenderObject.drawCount = renderObject.vertices.size();

        std::vector<glm::vec4> vboBuffer = renderObject.generateCombinedData();

        vk::DeviceSize size = vboBuffer.size() * sizeof(vboBuffer.front());
        vk::BufferUsageFlags usage = vk::BufferUsageFlagBits::eVertexBuffer;

        base::vkx::Buffer stagingBuffer = memory().createStagingBuffer(size);
        {
            auto vboMemory = device().mapMemory(stagingBuffer.memory, stagingBuffer.offset, stagingBuffer.size, {});
            std::memcpy(vboMemory, vboBuffer.data(), static_cast<std::size_t>(stagingBuffer.size));
            device().unmapMemory(stagingBuffer.memory);
        }
        vkRenderObject.vbo =
            memory().copyToDeviceLocalMemory(stagingBuffer, usage, _cmdBuffers.front(), queues().queue());

        memory().destroyBuffer(stagingBuffer);
    }
}

void MultithreadedShadowMappingSceneTest::createSemaphores()
{
    for (std::size_t i = 0; i < window().swapchainImages().size(); ++i) {
        _acquireSemaphores.push_back(device().createSemaphore({}));
        _renderSemaphores.push_back(device().createSemaphore({}));
    }
}

void MultithreadedShadowMappingSceneTest::createFences()
{
    _fences.resize(_cmdBuffers.size());
    for (vk::Fence& fence : _fences) {
        fence = device().createFence({vk::FenceCreateFlagBits::eSignaled});
    }
}

MultithreadedShadowMappingSceneTest::VkDepthBuffer MultithreadedShadowMappingSceneTest::createDepthBuffer(
    const glm::uvec2& size, vk::ImageUsageFlags usage)
{
    VkDepthBuffer depthBuffer;
    depthBuffer.format = findOptimalTilingDepthFormat(physicalDevice());

    if (depthBuffer.format != kDepthFormatCandidates.front()) {
        std::cerr << "Warning: Selected depth format (" << vk::to_string(depthBuffer.format)
                  << ") differs from preffered " << vk::to_string(kDepthFormatCandidates.front())
                  << ". Performance might be slightly changed!";
    }

    vk::ImageCreateInfo depthBufferInfo{{},
                                        vk::ImageType::e2D,
                                        depthBuffer.format,
                                        vk::Extent3D{static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y), 1},
                                        1,
                                        1,
                                        vk::SampleCountFlagBits::e1,
                                        vk::ImageTiling::eOptimal,
                                        usage,
                                        vk::SharingMode::eExclusive,
                                        0,
                                        nullptr,
                                        vk::ImageLayout::eUndefined};
    depthBuffer.image = device().createImage(depthBufferInfo);
    depthBuffer.memory = memory().allocateDeviceLocalMemory(device().getImageMemoryRequirements(depthBuffer.image));
    device().bindImageMemory(depthBuffer.image, depthBuffer.memory, 0);

    // DepthBuffer view
    vk::ImageViewCreateInfo depthBufferViewInfo{{},
                                                depthBuffer.image,
                                                vk::ImageViewType::e2D,
                                                depthBuffer.format,
                                                {},
                                                vk::ImageSubresourceRange{vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1}};
    depthBuffer.view = device().createImageView(depthBufferViewInfo);

    return depthBuffer;
}

vk::RenderPass MultithreadedShadowMappingSceneTest::createShadowmapRenderPass()
{
    std::vector<vk::AttachmentDescription> attachments{
        vk::AttachmentDescription{{},
                                  _shadowmapPass.depthBuffer.format,
                                  vk::SampleCountFlagBits::e1,
                                  vk::AttachmentLoadOp::eClear,
                                  vk::AttachmentStoreOp::eStore,
                                  vk::AttachmentLoadOp::eDontCare,
                                  vk::AttachmentStoreOp::eDontCare,
                                  vk::ImageLayout::eUndefined,
                                  vk::ImageLayout::eDepthStencilAttachmentOptimal},
    };
    vk::AttachmentReference depthReference{0, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    std::vector<vk::SubpassDescription> subpasses{
        {{}, vk::PipelineBindPoint::eGraphics, 0, nullptr, 0, nullptr, nullptr, &depthReference, 0, nullptr},
    };

    std::vector<vk::SubpassDependency> dependencies{
        vk::SubpassDependency{VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe,
                              vk::PipelineStageFlagBits::eLateFragmentTests, vk::AccessFlagBits::eMemoryRead,
                              vk::AccessFlagBits::eDepthStencilAttachmentRead |
                                  vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                              vk::DependencyFlagBits::eByRegion},
        vk::SubpassDependency{0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eLateFragmentTests,
                              vk::PipelineStageFlagBits::eBottomOfPipe,
                              vk::AccessFlagBits::eDepthStencilAttachmentRead |
                                  vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                              vk::AccessFlagBits::eMemoryRead, vk::DependencyFlagBits::eByRegion},
    };

    vk::RenderPassCreateInfo renderPassInfo{{},
                                            static_cast<uint32_t>(attachments.size()),
                                            attachments.data(),
                                            static_cast<uint32_t>(subpasses.size()),
                                            subpasses.data(),
                                            static_cast<uint32_t>(dependencies.size()),
                                            dependencies.data()};

    return device().createRenderPass(renderPassInfo);
}

vk::RenderPass MultithreadedShadowMappingSceneTest::createRenderRenderPass()
{
    std::vector<vk::AttachmentDescription> attachments{
        vk::AttachmentDescription{{},
                                  window().swapchainImageFormat(),
                                  vk::SampleCountFlagBits::e1,
                                  vk::AttachmentLoadOp::eClear,
                                  vk::AttachmentStoreOp::eStore,
                                  vk::AttachmentLoadOp::eDontCare,
                                  vk::AttachmentStoreOp::eDontCare,
                                  vk::ImageLayout::eUndefined,
                                  vk::ImageLayout::ePresentSrcKHR},
        vk::AttachmentDescription{{},
                                  _renderPass.depthBuffer.format,
                                  vk::SampleCountFlagBits::e1,
                                  vk::AttachmentLoadOp::eClear,
                                  vk::AttachmentStoreOp::eStore,
                                  vk::AttachmentLoadOp::eDontCare,
                                  vk::AttachmentStoreOp::eDontCare,
                                  vk::ImageLayout::eUndefined,
                                  vk::ImageLayout::eDepthStencilAttachmentOptimal}};

    vk::AttachmentReference colorWriteAttachment{0, vk::ImageLayout::eColorAttachmentOptimal};
    vk::AttachmentReference depthWriteAttachment{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};

    std::vector<vk::SubpassDescription> subpasses{
        vk::SubpassDescription{{},
                               vk::PipelineBindPoint::eGraphics,
                               0,
                               nullptr,
                               1,
                               &colorWriteAttachment,
                               nullptr,
                               &depthWriteAttachment,
                               0,
                               nullptr},
    };

    std::vector<vk::SubpassDependency> dependencies{
        vk::SubpassDependency{VK_SUBPASS_EXTERNAL, 0, vk::PipelineStageFlagBits::eBottomOfPipe,
                              vk::PipelineStageFlagBits::eLateFragmentTests, vk::AccessFlagBits::eMemoryRead,
                              vk::AccessFlagBits::eDepthStencilAttachmentRead |
                                  vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                              vk::DependencyFlagBits::eByRegion},
        vk::SubpassDependency{0, VK_SUBPASS_EXTERNAL, vk::PipelineStageFlagBits::eLateFragmentTests,
                              vk::PipelineStageFlagBits::eBottomOfPipe,
                              vk::AccessFlagBits::eDepthStencilAttachmentRead |
                                  vk::AccessFlagBits::eDepthStencilAttachmentWrite,
                              vk::AccessFlagBits::eMemoryRead, vk::DependencyFlagBits::eByRegion},
    };

    vk::RenderPassCreateInfo renderPassInfo{{},
                                            static_cast<uint32_t>(attachments.size()),
                                            attachments.data(),
                                            static_cast<uint32_t>(subpasses.size()),
                                            subpasses.data(),
                                            static_cast<uint32_t>(dependencies.size()),
                                            dependencies.data()};
    return device().createRenderPass(renderPassInfo);
}

std::vector<vk::Framebuffer> MultithreadedShadowMappingSceneTest::createFramebuffers(
    const vk::RenderPass& renderPass,
    const std::vector<vk::ImageView>& colorImages,
    const vk::ImageView& depthBuffer,
    const glm::uvec2& size)
{
    std::vector<vk::Framebuffer> framebuffers;
    framebuffers.reserve(colorImages.size());

    for (const vk::ImageView& colorImage : colorImages) {
        std::vector<vk::ImageView> attachments;

        if (colorImage)
            attachments.push_back(colorImage);
        if (depthBuffer)
            attachments.push_back(depthBuffer);

        vk::FramebufferCreateInfo framebufferInfo{{},
                                                  renderPass,
                                                  static_cast<uint32_t>(attachments.size()),
                                                  attachments.data(),
                                                  static_cast<uint32_t>(size.x),
                                                  static_cast<uint32_t>(size.y),
                                                  1};
        framebuffers.push_back(device().createFramebuffer(framebufferInfo));
    }

    return framebuffers;
}

MultithreadedShadowMappingSceneTest::VkProgram MultithreadedShadowMappingSceneTest::createProgram(
    const std::string& path)
{
    VkProgram program;

    program.vertexModule = base::vkx::ShaderModule{device(), path + ".vert.spv"};
    program.fragmentModule = base::vkx::ShaderModule{device(), path + ".frag.spv"};

    return program;
}

vk::DescriptorSetLayout MultithreadedShadowMappingSceneTest::createShadowmapDescriptorSetLayout()
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings{};
    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{{},
                                                              static_cast<uint32_t>(bindings.size()),
                                                              bindings.data()};
    return device().createDescriptorSetLayout(descriptorSetLayoutInfo);
}

vk::DescriptorSetLayout MultithreadedShadowMappingSceneTest::createRenderDescriptorSetLayout()
{
    std::vector<vk::DescriptorSetLayoutBinding> bindings{
        vk::DescriptorSetLayoutBinding{0, vk::DescriptorType::eCombinedImageSampler, 1,
                                       vk::ShaderStageFlagBits::eFragment, nullptr},
    };
    vk::DescriptorSetLayoutCreateInfo descriptorSetLayoutInfo{{},
                                                              static_cast<uint32_t>(bindings.size()),
                                                              bindings.data()};
    return device().createDescriptorSetLayout(descriptorSetLayoutInfo);
}

vk::DescriptorPool MultithreadedShadowMappingSceneTest::createRenderDescriptorPool()
{
    std::vector<vk::DescriptorPoolSize> poolSizes{
        vk::DescriptorPoolSize{vk::DescriptorType::eCombinedImageSampler, 1},
    };
    vk::DescriptorPoolCreateInfo poolInfo{{}, 1, static_cast<uint32_t>(poolSizes.size()), poolSizes.data()};
    return device().createDescriptorPool(poolInfo);
}

vk::DescriptorSet MultithreadedShadowMappingSceneTest::createRenderDescriptorSet(
    const vk::DescriptorPool& descriptorPool, const vk::DescriptorSetLayout descriptorSetLayout)
{
    vk::DescriptorSetAllocateInfo descriptorSetInfo{descriptorPool, 1, &descriptorSetLayout};
    return device().allocateDescriptorSets(descriptorSetInfo).front();
}

vk::Sampler MultithreadedShadowMappingSceneTest::createRenderShadowmapSampler()
{
    vk::SamplerCreateInfo samplerInfo{{},
                                      vk::Filter::eNearest,
                                      vk::Filter::eNearest,
                                      vk::SamplerMipmapMode::eNearest,
                                      vk::SamplerAddressMode::eClampToEdge,
                                      vk::SamplerAddressMode::eClampToEdge,
                                      vk::SamplerAddressMode::eClampToEdge,
                                      0.0f,
                                      VK_FALSE,
                                      1.0f,
                                      VK_FALSE,
                                      {},
                                      0.0f,
                                      0.0f,
                                      vk::BorderColor::eFloatOpaqueWhite,
                                      VK_FALSE};
    return device().createSampler(samplerInfo);
}

vk::PipelineLayout MultithreadedShadowMappingSceneTest::createPipelineLayout(
    const std::vector<vk::DescriptorSetLayout>& setLayouts, bool depthMatrix)
{
    std::vector<vk::PushConstantRange> pushConstants;
    uint32_t rangeSize = static_cast<uint32_t>(sizeof(glm::mat4) * (depthMatrix ? 2 : 1));
    pushConstants.push_back({vk::ShaderStageFlagBits::eVertex, 0, rangeSize}); // MVP matrix (+ depthMVP matrix)

    vk::PipelineLayoutCreateInfo pipelineLayoutInfo{{},
                                                    static_cast<uint32_t>(setLayouts.size()),
                                                    setLayouts.data(),
                                                    static_cast<uint32_t>(pushConstants.size()),
                                                    pushConstants.data()};

    return device().createPipelineLayout(pipelineLayoutInfo);
}

vk::Pipeline MultithreadedShadowMappingSceneTest::createPipeline(const VkProgram& program,
                                                                 const glm::uvec2& renderSize,
                                                                 const vk::PipelineLayout& layout,
                                                                 const vk::RenderPass& renderPass,
                                                                 bool colorBlendEnabled) const
{
    // Shader stages
    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages = getShaderStages(program);

    // Vertex input state
    std::vector<vk::VertexInputBindingDescription> vertexBindingDescriptions{
        {0, 3 * sizeof(glm::vec4), vk::VertexInputRate::eVertex} // Binding #0 - vertex data (position, color, normal)
    };
    std::vector<vk::VertexInputAttributeDescription> vertexAttributeDescription{
        {0, 0, vk::Format::eR32G32B32A32Sfloat, 0}, // Attribute #0 (from binding #0) - vertex position
        {1, 0, vk::Format::eR32G32B32A32Sfloat, sizeof(glm::vec4)}, // Attribute #1 (from binding #1) - vertex color
        {2, 0, vk::Format::eR32G32B32A32Sfloat, 2 * sizeof(glm::vec4)} // Attribute #2 (from binding #0) - vertex normal
    };
    vk::PipelineVertexInputStateCreateInfo vertexInputState{{},
                                                            static_cast<uint32_t>(vertexBindingDescriptions.size()),
                                                            vertexBindingDescriptions.data(),
                                                            static_cast<uint32_t>(vertexAttributeDescription.size()),
                                                            vertexAttributeDescription.data()};

    // Input assembly state
    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyState{{}, vk::PrimitiveTopology::eTriangleList, VK_FALSE};

    // Viewport state
    vk::Viewport viewport{0.0f, 0.0f, static_cast<float>(renderSize.x), static_cast<float>(renderSize.y), 0.0f, 1.0f};
    vk::Rect2D scissor{{0, 0}, {static_cast<uint32_t>(renderSize.x), static_cast<uint32_t>(renderSize.y)}};
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

    // Depth-stencil state
    vk::PipelineDepthStencilStateCreateInfo depthStencilState{{},       VK_TRUE,  VK_TRUE, vk::CompareOp::eLessOrEqual,
                                                              VK_FALSE, VK_FALSE, {},      {},
                                                              0.0f,     1.0f};

    // ColorBlend state
    vk::PipelineColorBlendAttachmentState colorBlendAttachmentState{VK_FALSE};
    colorBlendAttachmentState.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
                                               vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendStateCreateInfo colorBlendState{
        {}, VK_FALSE, vk::LogicOp::eClear, 1, &colorBlendAttachmentState};
    vk::PipelineColorBlendStateCreateInfo* pColorBlendState = (colorBlendEnabled ? &colorBlendState : nullptr);

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
                                                &depthStencilState,
                                                pColorBlendState,
                                                nullptr,
                                                layout,
                                                renderPass,
                                                0};
    return device().createGraphicsPipeline({}, pipelineInfo);
}

void MultithreadedShadowMappingSceneTest::destroyPipeline(vk::Pipeline& pipeline)
{
    device().destroyPipeline(pipeline);
    pipeline = vk::Pipeline{};
}

void MultithreadedShadowMappingSceneTest::destroyPipelineLayout(vk::PipelineLayout& pipelineLayout)
{
    device().destroyPipelineLayout(pipelineLayout);
    pipelineLayout = vk::PipelineLayout{};
}

void MultithreadedShadowMappingSceneTest::destroySampler(vk::Sampler& sampler)
{
    if (sampler) {
        device().destroySampler(sampler);
        sampler = vk::Sampler{};
    }
}

void MultithreadedShadowMappingSceneTest::destroyDescriptorPool(vk::DescriptorPool& descriptorPool)
{
    if (descriptorPool) {
        device().destroyDescriptorPool(descriptorPool);
        descriptorPool = vk::DescriptorPool{};
    }
}

void MultithreadedShadowMappingSceneTest::destroyDescriptorSetLayout(vk::DescriptorSetLayout& descriptorSetLayout)
{
    device().destroyDescriptorSetLayout(descriptorSetLayout);
    descriptorSetLayout = vk::DescriptorSetLayout{};
}

void MultithreadedShadowMappingSceneTest::destroyProgram(VkProgram& program)
{
    program = VkProgram{};
}

void MultithreadedShadowMappingSceneTest::destroyFramebuffers(std::vector<vk::Framebuffer>& framebuffers)
{
    for (const vk::Framebuffer& framebuffer : framebuffers) {
        device().destroyFramebuffer(framebuffer);
    }
    framebuffers.clear();
}

void MultithreadedShadowMappingSceneTest::destroyRenderPass(vk::RenderPass& renderPass)
{
    device().destroyRenderPass(renderPass);
    renderPass = vk::RenderPass{};
}

void MultithreadedShadowMappingSceneTest::destroyDepthBuffer(VkDepthBuffer& depthBuffer)
{
    device().destroyImageView(depthBuffer.view);
    device().destroyImage(depthBuffer.image);
    device().freeMemory(depthBuffer.memory);
    depthBuffer = VkDepthBuffer{};
}

void MultithreadedShadowMappingSceneTest::destroyFences()
{
    for (const vk::Fence& fence : _fences) {
        device().destroyFence(fence);
    }
    _fences.clear();
}

void MultithreadedShadowMappingSceneTest::destroySemaphores()
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

void MultithreadedShadowMappingSceneTest::destroyVbos()
{
    for (auto& vkRenderObject : _vkRenderObjects) {
        memory().destroyBuffer(vkRenderObject.vbo);
    }
    _vkRenderObjects.clear();
}

void MultithreadedShadowMappingSceneTest::setRenderDescriptorSet(const vk::DescriptorSet& descriptorSet)
{
    vk::DescriptorImageInfo imageInfo{_renderPass.sampler, _shadowmapPass.depthBuffer.view,
                                      vk::ImageLayout::eShaderReadOnlyOptimal};
    std::vector<vk::WriteDescriptorSet> descriptorWrites{
        {descriptorSet, 0, 0, 1, vk::DescriptorType::eCombinedImageSampler, &imageInfo, nullptr, nullptr}};
    std::vector<vk::CopyDescriptorSet> descriptorCopies{};
    device().updateDescriptorSets(descriptorWrites, descriptorCopies);
}

glm::mat4 MultithreadedShadowMappingSceneTest::convertProjectionToImage(const glm::mat4& matrix) const
{
    // Since in Vulkan we already have depth in [0, 1] range, we don't have to scale it like we do
    // in OpenGL. Thus we only have to scale and move .x and .y coordinates.
    static const glm::mat4 bias = {0.5, 0.0, 0.0, 0.0, //
                                   0.0, 0.5, 0.0, 0.0, //
                                   0.0, 0.0, 1.0, 0.0, //
                                   0.5, 0.5, 0.0, 1.0};

    return bias * matrix;
}

void MultithreadedShadowMappingSceneTest::destroySecondaryCommandBuffers()
{
    for (auto& threadCmdPool : _threadCmdPools) {
        device().destroyCommandPool(threadCmdPool.cmdPool);
        threadCmdPool.shadowmapCmdBuffers.clear();
        threadCmdPool.renderCmdBuffers.clear();
    }
    _threadCmdPools.clear();
}

void MultithreadedShadowMappingSceneTest::destroyCommandBuffers()
{
    device().destroyCommandPool(_cmdPool);
    _cmdBuffers.clear();
}

std::vector<vk::PipelineShaderStageCreateInfo> MultithreadedShadowMappingSceneTest::getShaderStages(
    const VkProgram& program) const
{
    std::vector<vk::PipelineShaderStageCreateInfo> stages{
        {{}, vk::ShaderStageFlagBits::eVertex, program.vertexModule, "main", nullptr},
        {{}, vk::ShaderStageFlagBits::eFragment, program.fragmentModule, "main", nullptr}};

    return stages;
}

uint32_t MultithreadedShadowMappingSceneTest::getNextFrameIndex() const
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

void MultithreadedShadowMappingSceneTest::prepareSecondaryCommandBuffer(std::size_t threadIndex,
                                                                        std::size_t frameIndex,
                                                                        std::size_t rangeFrom,
                                                                        std::size_t rangeTo) const
{
    TIME_IT("CmdBuffer (secondary) building");

    // Shadowmap pass
    {
        const VkPass& pass = _shadowmapPass;
        const vk::CommandBuffer& cmdBuffer = _threadCmdPools[threadIndex].shadowmapCmdBuffers[frameIndex];

        cmdBuffer.reset({});
        vk::CommandBufferInheritanceInfo inheritanceInfo{pass.renderPass, 0,  pass.framebuffers[frameIndex],
                                                         VK_FALSE,        {}, {}};
        cmdBuffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eRenderPassContinue |
                                                       vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
                                                   &inheritanceInfo});

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pass.pipeline);

        for (std::size_t index = rangeFrom; index < rangeTo; ++index) {
            const VkRenderObject& renderObject = _vkRenderObjects[index];

            glm::mat4 MVP = base::vkx::fixGLMatrix(shadowMatrix() * renderObject.modelMatrix);
            cmdBuffer.pushConstants(pass.pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(MVP), &MVP);

            cmdBuffer.bindVertexBuffers(0, {{renderObject.vbo.buffer}}, {{0}});
            cmdBuffer.draw(static_cast<uint32_t>(renderObject.drawCount), 1, 0, 0);
        }

        cmdBuffer.end();
    }

    // Render pass
    {
        const VkPass& pass = _renderPass;
        const vk::CommandBuffer& cmdBuffer = _threadCmdPools[threadIndex].renderCmdBuffers[frameIndex];

        cmdBuffer.reset({});
        vk::CommandBufferInheritanceInfo inheritanceInfo{pass.renderPass, 0,  pass.framebuffers[frameIndex],
                                                         VK_FALSE,        {}, {}};
        cmdBuffer.begin(vk::CommandBufferBeginInfo{vk::CommandBufferUsageFlagBits::eRenderPassContinue |
                                                       vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
                                                   &inheritanceInfo});

        cmdBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pass.pipeline);
        cmdBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pass.pipelineLayout, 0, 1, &pass.descriptorSet,
                                     0, nullptr);

        for (std::size_t index = rangeFrom; index < rangeTo; ++index) {
            const VkRenderObject& renderObject = _vkRenderObjects[index];

            glm::mat4 matrices[2] = {
                base::vkx::fixGLMatrix(renderMatrix() * renderObject.modelMatrix),
                convertProjectionToImage(base::vkx::fixGLMatrix(shadowMatrix() * renderObject.modelMatrix)),
            };
            cmdBuffer.pushConstants(pass.pipelineLayout, vk::ShaderStageFlagBits::eVertex, 0, 2 * sizeof(glm::mat4),
                                    &matrices);

            cmdBuffer.bindVertexBuffers(0, {{renderObject.vbo.buffer}}, {{0}});
            cmdBuffer.draw(static_cast<uint32_t>(renderObject.drawCount), 1, 0, 0);
        }

        cmdBuffer.end();
    }
}

void MultithreadedShadowMappingSceneTest::prepareCommandBuffer(std::size_t frameIndex) const
{
    {
        TIME_IT("Fence waiting");
        device().waitForFences(1, &_fences[frameIndex], VK_FALSE, UINT64_MAX);
        device().resetFences(1, &_fences[frameIndex]);
    }

    {
        TIME_IT("CmdBuffer building");
        const vk::CommandBuffer& cmdBuffer = _cmdBuffers[frameIndex];
        cmdBuffer.reset({});
        cmdBuffer.begin({vk::CommandBufferUsageFlagBits::eOneTimeSubmit, nullptr});

        std::vector<vk::CommandBuffer> shadowmapSecondaryCommandBuffer;
        std::vector<vk::CommandBuffer> renderSecondaryCommandBuffer;
        {
            // Multithreaded secondary CommandBuffer generation
            std::vector<std::thread> threads(_threadCmdPools.size());
            float batchSize = static_cast<float>(_vkRenderObjects.size()) / static_cast<float>(_threadCmdPools.size());
            std::size_t batchSizeRounded = static_cast<std::size_t>(std::ceil(batchSize));
            for (std::size_t threadIndex = 0; threadIndex < _threadCmdPools.size(); ++threadIndex) {
                shadowmapSecondaryCommandBuffer.push_back(_threadCmdPools[threadIndex].shadowmapCmdBuffers[frameIndex]);
                renderSecondaryCommandBuffer.push_back(_threadCmdPools[threadIndex].renderCmdBuffers[frameIndex]);

                std::size_t rangeFrom = threadIndex * batchSizeRounded;
                std::size_t rangeTo = std::min((threadIndex + 1) * batchSizeRounded, _vkRenderObjects.size());

                threads[threadIndex] =
                    std::move(std::thread(&MultithreadedShadowMappingSceneTest::prepareSecondaryCommandBuffer, this,
                                          threadIndex, frameIndex, rangeFrom, rangeTo));
            }
            for (auto& thread : threads) {
                thread.join();
            }
        }

        {
            // Shadowmap pass
            const vk::ClearValue clearValue = vk::ClearDepthStencilValue{1.0f, 0};
            vk::RenderPassBeginInfo renderPassInfo{_shadowmapPass.renderPass,
                                                   _shadowmapPass.framebuffers[frameIndex],
                                                   {{}, {shadowmapSize().x, shadowmapSize().y}},
                                                   1,
                                                   &clearValue};

            cmdBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);
            cmdBuffer.executeCommands(shadowmapSecondaryCommandBuffer);
            cmdBuffer.endRenderPass();
        }

        // Image barrier between draw calls for shadowmap image
        vk::ImageAspectFlags depthImageAspect = getImageDepthFormatAspect(_shadowmapPass.depthBuffer.format);
        std::vector<vk::ImageMemoryBarrier> imageBarriers{vk::ImageMemoryBarrier{
            vk::AccessFlagBits::eDepthStencilAttachmentWrite, vk::AccessFlagBits::eShaderRead,
            vk::ImageLayout::eDepthStencilAttachmentOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
            VK_QUEUE_FAMILY_IGNORED, VK_QUEUE_FAMILY_IGNORED, _shadowmapPass.depthBuffer.image,
            vk::ImageSubresourceRange{depthImageAspect, 0, 1, 0, 1}}};
        cmdBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eLateFragmentTests,
                                  vk::PipelineStageFlagBits::eFragmentShader, vk::DependencyFlagBits{},
                                  std::vector<vk::MemoryBarrier>{}, std::vector<vk::BufferMemoryBarrier>{},
                                  imageBarriers);

        {
            // Render pass
            const std::vector<vk::ClearValue> clearValues{vk::ClearColorValue{
                                                              std::array<float, 4>{{0.1f, 0.1f, 0.1f, 1.0f}}},
                                                          vk::ClearDepthStencilValue{1.0f, 0}};
            vk::RenderPassBeginInfo renderPassInfo{_renderPass.renderPass,
                                                   _renderPass.framebuffers[frameIndex],
                                                   {{}, {window().size().x, window().size().y}},
                                                   static_cast<uint32_t>(clearValues.size()),
                                                   clearValues.data()};

            cmdBuffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eSecondaryCommandBuffers);
            cmdBuffer.executeCommands(renderSecondaryCommandBuffer);
            cmdBuffer.endRenderPass();
        }

        cmdBuffer.end();
    }
}

void MultithreadedShadowMappingSceneTest::submitCommandBuffer(std::size_t frameIndex) const
{
    TIME_IT("CmdBuffer submition");

    vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    vk::SubmitInfo submits{1, &_acquireSemaphores[_semaphoreIndex], &waitStage, 1, &_cmdBuffers[frameIndex],
                           1, &_renderSemaphores[_semaphoreIndex]};
    queues().queue().submit(submits, _fences[frameIndex]);
}

void MultithreadedShadowMappingSceneTest::presentFrame(std::size_t frameIndex) const
{
    TIME_IT("Frame presentation");

    uint32_t imageIndex = static_cast<uint32_t>(frameIndex);
    vk::PresentInfoKHR presentInfo{1,      &_renderSemaphores[_semaphoreIndex], 1, &window().swapchain(), &imageIndex,
                                   nullptr};
    queues().queue().presentKHR(presentInfo);
}
}
}
