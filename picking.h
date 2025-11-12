#pragma once

#include "vulkanresourcehelpers.h"

struct pickingLayer : RenderLayer {

    pickingLayer(
        vkDeviceInstance* vkDevice_,
        vkSwapChain* swapChain_,
        VkCommandPool& commandPool_,
        VkRenderPass& renderPass_)
        : RenderLayer(vkDevice_, swapChain_, commandPool_, renderPass_) {}

    ~pickingLayer() {}

    std::vector<VkCommandBuffer> commandBuffers;
    VkDescriptorPool descriptorPool;

    VkImage textureImageColorPick;
    VkDeviceMemory textureImageColorPickMemory;
    VkImage textureImageDepthPick;
    VkDeviceMemory textureImageDepthPickMemory;
    VkImageView textureImageColorPickView;
    VkImageView textureImageDepthPickView;

    std::vector<VkBuffer> pickingsSSBOs;
    std::vector<VkDeviceMemory> pickingsSSBOsMemory;

    VkFramebuffer pickingFrameBuffer;

    std::vector<VkCommandBuffer> pickingCommandBuffers;

    VkDescriptorSetLayout pickingDescriptorSetLayout;

    VkPipelineLayout pickingPipelineLayout;
    VkPipeline pickingPipeline;

    void setupPickingImagesAndViews(
        uint32_t gwidth,
        uint32_t gheight
    ) {
        VulkanResourceHelpers::createImage(vkDevice->physicalDevice, vkDevice->device, gwidth, gheight, VK_FORMAT_R32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImageColorPick, textureImageColorPickMemory);
        textureImageColorPickView = VulkanResourceHelpers::createImageView(vkDevice->device, textureImageColorPick, VK_FORMAT_R32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT);

        VulkanResourceHelpers::createImage(vkDevice->physicalDevice, vkDevice->device, gwidth, gheight, VK_FORMAT_D32_SFLOAT, VK_IMAGE_TILING_OPTIMAL,
            VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, textureImageDepthPick, textureImageDepthPickMemory);
        textureImageDepthPickView = VulkanResourceHelpers::createImageView(vkDevice->device, textureImageDepthPick, VK_FORMAT_D32_SFLOAT, VK_IMAGE_ASPECT_DEPTH_BIT);
    }

    void setupPickingFrameBuffer() {
        std::array<VkImageView, 2> attachments = { textureImageColorPickView, textureImageDepthPickView };
        VkFramebufferCreateInfo framebuffer_info{};
        framebuffer_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebuffer_info.renderPass = renderPass;
        framebuffer_info.pAttachments = attachments.data();
        framebuffer_info.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebuffer_info.width = swapChain->swapChainExtent.width;
        framebuffer_info.height = swapChain->swapChainExtent.height;
        framebuffer_info.layers = 1;
        if (vkCreateFramebuffer(vkDevice->device, &framebuffer_info, nullptr, &pickingFrameBuffer) != VK_SUCCESS) {
            throw std::runtime_error("failed to create picking framebuffer!");
        }
    }

    void setupPickingCommandBuffers() {
        //picking command buffers
        pickingCommandBuffers.resize(swapChain->swapChainImages.size());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)pickingCommandBuffers.size();

        if (vkAllocateCommandBuffers(vkDevice->device, &allocInfo, pickingCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate picking command buffers!");
        }
    }

    void createPickingPipeline(
        VkDescriptorSetLayout& mainDescriptorSetLayout
    ) {
        //picking pipeline
        std::string shadersDir("E:/source/biogamedev2/vkMeshProj2/shaders/");
        auto vertShaderCode = DataHelpers::readFile(shadersDir + "pickingvert.spv");
        auto fragShaderCode = DataHelpers::readFile(shadersDir + "pickingfrag.spv");

        VkShaderModule vertShaderModule = VulkanHelper::createShaderModule(vkDevice->device, vertShaderCode);
        VkShaderModule fragShaderModule = VulkanHelper::createShaderModule(vkDevice->device, fragShaderCode);

        VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
        vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
        fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName = "main";
        std::vector< VkPipelineShaderStageCreateInfo> shaderStages;
        shaderStages.push_back(vertShaderStageInfo);
        shaderStages.push_back(fragShaderStageInfo);

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        auto bindingDescription = Vertex::getBindingDescription();
        auto attributeDescriptions = Vertex::getAttributeDescriptions();

        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        viewport.x = 0;
        viewport.y = 0;
        viewport.width = (float)swapChain->swapChainExtent.width;
        viewport.height = (float)swapChain->swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        //We're going to have a dynamic scissor so this doesnt really matter
        VkRect2D scissor{};
        scissor.offset = { 0,0 };
        scissor.extent = swapChain->swapChainExtent;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        //Only one samples since this is offscreen
        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        //blending is set to false since we don't need it here
        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;
        colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
        colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 1.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        //must use vkCmdSetScissor() for every command buffer / frame
        std::array<VkDynamicState, 1> states = { VK_DYNAMIC_STATE_SCISSOR };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = states.size();
        dynamic_state.pDynamicStates = states.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = &mainDescriptorSetLayout;

        if (vkCreatePipelineLayout(vkDevice->device, &pipelineLayoutInfo, nullptr, &pickingPipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pickingPipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        pipelineInfo.pDynamicState = &dynamic_state;

        if (vkCreateGraphicsPipelines(vkDevice->device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pickingPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(vkDevice->device, fragShaderModule, nullptr);
        vkDestroyShaderModule(vkDevice->device, vertShaderModule, nullptr);
    }

    void buildPickingCommandBuffers(
        std::list<Actor3<Vertex> >& pickables,
        int image_index,
        double mousex,
        double mousey
    ) {
        //object picking;
        //if (true) {
            //start recording object picking command buffer
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(pickingCommandBuffers[image_index], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("failed to begin recording command buffer!");
        }
        //begin renderpass
        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = { 0.5f, 0.0f, 0.0f, 0.0f };
        clearValues[1].depthStencil = { 1.0f, 0 };
        VkRenderPassBeginInfo object_picking_renderpass_begin_info{};
        object_picking_renderpass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        object_picking_renderpass_begin_info.renderPass = renderPass;
        object_picking_renderpass_begin_info.framebuffer = pickingFrameBuffer;
        object_picking_renderpass_begin_info.renderArea.offset = { 0,0 };
        object_picking_renderpass_begin_info.renderArea.extent = swapChain->swapChainExtent;
        object_picking_renderpass_begin_info.clearValueCount = static_cast<uint32_t>(clearValues.size());
        object_picking_renderpass_begin_info.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(pickingCommandBuffers[image_index], &object_picking_renderpass_begin_info,
            VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(pickingCommandBuffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS,
            pickingPipeline);

        //render all renderables
        //for (size_t j = 0; j < pickables.size(); j++)
        for (auto& p : pickables)
        {
            //the dynamic pipeline state means we have to set the scissor before each draw
            VkRect2D rect{};
            //rect.offset = { 0,0 };
            //rect.extent = swapChainExtent;
            rect.offset.x = mousex > 0 ? mousex : 0;
            rect.offset.y = mousey > 0 ? mousey : 0;
            rect.extent = { 1,1 };

            //can only be used with a dynamic scissor state
            vkCmdSetScissor(pickingCommandBuffers[image_index], 0, 1, &rect);

            //bind descriptor sets for current object
            vkCmdBindDescriptorSets(pickingCommandBuffers[image_index], VK_PIPELINE_BIND_POINT_GRAPHICS,
                pickingPipelineLayout, 0, 1, &(p.descriptorSets[image_index]),
                0, nullptr);

            //bind vertex and index buffers for current object
            VkBuffer vertexBuffers[] = { p.vertexBuffer };
            VkDeviceSize offsets[] = { 0 };
            vkCmdBindVertexBuffers(pickingCommandBuffers[image_index], 0, 1,
                vertexBuffers, offsets);
            vkCmdBindIndexBuffer(pickingCommandBuffers[image_index], p.indexBuffer,
                0, VK_INDEX_TYPE_UINT16);

            //draw
            vkCmdDrawIndexed(pickingCommandBuffers[image_index], static_cast<uint32_t>(p.indices.size()), 1, 0, 0, 0);

            //offset += index_size[j];
        }
        vkCmdEndRenderPass(pickingCommandBuffers[image_index]);

        if (vkEndCommandBuffer(pickingCommandBuffers[image_index]) != VK_SUCCESS) {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void cleanupRenderLayer() {
        vkDestroyFramebuffer(vkDevice->device, pickingFrameBuffer, nullptr);
        //picking images and views
        vkDestroyImageView(vkDevice->device, textureImageDepthPickView, nullptr);
        vkDestroyImageView(vkDevice->device, textureImageColorPickView, nullptr);
        vkDestroyImage(vkDevice->device, textureImageDepthPick, nullptr);
        vkDestroyImage(vkDevice->device, textureImageColorPick, nullptr);
        vkFreeMemory(vkDevice->device, textureImageDepthPickMemory, nullptr);
        vkFreeMemory(vkDevice->device, textureImageColorPickMemory, nullptr);

        vkDestroyPipeline(vkDevice->device, pickingPipeline, nullptr);
        vkDestroyPipelineLayout(vkDevice->device, pickingPipelineLayout, nullptr);
        vkDestroyRenderPass(vkDevice->device, renderPass, nullptr);
    }

};
