#pragma once
#ifndef VULKAN_RES_HELPERS_H
#define VULKAN_RES_HELPERS_H


#include <vulkan/vulkan.h>

#include <vector>
#include "vulkantypes.h"


namespace VulkanResourceHelpers {
    // descriptor sets

    void createDescriptorSetLayout(VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout);
    uint32_t findMemoryType(VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    VkImageView createImageView(VkDevice& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);

    void createImage(VkPhysicalDevice& physicalDevice, VkDevice& device, uint32_t width, uint32_t height,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
        VkImage& image, VkDeviceMemory& imageMemory);

    // depth resources

    //helperhelper refactor
    VkFormat findSupportedFormat(VkPhysicalDevice& physicalDevice, const std::vector<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat(VkPhysicalDevice& physicalDevice);

    //depth resources
    void createDepthResources(VkPhysicalDevice& physicalDevice, VkDevice& device, VkExtent2D swapChainExtent, VkImageView& depthImageView,
        VkImage& depthImage, VkDeviceMemory& depthImageMemory);

    //frame buffers
    void createFramebuffers(VkDevice& device, std::vector<VkFramebuffer>& swapChainFramebuffers, std::vector<VkImageView>& swapChainImageViews,
        VkRenderPass& renderPass, VkImageView& depthImageView, VkExtent2D& swapChainExtent);

    //####

    void createBuffer(VkPhysicalDevice& physicalDevice, VkDevice& device, VkDeviceSize& size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

    //command helpers 1
    VkCommandBuffer beginSingleTimeCommands(VkCommandPool& commandPool, VkDevice& device);
    void endSingleTimeCommands(VkCommandBuffer& commandBuffer, VkCommandPool& commandPool,
        VkDevice& device, VkQueue& graphicsQueue);
    //cmd helpers

    void transitionImageLayout(VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue,
        VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
    void copyBufferToImage(VkCommandPool& commandPool, VkDevice& device, VkQueue& graphicsQueue,
        VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    //should be able to use this with multiple textures
    void createTextureImage(VkPhysicalDevice& physicalDevice, VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue,
        VkImage& textureImage, VkDeviceMemory& textureImageMemory, std::string imgName);
    void createTextureImageView(VkDevice& device, VkImage& textureImage, VkImageView& textureImageView);
    void createTextureSampler(VkDevice& device, VkSampler& textureSampler);

    // vertex and index buffer helpers
    void copyBuffer(VkCommandPool& commandPool, VkDevice& device, VkQueue& graphicsQueue, VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize& size);

    template<typename V>
    void createVertexBuffer(VkPhysicalDevice& physicalDevice, VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue,
        std::vector<V> vertices, VkBuffer& vertexBuffer, VkDeviceMemory& vertexBufferMemory) {
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;
        createBuffer(physicalDevice, device, bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer, stagingBufferMemory);

        void* data;
        vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(device, stagingBufferMemory);

        createBuffer(physicalDevice, device, bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);

        copyBuffer(commandPool, device, graphicsQueue, stagingBuffer, vertexBuffer, bufferSize);

        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingBufferMemory, nullptr);
    }
    void createIndexBuffer(VkPhysicalDevice& physicalDevice, VkDevice& device, VkCommandPool& commandPool, VkQueue& graphicsQueue,
        std::vector<uint16_t> indices, VkBuffer& indexBuffer, VkDeviceMemory& indexBufferMemory);

    //TODO poss replace swapChainImages with size int
    template <typename UBO>
    void createUniformBuffers(VkPhysicalDevice& physicalDevice, VkDevice& device, std::vector<VkBuffer>& uniformBuffers,
        std::vector<VkDeviceMemory>& uniformBuffersMemory, std::vector<VkImage>& swapChainImages) {

        VkDeviceSize bufferSize = sizeof(UBO);

        uniformBuffers.resize(swapChainImages.size());
        uniformBuffersMemory.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            createBuffer(physicalDevice, device, bufferSize,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                uniformBuffers[i], uniformBuffersMemory[i]);
        }
    }

    template <typename SSBO>
    void createSSBOs(VkPhysicalDevice& physicalDevice, VkDevice& device, std::vector<VkBuffer>& ssboBuffers,
        std::vector<VkDeviceMemory>& ssboBuffersMemory, std::vector<VkImage>& swapChainImages) {

        VkDeviceSize bufferSize = sizeof(SSBO);

        ssboBuffers.resize(swapChainImages.size());
        ssboBuffersMemory.resize(swapChainImages.size());

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            createBuffer(physicalDevice, device, bufferSize,
                VK_BUFFER_USAGE_STORAGE_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                ssboBuffers[i], ssboBuffersMemory[i]);
        }
    }

    //poss replace swapchain images param with size int
    void createDescriptorPool(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool);
    void createDescriptorSets(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool,
        VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers,
        VkImageView& textureImageView, VkSampler& textureSampler);


}

//could put entire configs in these but currently hold the main varying things
struct pipelinebindings {
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
};

struct renderPassConfig {
    VkAttachmentLoadOp colorattachment_loadop;
    VkImageLayout colorattachment_intiallayout;
    VkImageLayout colorattachment_finallayout;
};

struct graphicsPipelineConfig {
    std::string vertshaderFilename;
    std::string fragshaderFilename;
    VkPrimitiveTopology pipelineTopology;
    pipelinebindings pipelinebindsattrs;
    bool dynamicpipelinestate;
    VkColorComponentFlags blendAttachment_colorWriteMask;
    VkBlendFactor blendAttachment_srcColorBlendFactor;
    VkBlendFactor blendAttachment_dstColorBlendFactor;
    VkBlendOp blendAttachment_colorBlendOp;
    VkBlendFactor blendAttachment_srcAlphaBlendFactor;
    VkBlendFactor blendAttachment_dstAlphaBlendFactor;
    VkBlendOp blendAttachment_alphaBlendOp;
};


#endif //VULKAN_RES_HELPERS_H