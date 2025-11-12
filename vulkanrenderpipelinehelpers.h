#pragma once



//#include <vulkan/vulkan.h>
#include <vector>
#include <array>

#include "datahelpers.h"
#include "vulkanresourcehelpers.h"

// renderpass helpers

namespace VulkanRenderPipelineHelpers {

    VkAttachmentDescription buildVkAttachmentDescription(
        VkAttachmentDescriptionFlags flags,
        VkFormat format,
        VkSampleCountFlagBits samples,
        VkAttachmentLoadOp loadOp,
        VkAttachmentStoreOp storeOp,
        VkAttachmentLoadOp stencilLoadOp,
        VkAttachmentStoreOp stencilStoreOp,
        VkImageLayout initialLayout,
        VkImageLayout finalLayout) {

        VkAttachmentDescription attachmentdesc;
            attachmentdesc.flags = flags;
            attachmentdesc.format = format;
            attachmentdesc.samples = samples;
            attachmentdesc.loadOp = loadOp;
            attachmentdesc.storeOp = storeOp;
            attachmentdesc.stencilLoadOp = stencilLoadOp;
            attachmentdesc.stencilStoreOp = stencilStoreOp;
            attachmentdesc.initialLayout = initialLayout;
            attachmentdesc.finalLayout = finalLayout;
        return attachmentdesc;
    }

    VkSubpassDependency buildVkSubpassDependency(
        uint32_t                srcSubpass,
        uint32_t                dstSubpass,
        VkPipelineStageFlags    srcStageMask,
        VkPipelineStageFlags    dstStageMask,
        VkAccessFlags           srcAccessMask,
        VkAccessFlags           dstAccessMask,
        VkDependencyFlags       dependencyFlags) {

        VkSubpassDependency subpassdep;
        subpassdep.srcSubpass = srcSubpass;
        subpassdep.dstSubpass = dstSubpass;
        subpassdep.srcStageMask = srcStageMask;
        subpassdep.dstStageMask = dstStageMask;
        subpassdep.srcAccessMask = srcAccessMask;
        subpassdep.dstAccessMask = dstAccessMask;
        subpassdep.dependencyFlags = dependencyFlags;

        return subpassdep;
    }

    VkSubpassDescription buildVkSubpassDescription(
        VkSubpassDescriptionFlags       flags,
        VkPipelineBindPoint             pipelineBindPoint,
        uint32_t                        inputAttachmentCount,
        const VkAttachmentReference* pInputAttachments,
        uint32_t                        colorAttachmentCount,
        const VkAttachmentReference* pColorAttachments,
        const VkAttachmentReference* pResolveAttachments,
        const VkAttachmentReference* pDepthStencilAttachment,
        uint32_t                        preserveAttachmentCount,
        const uint32_t* pPreserveAttachments) {
        
        VkSubpassDescription subpassdesc;
        subpassdesc.flags = flags;
        subpassdesc.pipelineBindPoint = pipelineBindPoint;
        subpassdesc.inputAttachmentCount = inputAttachmentCount;
        subpassdesc.pInputAttachments = pInputAttachments;
        subpassdesc.colorAttachmentCount = colorAttachmentCount;
        subpassdesc.pColorAttachments = pColorAttachments;
        subpassdesc.pResolveAttachments = pResolveAttachments;
        subpassdesc.pDepthStencilAttachment = pDepthStencilAttachment;
        subpassdesc.preserveAttachmentCount = preserveAttachmentCount;
        subpassdesc.pPreserveAttachments = pPreserveAttachments;

        return subpassdesc;
    }
    
    static void createRenderPass(VkFormat colorFormat, VkPhysicalDevice& physicalDevice, VkDevice& device, VkRenderPass& renderPass, renderPassConfig renderpasscfg) {
        VkAttachmentDescription colorAttachment{};
        colorAttachment = VulkanRenderPipelineHelpers::buildVkAttachmentDescription(
            0,
            colorFormat,
            VK_SAMPLE_COUNT_1_BIT,
            renderpasscfg.colorattachment_loadop,
            VK_ATTACHMENT_STORE_OP_STORE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            renderpasscfg.colorattachment_intiallayout,
            renderpasscfg.colorattachment_finallayout);

        VkAttachmentDescription depthAttachment{};
        
        depthAttachment = VulkanRenderPipelineHelpers::buildVkAttachmentDescription(
            0,
            VulkanResourceHelpers::findDepthFormat(physicalDevice),
            VK_SAMPLE_COUNT_1_BIT,
            VK_ATTACHMENT_LOAD_OP_CLEAR,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            VK_ATTACHMENT_STORE_OP_DONT_CARE,
            VK_IMAGE_LAYOUT_UNDEFINED,
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

        VkAttachmentReference colorAttachmentRef{};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkAttachmentReference depthAttachmentRef{};
        depthAttachmentRef.attachment = 1;
        depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass = VulkanRenderPipelineHelpers::buildVkSubpassDescription(0, VK_PIPELINE_BIND_POINT_GRAPHICS, 0, nullptr,
            1, &colorAttachmentRef, nullptr, &depthAttachmentRef, 0, nullptr);

        std::array<VkSubpassDependency, 2> dependencies;
        dependencies[0] = buildVkSubpassDependency(
            VK_SUBPASS_EXTERNAL,
            0,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_DEPENDENCY_BY_REGION_BIT);
        
        dependencies[1] = buildVkSubpassDependency(
            0,
            VK_SUBPASS_EXTERNAL,
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            VK_ACCESS_MEMORY_READ_BIT,
            VK_DEPENDENCY_BY_REGION_BIT);

        std::array<VkAttachmentDescription, 2> attachments = { colorAttachment, depthAttachment };
        VkRenderPassCreateInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        renderPassInfo.pAttachments = attachments.data();
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
        renderPassInfo.pDependencies = dependencies.data();

        if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create render pass!");
        }
    }


    // pipeline helpers and initialisers
    VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(
        VkPrimitiveTopology topology,
        VkPipelineInputAssemblyStateCreateFlags flags,
        VkBool32 primitiveRestartEnable)
    {
        VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo{};
        pipelineInputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        pipelineInputAssemblyStateCreateInfo.topology = topology;
        pipelineInputAssemblyStateCreateInfo.flags = flags;
        pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable = primitiveRestartEnable;
        return pipelineInputAssemblyStateCreateInfo;
    }

    VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
        VkPolygonMode polygonMode,
        VkCullModeFlags cullMode,
        VkFrontFace frontFace,
        VkPipelineRasterizationStateCreateFlags flags = 0)
    {
        VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo{};
        pipelineRasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
        pipelineRasterizationStateCreateInfo.cullMode = cullMode;
        pipelineRasterizationStateCreateInfo.frontFace = frontFace;
        pipelineRasterizationStateCreateInfo.flags = flags;
        pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
        pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
        pipelineRasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
        pipelineRasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
        return pipelineRasterizationStateCreateInfo;
    }

    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(
        uint32_t attachmentCount,
        const VkPipelineColorBlendAttachmentState* pAttachments)
    {
        VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
        pipelineColorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
        pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
        return pipelineColorBlendStateCreateInfo;
    }

    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
        VkBool32 depthTestEnable,
        VkBool32 depthWriteEnable,
        VkCompareOp depthCompareOp)
    {
        VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
        pipelineDepthStencilStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
        pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
        pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
        pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
        return pipelineDepthStencilStateCreateInfo;
    }

    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(
        uint32_t viewportCount,
        uint32_t scissorCount,
        VkPipelineViewportStateCreateFlags flags = 0)
    {
        VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
        pipelineViewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        pipelineViewportStateCreateInfo.viewportCount = viewportCount;
        pipelineViewportStateCreateInfo.scissorCount = scissorCount;
        pipelineViewportStateCreateInfo.flags = flags;
        return pipelineViewportStateCreateInfo;
    }

    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(
        VkSampleCountFlagBits rasterizationSamples,
        VkPipelineMultisampleStateCreateFlags flags = 0)
    {
        VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
        pipelineMultisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        pipelineMultisampleStateCreateInfo.rasterizationSamples = rasterizationSamples;
        pipelineMultisampleStateCreateInfo.flags = flags;
        return pipelineMultisampleStateCreateInfo;
    }

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(
        const VkDynamicState* pDynamicStates,
        uint32_t dynamicStateCount,
        VkPipelineDynamicStateCreateFlags flags = 0)
    {
        VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
        pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates;
        pipelineDynamicStateCreateInfo.dynamicStateCount = dynamicStateCount;
        pipelineDynamicStateCreateInfo.flags = flags;
        return pipelineDynamicStateCreateInfo;
    }

    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(
        const std::vector<VkDynamicState>& pDynamicStates,
        VkPipelineDynamicStateCreateFlags flags = 0)
    {
        VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
        pipelineDynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates.data();
        pipelineDynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(pDynamicStates.size());
        pipelineDynamicStateCreateInfo.flags = flags;
        return pipelineDynamicStateCreateInfo;
    }

    VkVertexInputBindingDescription vertexInputBindingDescription(
        uint32_t binding,
        uint32_t stride,
        VkVertexInputRate inputRate)
    {
        VkVertexInputBindingDescription vInputBindDescription{};
        vInputBindDescription.binding = binding;
        vInputBindDescription.stride = stride;
        vInputBindDescription.inputRate = inputRate;
        return vInputBindDescription;
    }

    VkVertexInputAttributeDescription vertexInputAttributeDescription(
        uint32_t binding,
        uint32_t location,
        VkFormat format,
        uint32_t offset)
    {
        VkVertexInputAttributeDescription vInputAttribDescription{};
        vInputAttribDescription.location = location;
        vInputAttribDescription.binding = binding;
        vInputAttribDescription.format = format;
        vInputAttribDescription.offset = offset;
        return vInputAttribDescription;
    }

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo()
    {
        VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
        pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        return pipelineVertexInputStateCreateInfo;
    }

    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo(
        const std::vector<VkVertexInputBindingDescription>& vertexBindingDescriptions,
        const std::vector<VkVertexInputAttributeDescription>& vertexAttributeDescriptions
    )
    {
        VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
        pipelineVertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        pipelineVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(vertexBindingDescriptions.size());
        pipelineVertexInputStateCreateInfo.pVertexBindingDescriptions = vertexBindingDescriptions.data();
        pipelineVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptions.size());
        pipelineVertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptions.data();
        return pipelineVertexInputStateCreateInfo;
    }

    VkGraphicsPipelineCreateInfo initpipelineCreateInfo(
        VkPipelineLayout layout,
        VkRenderPass renderPass,
        VkPipelineCreateFlags flags = 0)
    {
        VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.layout = layout;
        pipelineCreateInfo.renderPass = renderPass;
        pipelineCreateInfo.flags = flags;
        pipelineCreateInfo.basePipelineIndex = -1;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        return pipelineCreateInfo;
    }

    VkGraphicsPipelineCreateInfo initpipelineCreateInfo()
    {
        VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
        pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineCreateInfo.basePipelineIndex = -1;
        pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
        return pipelineCreateInfo;
    }





    static void createPipelineLayout(VkDevice& device, std::vector<VkDescriptorSetLayout> descriptorSetLayouts, VkPipelineLayout& pipelineLayout) {

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 1;
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();

        if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    pipelinebindings getpipelinebindingattribdefinitions() {

        VkVertexInputBindingDescription vInputBindDescription1{ Vertex::getBindingDescription() };
        std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
            vInputBindDescription1
        };

        std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = Vertex::getAttributeDescriptions();
        pipelinebindings pipelinebindattrs{
            vertexInputBindings,
            vertexInputAttributes
        };

        return pipelinebindattrs;
    }

    pipelinebindings getpipelinebindingattribdefinitionsLists() {

        VkVertexInputBindingDescription vInputBindDescription1{ VertexLineList::getBindingDescription() };
        std::vector<VkVertexInputBindingDescription> vertexInputBindings = {
            vInputBindDescription1
        };

        std::vector<VkVertexInputAttributeDescription> vertexInputAttributes = VertexLineList::getAttributeDescriptions();
        pipelinebindings pipelinebindattrs{
            vertexInputBindings,
            vertexInputAttributes
        };

        return pipelinebindattrs;
    }


    static void createGraphicsPipeline2(VkDevice& device, VkExtent2D& swapChainExtent,
        VkPipelineLayout& pipelineLayout, VkRenderPass& renderPass, VkPipeline& graphicsPipeline, graphicsPipelineConfig gfxconfig) {

        //shaders
        std::string shadersDir("E:/source/biogamedev2/vkMeshProj2/shaders/");
        auto vertShaderCode = DataHelpers::readFile(shadersDir + gfxconfig.vertshaderFilename);
        auto fragShaderCode = DataHelpers::readFile(shadersDir + gfxconfig.fragshaderFilename);

        VkShaderModule vertShaderModule = VulkanHelper::createShaderModule(device, vertShaderCode);
        VkShaderModule fragShaderModule = VulkanHelper::createShaderModule(device, fragShaderCode);

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

        std::vector<VkPipelineShaderStageCreateInfo> shaderstages = { vertShaderStageInfo, fragShaderStageInfo };
        //end shaders


        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        pipelinebindings pipebindattrs = gfxconfig.pipelinebindsattrs;//getpipelinebindingattribdefinitions();

        vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(pipebindattrs.vertexInputBindings.size());
        vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(pipebindattrs.vertexInputAttributes.size());
        vertexInputInfo.pVertexBindingDescriptions = pipebindattrs.vertexInputBindings.data();
        vertexInputInfo.pVertexAttributeDescriptions = pipebindattrs.vertexInputAttributes.data();

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = gfxconfig.pipelineTopology;
        inputAssembly.flags = 0;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkViewport viewport{};
        VkRect2D scissor{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)swapChainExtent.width;
        viewport.height = (float)swapChainExtent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        scissor.offset = { 0, 0 };
        scissor.extent = swapChainExtent;
        
        //could be changed to dynamic viewport and scissor and set at drawing time
        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;
        viewportState.flags = 0;
        viewportState.pViewports = &viewport;
        viewportState.pScissors = &scissor;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_NONE;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
        depthStencil.back.compareOp = VK_COMPARE_OP_ALWAYS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.blendEnable = VK_TRUE;
        colorBlendAttachment.colorWriteMask = gfxconfig.blendAttachment_colorWriteMask;// VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.srcColorBlendFactor = gfxconfig.blendAttachment_srcColorBlendFactor; // VK_BLEND_FACTOR_SRC_ALPHA;
        colorBlendAttachment.dstColorBlendFactor = gfxconfig.blendAttachment_dstColorBlendFactor; //VK_BLEND_FACTOR_DST_ALPHA;
        colorBlendAttachment.colorBlendOp = gfxconfig.blendAttachment_colorBlendOp; // VK_BLEND_OP_ADD;
        colorBlendAttachment.srcAlphaBlendFactor = gfxconfig.blendAttachment_dstAlphaBlendFactor; // VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.dstAlphaBlendFactor = gfxconfig.blendAttachment_dstAlphaBlendFactor; // VK_BLEND_FACTOR_ONE;
        colorBlendAttachment.alphaBlendOp = gfxconfig.blendAttachment_alphaBlendOp; // VK_BLEND_OP_ADD;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        //colorBlending.logicOpEnable = VK_FALSE;
        //colorBlending.logicOp = VK_LOGIC_OP_CLEAR;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        /*colorBlending.blendConstants[0] = 0.5f;
        colorBlending.blendConstants[1] = 0.5f;
        colorBlending.blendConstants[2] = 0.5f;
        colorBlending.blendConstants[3] = 0.0f;*/
        std::array<VkDynamicState, 2> states = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };

        VkPipelineDynamicStateCreateInfo dynamic_state{};
        dynamic_state.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamic_state.dynamicStateCount = states.size();
        dynamic_state.pDynamicStates = states.data();

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = static_cast<uint32_t>(shaderstages.size());
        pipelineInfo.pStages = shaderstages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.flags = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(device, fragShaderModule, nullptr);
        vkDestroyShaderModule(device, vertShaderModule, nullptr);
    }


}