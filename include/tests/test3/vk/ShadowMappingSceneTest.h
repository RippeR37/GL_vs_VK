#pragma once

#include <framework/VKTest.h>

#include <base/vkx/ShaderModule.h>
#include <tests/test3/BaseShadowMappingSceneTest.h>

#include <string>

namespace tests {
namespace test_vk {
class ShadowMappingSceneTest : public BaseShadowMappingSceneTest, public framework::VKTest
{
  public:
    ShadowMappingSceneTest();

    void setup() override;
    void run() override;
    void teardown() override;

  private:
    struct VkRenderObject
    {
        glm::mat4 modelMatrix;
        std::size_t drawCount;
        base::vkx::Buffer vbo;
    };

    struct VkProgram
    {
        VkProgram() {}
        VkProgram(VkProgram&& other)
            : vertexModule(std::move(other.vertexModule))
            , fragmentModule(std::move(other.fragmentModule))
        {
        }

        VkProgram& operator=(VkProgram&& other)
        {
            vertexModule = std::move(other.vertexModule);
            fragmentModule = std::move(other.fragmentModule);
            return *this;
        }

        base::vkx::ShaderModule vertexModule;
        base::vkx::ShaderModule fragmentModule;
    };

    struct VkDepthBuffer
    {
        vk::Format format;
        vk::Image image;
        vk::DeviceMemory memory;
        vk::ImageView view;
    };

    struct VkPass
    {
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::DescriptorPool descriptorPool;
        vk::DescriptorSet descriptorSet;
        vk::Sampler sampler;
        vk::PipelineLayout pipelineLayout;
        vk::Pipeline pipeline;
        vk::RenderPass renderPass;
        VkProgram program;
        VkDepthBuffer depthBuffer;
        std::vector<vk::Framebuffer> framebuffers;
    };

    void prepareShadowmapPass();
    void prepareRenderPass();
    void destroyPass(VkPass& pass);

    void createVbos();
    void createCommandBuffers();
    void createSemaphores();
    void createFences();
    VkDepthBuffer createDepthBuffer(const glm::uvec2& size, vk::ImageUsageFlags usage);
    vk::RenderPass createShadowmapRenderPass();
    vk::RenderPass createRenderRenderPass();
    std::vector<vk::Framebuffer> createFramebuffers(const vk::RenderPass& renderPass,
                                                    const std::vector<vk::ImageView>& colorImages,
                                                    const vk::ImageView& depthBuffer,
                                                    const glm::uvec2& size);
    VkProgram createProgram(const std::string& path);
    vk::DescriptorSetLayout createShadowmapDescriptorSetLayout();
    vk::DescriptorSetLayout createRenderDescriptorSetLayout();
    vk::DescriptorPool createRenderDescriptorPool();
    vk::DescriptorSet createRenderDescriptorSet(const vk::DescriptorPool& descriptorPool,
                                                const vk::DescriptorSetLayout descriptorSetLayout);
    vk::Sampler createRenderShadowmapSampler();
    vk::PipelineLayout createPipelineLayout(const std::vector<vk::DescriptorSetLayout>& setLayouts, bool depthMatrix);
    vk::Pipeline createPipeline(const VkProgram& program,
                                const glm::uvec2& renderSize,
                                const vk::PipelineLayout& layout,
                                const vk::RenderPass& renderPass,
                                bool colorBlendEnabled) const;

    void destroyPipeline(vk::Pipeline& pipeline);
    void destroyPipelineLayout(vk::PipelineLayout& pipelineLayout);
    void destroySampler(vk::Sampler& sampler);
    void destroyDescriptorPool(vk::DescriptorPool& descriptorPool);
    void destroyDescriptorSetLayout(vk::DescriptorSetLayout& descriptorSetLayout);
    void destroyProgram(VkProgram& program);
    void destroyFramebuffers(std::vector<vk::Framebuffer>& framebuffers);
    void destroyRenderPass(vk::RenderPass& renderPass);
    void destroyDepthBuffer(VkDepthBuffer& depthBuffer);

    void destroyFences();
    void destroySemaphores();
    void destroyCommandBuffers();
    void destroyVbos();

    void setRenderDescriptorSet(const vk::DescriptorSet& descriptorSet);
    glm::mat4 convertProjectionToImage(const glm::mat4& matrix) const;

    std::vector<vk::PipelineShaderStageCreateInfo> getShaderStages(const VkProgram& program) const;
    uint32_t getNextFrameIndex() const;
    void prepareCommandBuffer(std::size_t frameIndex) const;
    void submitCommandBuffer(std::size_t frameIndex) const;
    void presentFrame(std::size_t frameIndex) const;

    std::vector<VkRenderObject> _vkRenderObjects;
    vk::CommandPool _cmdPool;
    std::vector<vk::CommandBuffer> _cmdBuffers;
    std::vector<vk::Fence> _fences;
    mutable std::size_t _semaphoreIndex;
    std::vector<vk::Semaphore> _acquireSemaphores;
    std::vector<vk::Semaphore> _renderSemaphores;

    VkPass _shadowmapPass;
    VkPass _renderPass;
};
}
}
