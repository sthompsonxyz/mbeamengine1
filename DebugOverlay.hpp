#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <array>
#include <optional>
#include <stdexcept>
#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "stb_font_consolas_24_latin1.inl"
#endif
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "vulkanhelpers.h"
#include "vulkanresourcehelpers.h"
#include "vulkandescriptorhelpers.h"



namespace overlays {
    pipelinebindings getdebugpipelinepipelinebindingattribdefinitions() {

        VkVertexInputBindingDescription vInputBindDescription1{};
        vInputBindDescription1.binding = 0;
        vInputBindDescription1.stride = sizeof(glm::vec4);
        vInputBindDescription1.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        VkVertexInputBindingDescription vInputBindDescription2{};
        vInputBindDescription2.binding = 1;
        vInputBindDescription2.stride = sizeof(glm::vec4);
        vInputBindDescription2.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
            vInputBindDescription1,
            vInputBindDescription2
        };
        VkVertexInputAttributeDescription vInputAttribDescription1{};
        vInputAttribDescription1.location = 0;
        vInputAttribDescription1.binding = 0;
        vInputAttribDescription1.format = VK_FORMAT_R32G32_SFLOAT;
        vInputAttribDescription1.offset = 0;
        VkVertexInputAttributeDescription vInputAttribDescription2{};
        vInputAttribDescription2.location = 1;
        vInputAttribDescription2.binding = 1;
        vInputAttribDescription2.format = VK_FORMAT_R32G32_SFLOAT;
        vInputAttribDescription2.offset = sizeof(glm::vec2);
        std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = {
            vInputAttribDescription1,					// Location 0: Position
            vInputAttribDescription2	// Location 1: UV
        };
        pipelinebindings pipelinebindattrs{
            vertexInputBindings,
            vertexInputAttributes
        };

        return pipelinebindattrs;
    }

    class DebugOverlay {

        // Max. number of chars the text overlay buffer can hold
        const int TEXTOVERLAY_MAX_CHAR_COUNT = 2048;

        VkSurfaceKHR surface;
        VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
        VkDevice device;
        VkQueue queue;
        VkFormat colorFormat;
        VkFormat depthFormat;

        uint32_t* frameBufferWidth;
        uint32_t* frameBufferHeight;

        VkSampler sampler;
        VkImage image;
        VkImageView view;
        VkBuffer buffer;
        VkDeviceMemory bufferMemory;
        VkDeviceMemory imageMemory;
        VkDescriptorPool descriptorPool;
        VkDescriptorSetLayout descriptorSetLayout;
        VkDescriptorSet descriptorSet;
        VkPipelineLayout pipelineLayout;
        VkPipelineCache pipelineCache;
        VkPipeline pipeline;
        
        VkCommandPool commandPool;
        std::vector<VkFramebuffer*> frameBuffers;
        renderPassConfig renderpassconfig;
        graphicsPipelineConfig pipelinecfg;
        VkExtent2D swapChainExtent;

        // Pointer to mapped vertex buffer
        glm::vec4* mapped = nullptr;

        stb_fontchar stbFontData[STB_FONT_consolas_24_latin1_NUM_CHARS];
        uint32_t numLetters;

    public:
        VkRenderPass renderPass;

        enum TextAlign { alignLeft, alignCenter, alignRight };

        bool visible = true;

        std::vector<VkCommandBuffer> cmdBuffers;

        DebugOverlay(
            VkSurfaceKHR& surface,
            VkPhysicalDevice& physicalDevice,
            VkDevice& dev,
            VkQueue queue,
            std::vector<VkFramebuffer>& framebuffers,
            VkFormat colorformat,
            VkFormat depthformat,
            uint32_t* framebufferwidth,
            uint32_t* framebufferheight,
            renderPassConfig renderpassconfig,
            graphicsPipelineConfig pipelinecfg,
            VkExtent2D& swapChainExtent) {

            this->surface = surface;
            this->device = dev;
            this->queue = queue;
            this->colorFormat = colorformat;
            this->depthFormat = depthformat;
            this->physicalDevice = physicalDevice;
            this->renderpassconfig = renderpassconfig;
            this->pipelinecfg = pipelinecfg;
            this->swapChainExtent = swapChainExtent;

            this->frameBuffers.resize(framebuffers.size());
            for (uint32_t i = 0; i < framebuffers.size(); i++)
            {
                this->frameBuffers[i] = &framebuffers[i];
            }

            //this->shaderStages = shaderstages;

            this->frameBufferWidth = framebufferwidth;
            this->frameBufferHeight = framebufferheight;

            cmdBuffers.resize(framebuffers.size());

            prepareResources();
            prepareRenderPass();
            preparePipeline();
        }
        ~DebugOverlay() {
            // Free up all Vulkan resources requested by the text overlay
            vkDestroySampler(device, sampler, nullptr);
            vkDestroyImage(device, image, nullptr);
            vkDestroyImageView(device, view, nullptr);
            vkDestroyBuffer(device, buffer, nullptr);
            vkFreeMemory(device, bufferMemory, nullptr);
            vkFreeMemory(device, imageMemory, nullptr);
            vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
            vkDestroyDescriptorPool(device, descriptorPool, nullptr);
            vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
            vkDestroyPipelineCache(device, pipelineCache, nullptr);
            vkDestroyPipeline(device, pipeline, nullptr);
            vkDestroyRenderPass(device, renderPass, nullptr);
            vkDestroyCommandPool(device, commandPool, nullptr);
        }

        // Prepare all vulkan resources required to render the font
        // The text overlay uses separate resources for descriptors (pool, sets, layouts), pipelines and command buffers
        void prepareResources()
        {
            const uint32_t fontWidth = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;
            const uint32_t fontHeight = STB_FONT_consolas_24_latin1_BITMAP_WIDTH;

            static unsigned char font24pixels[fontWidth][fontHeight];
            stb_font_consolas_24_latin1(stbFontData, font24pixels, fontHeight);

            // Command buffer


            // Pool
            VkCommandPoolCreateInfo cmdPoolInfo = {};
            cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
            //auto val = VulkanHelper::findQueueFamilies(physicalDevice, surface); // .graphicsFamily.value();
            cmdPoolInfo.queueFamilyIndex = VulkanHelper::findQueueFamilies(physicalDevice, surface).graphicsFamily.value(); //vulkanDevice->queueFamilyIndices.graphics;
            cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;


            //VulkanHelper::createCommandPool(physicalDevice, device, surface, commandPool);
            //vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &commandPool);
            if (vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &commandPool) != VK_SUCCESS) {
                throw std::runtime_error("failed to create graphics command pool!");
            }

            VkCommandBufferAllocateInfo cmdBufAllocateInfo{}; // =
              //  vks::initializers::commandBufferAllocateInfo(
                 //   commandPool,
                   // VK_COMMAND_BUFFER_LEVEL_PRIMARY,
                    //(uint32_t)cmdBuffers.size());
            //VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
            cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            cmdBufAllocateInfo.commandPool = commandPool;
            cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            cmdBufAllocateInfo.commandBufferCount = (uint32_t)cmdBuffers.size();

            vkAllocateCommandBuffers(device, &cmdBufAllocateInfo, cmdBuffers.data());

            // Vertex buffer
            VkDeviceSize bufferSize = TEXTOVERLAY_MAX_CHAR_COUNT * sizeof(glm::vec4);

            VulkanResourceHelpers::createBuffer(physicalDevice, device, bufferSize,
                VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                buffer, bufferMemory);
            //VkBufferCreateInfo bufferInfo{}; // = vks::initializers::bufferCreateInfo(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, bufferSize);
            //bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            //bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
            //bufferInfo.size = bufferSize;
            //bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            //vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);

            VkMemoryRequirements memReqs;
            VkMemoryAllocateInfo allocInfo{}; // = vks::initializers::memoryAllocateInfo();
            allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;

            vkGetBufferMemoryRequirements(device, buffer, &memReqs);
            allocInfo.allocationSize = memReqs.size;
            allocInfo.memoryTypeIndex = VulkanResourceHelpers::findMemoryType(physicalDevice, memReqs.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            ////vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            //vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
            //vkBindBufferMemory(device, buffer, bufferMemory, 0);

            // Font texture
            VulkanResourceHelpers::createImage(physicalDevice, device, fontWidth, fontHeight,
                VK_FORMAT_R8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                image, imageMemory);

            VkImageCreateInfo imageInfo{}; // = vks::initializers::imageCreateInfo();
            imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
            imageInfo.imageType = VK_IMAGE_TYPE_2D;
            imageInfo.format = VK_FORMAT_R8_UNORM;
            imageInfo.extent.width = fontWidth;
            imageInfo.extent.height = fontHeight;
            imageInfo.extent.depth = 1;
            imageInfo.mipLevels = 1;
            imageInfo.arrayLayers = 1;
            imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
            imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
            imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
            imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
            imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

            //vkCreateImage(device, &imageInfo, nullptr, &image);

            vkGetImageMemoryRequirements(device, image, &memReqs);
            allocInfo.allocationSize = memReqs.size;
            //allocInfo.memoryTypeIndex = VulkanResourceHelpers::findMemoryType(physicalDevice, memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
            ////vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

            //vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory);
            //vkBindImageMemory(device, image, imageMemory, 0);

            // Staging
            //TODO this isall looking like the textureCreateImage helper but with a different source of data
            struct {
                VkDeviceMemory memory;
                VkBuffer buffer;
            } stagingBuffer;

            VkBufferCreateInfo bufferCreateInfo{}; // = vks::initializers::bufferCreateInfo();
            bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferCreateInfo.size = allocInfo.allocationSize;
            bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

            vkCreateBuffer(device, &bufferCreateInfo, nullptr, &stagingBuffer.buffer);

            // Get memory requirements for the staging buffer (alignment, memory type bits)
            vkGetBufferMemoryRequirements(device, stagingBuffer.buffer, &memReqs);

            allocInfo.allocationSize = memReqs.size;
            // Get memory type index for a host visible buffer
            allocInfo.memoryTypeIndex = VulkanResourceHelpers::findMemoryType(physicalDevice, memReqs.memoryTypeBits,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
            //vulkanDevice->getMemoryType(memReqs.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

            vkAllocateMemory(device, &allocInfo, nullptr, &stagingBuffer.memory);
            vkBindBufferMemory(device, stagingBuffer.buffer, stagingBuffer.memory, 0);

            uint8_t* data;
            vkMapMemory(device, stagingBuffer.memory, 0, allocInfo.allocationSize, 0, (void**)&data);
            // Size of the font texture is WIDTH * HEIGHT * 1 byte (only one channel)
            memcpy(data, &font24pixels[0][0], fontWidth * fontHeight);
            vkUnmapMemory(device, stagingBuffer.memory);

            // Copy to image
            VulkanResourceHelpers::transitionImageLayout(device, commandPool, queue, image,
                VK_FORMAT_R8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

            VulkanResourceHelpers::copyBufferToImage(commandPool, device, queue, stagingBuffer.buffer, image,
                static_cast<uint32_t>(fontWidth), static_cast<uint32_t>(fontHeight));

            // Prepare for shader read
            VulkanResourceHelpers::transitionImageLayout(device, commandPool, queue, image, VK_FORMAT_R8_UNORM,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

            vkFreeMemory(device, stagingBuffer.memory, nullptr);
            vkDestroyBuffer(device, stagingBuffer.buffer, nullptr);
            //TODO end textcreateimage refactor


            VkImageViewCreateInfo imageViewInfo{}; // = vks::initializers::imageViewCreateInfo();
            imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            imageViewInfo.image = image;
            imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
            imageViewInfo.format = imageInfo.format;
            imageViewInfo.components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G, VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A };
            imageViewInfo.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
            vkCreateImageView(device, &imageViewInfo, nullptr, &view);

            // Sampler
            VkSamplerCreateInfo samplerInfo{}; // = vks::initializers::samplerCreateInfo();
            samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            samplerInfo.maxAnisotropy = 1.0f;
            samplerInfo.magFilter = VK_FILTER_LINEAR;
            samplerInfo.minFilter = VK_FILTER_LINEAR;
            samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
            samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
            samplerInfo.mipLodBias = 0.0f;
            samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
            samplerInfo.minLod = 0.0f;
            samplerInfo.maxLod = 1.0f;
            samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
            vkCreateSampler(device, &samplerInfo, nullptr, &sampler);

            // Descriptor
            // Font uses a separate descriptor pool
            VulkanDescriptorHelpers::createDescriptorPoolOverlay(device, descriptorPool);

            // Descriptor set layout
            VulkanDescriptorHelpers::createDescriptorSetLayoutOverlay(device, descriptorSetLayout);

            VulkanRenderPipelineHelpers::createPipelineLayout(device, { descriptorSetLayout }, pipelineLayout);

            // Descriptor set
            VulkanDescriptorHelpers::createDescriptorSetsOverlay(device, descriptorPool, descriptorSetLayout, descriptorSet, view, sampler);

            // Pipeline cache
            VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
            pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
            vkCreatePipelineCache(device, &pipelineCacheCreateInfo, nullptr, &pipelineCache);

        }

        // Prepare a separate render pass for rendering the text as an overlay
        void prepareRenderPass()
        {
            VulkanRenderPipelineHelpers::createRenderPass(colorFormat, physicalDevice, device, renderPass, renderpassconfig);
        }

        // Prepare a separate pipeline for the font rendering decoupled from the main application
        void preparePipeline()
        {
            VulkanRenderPipelineHelpers::createGraphicsPipeline2(device, swapChainExtent, pipelineLayout, renderPass, pipeline, pipelinecfg);
        }

        // Map buffer
        void beginTextUpdate()
        {
            vkMapMemory(device, bufferMemory, 0, VK_WHOLE_SIZE, 0, (void**)&mapped);
            numLetters = 0;
        }

        // Add text to the current buffer
        // todo : drop shadow? color attribute?
        void addText(std::string text, float x, float y, TextAlign align)
        {
            const uint32_t firstChar = STB_FONT_consolas_24_latin1_FIRST_CHAR;

            assert(mapped != nullptr);

            const float charW = 1.5f / *frameBufferWidth;
            const float charH = 1.5f / *frameBufferHeight;

            float fbW = (float)*frameBufferWidth;
            float fbH = (float)*frameBufferHeight;
            x = (x / fbW * 2.0f) - 1.0f;
            y = (y / fbH * 2.0f) - 1.0f;

            // Calculate text width
            float textWidth = 0;
            for (auto letter : text)
            {
                stb_fontchar* charData = &stbFontData[(uint32_t)letter - firstChar];
                textWidth += charData->advance * charW;
            }

            switch (align)
            {
            case alignRight:
                x -= textWidth;
                break;
            case alignCenter:
                x -= textWidth / 2.0f;
                break;
            }

            // Generate a uv mapped quad per char in the new text
            for (auto letter : text)
            {
                stb_fontchar* charData = &stbFontData[(uint32_t)letter - firstChar];

                mapped->x = (x + (float)charData->x0 * charW);
                mapped->y = (y + (float)charData->y0 * charH);
                mapped->z = charData->s0;
                mapped->w = charData->t0;
                mapped++;

                mapped->x = (x + (float)charData->x1 * charW);
                mapped->y = (y + (float)charData->y0 * charH);
                mapped->z = charData->s1;
                mapped->w = charData->t0;
                mapped++;

                mapped->x = (x + (float)charData->x0 * charW);
                mapped->y = (y + (float)charData->y1 * charH);
                mapped->z = charData->s0;
                mapped->w = charData->t1;
                mapped++;

                mapped->x = (x + (float)charData->x1 * charW);
                mapped->y = (y + (float)charData->y1 * charH);
                mapped->z = charData->s1;
                mapped->w = charData->t1;
                mapped++;

                x += charData->advance * charW;

                numLetters++;
            }
        }

        // Unmap buffer and update command buffers
        void endTextUpdate()
        {
            vkUnmapMemory(device, bufferMemory);
            mapped = nullptr;
            updateCommandBuffers();
        }

        VkCommandBufferBeginInfo commandBufferBeginInfo()
        {
            VkCommandBufferBeginInfo cmdBufferBeginInfo{};
            cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            return cmdBufferBeginInfo;
        }

        VkRenderPassBeginInfo initrenderPassBeginInfo()
        {
            VkRenderPassBeginInfo renderPassBeginInfo{};
            renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            return renderPassBeginInfo;
        }

        VkViewport initviewport(
            float width,
            float height,
            float minDepth,
            float maxDepth)
        {
            VkViewport viewport{};
            viewport.width = width;
            viewport.height = height;
            viewport.minDepth = minDepth;
            viewport.maxDepth = maxDepth;
            return viewport;
        }

        inline VkRect2D initrect2D(
            int32_t width,
            int32_t height,
            int32_t offsetX,
            int32_t offsetY)
        {
            VkRect2D rect2D{};
            rect2D.extent.width = width;
            rect2D.extent.height = height;
            rect2D.offset.x = offsetX;
            rect2D.offset.y = offsetY;
            return rect2D;
        }

        // Needs to be called by the application
        void updateCommandBuffers()
        {
            VkCommandBufferBeginInfo cmdBufInfo = commandBufferBeginInfo();

            VkClearValue clearValues[2];
            clearValues[1].color = { { 0.0f, 0.0f, 0.0f, 0.0f } };

            VkRenderPassBeginInfo renderPassBeginInfo = initrenderPassBeginInfo();
            renderPassBeginInfo.renderPass = renderPass;
            renderPassBeginInfo.renderArea.extent.width = *frameBufferWidth;
            renderPassBeginInfo.renderArea.extent.height = *frameBufferHeight;
            renderPassBeginInfo.clearValueCount = 2;
            renderPassBeginInfo.pClearValues = clearValues;

            for (int32_t i = 0; i < cmdBuffers.size(); ++i)
            {
                renderPassBeginInfo.framebuffer = *frameBuffers[i];

                if (vkBeginCommandBuffer(cmdBuffers[i], &cmdBufInfo) != VK_SUCCESS) {
                    throw std::runtime_error("failed to begin recording overlay command buffer!");
                }

                vkCmdBeginRenderPass(cmdBuffers[i], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

                VkViewport viewport = initviewport((float)*frameBufferWidth, (float)*frameBufferHeight, 0.0f, 1.0f);
                vkCmdSetViewport(cmdBuffers[i], 0, 1, &viewport);

                VkRect2D scissor = initrect2D(*frameBufferWidth, *frameBufferHeight, 0, 0);
                vkCmdSetScissor(cmdBuffers[i], 0, 1, &scissor);

                vkCmdBindPipeline(cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
                vkCmdBindDescriptorSets(cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &descriptorSet, 0, NULL);

                VkDeviceSize offsets = 0;
                vkCmdBindVertexBuffers(cmdBuffers[i], 0, 1, &buffer, &offsets);
                vkCmdBindVertexBuffers(cmdBuffers[i], 1, 1, &buffer, &offsets);
                for (uint32_t j = 0; j < numLetters; j++)
                {
                    vkCmdDraw(cmdBuffers[i], 4, 1, j * 4, 0);
                }


                vkCmdEndRenderPass(cmdBuffers[i]);

                if (vkEndCommandBuffer(cmdBuffers[i]) != VK_SUCCESS) {
                    throw std::runtime_error("failed to record overlay command buffer!");
                }
            }
        }

    };

}