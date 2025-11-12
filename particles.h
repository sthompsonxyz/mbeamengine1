#pragma once


struct ParticlesLayer : RenderLayer {

    ParticlesLayer(
        vkDeviceInstance* vkDevice_,
        vkSwapChain* swapChain_,
        VkCommandPool& commandPool_,
        VkRenderPass& renderPass_)
        : RenderLayer(vkDevice_, swapChain_, commandPool_, renderPass_) {
        

    }

    ~ParticlesLayer() {}

	VkPipeline particlesgraphicsPipeline;
    VkPipelineLayout particlespipelineLayout;
    VkDescriptorSetLayout descriptorSetLayout;
    //std::vector<VkDescriptorSet> notextureDescriptorSets;
    //std::vector<VkDescriptorSet> bulletparticleDescriptorSets;
    //VkDescriptorPool particlesDescPool;

    /*VkBuffer particlesvertexBuffer;
    VkDeviceMemory particlesvertexBufferMemory;
    VkBuffer particlesindexBuffer;
    VkDeviceMemory particlesindexBufferMemory;*/

    std::vector<VkCommandBuffer> particlesCommandBuffers;
    //std::vector<VkBuffer> uniformBuffers;
    /*std::vector<VkBuffer> SSBOs;
    std::vector<VkDeviceMemory> SSBOsMemory;*/

    void setupCmdBuffers() {
        particlesCommandBuffers.resize(swapChain->swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)particlesCommandBuffers.size();

        if (vkAllocateCommandBuffers(vkDevice->device, &allocInfo, particlesCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate particles command buffers!");
        }
    }

    /*void setupVertexAndIndexBuffers(std::vector<bulletParticle>& bullets) {
        std::vector<VertexLineList> renderverts;
        for (auto& i : bullets) {
            renderverts.push_back(i.vertex);
        }
        std::vector<uint16_t> indices;
        for (uint16_t i = 0; i < renderverts.size(); i++) {
            indices.push_back(i);
        }

        VulkanResourceHelpers::createVertexBuffer<VertexLineList>(vkDevice->physicalDevice, vkDevice->device, commandPool,
            vkDevice->queues.graphicsQueue, renderverts, particlesvertexBuffer, particlesvertexBufferMemory);

        VulkanResourceHelpers::createIndexBuffer(vkDevice->physicalDevice, vkDevice->device, commandPool,
            vkDevice->queues.graphicsQueue, indices, particlesindexBuffer, particlesindexBufferMemory);
    }*/

    void buildFrameCmdBuffers(
        std::list<Actor3<VertexLineList>>& particles,
        size_t cmdBufferIdx
    ) {
        VkResult err;

        //err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
                //check_vk_result(err);
        {
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(particlesCommandBuffers[cmdBufferIdx], &info);
            check_vk_result(err);
        }
        {
            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.8f, 0.3f, 0.0f, 0.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass = renderPass;
            info.framebuffer = swapChain->swapChainFramebuffers[cmdBufferIdx];
            info.renderArea.offset = { 0, 0 };
            info.renderArea.extent = swapChain->swapChainExtent;
            info.clearValueCount = static_cast<uint32_t>(clearValues.size());
            info.pClearValues = clearValues.data();
            vkCmdBeginRenderPass(particlesCommandBuffers[cmdBufferIdx], &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        VkDeviceSize offsets[] = { 0 };

        vkCmdBindPipeline(particlesCommandBuffers[cmdBufferIdx], VK_PIPELINE_BIND_POINT_GRAPHICS, particlesgraphicsPipeline);

        for (auto& p : particles) {
            VkBuffer vertexBuffers[] = { p.vertexBuffer };
            vkCmdBindVertexBuffers(particlesCommandBuffers[cmdBufferIdx], 0, 1, vertexBuffers, offsets);

            vkCmdBindIndexBuffer(particlesCommandBuffers[cmdBufferIdx], p.indexBuffer, 0, VK_INDEX_TYPE_UINT16);

            vkCmdBindDescriptorSets(particlesCommandBuffers[cmdBufferIdx], VK_PIPELINE_BIND_POINT_GRAPHICS, particlespipelineLayout,
                0, 1, &(p.descriptorSets[cmdBufferIdx]), 0, nullptr);

            uint32_t particleindicessize = 1;
            vkCmdDrawIndexed(particlesCommandBuffers[cmdBufferIdx], particleindicessize, 1, 0, 0, 0);
        }

        // Submit command buffer
        vkCmdEndRenderPass(particlesCommandBuffers[cmdBufferIdx]);
        {

            err = vkEndCommandBuffer(particlesCommandBuffers[cmdBufferIdx]);
            check_vk_result(err);

        }


    }

    /*void updatebulletparticleSSBOs(std::vector<bulletParticle>& particles, size_t currentImage) {
        void* dataPickssbo;
        vkMapMemory(vkDevice->device, SSBOsMemory[currentImage], 0, sizeof(bulletParticle)*particles.size(), 0, &dataPickssbo);
        memcpy(particles.data(), dataPickssbo, sizeof(bulletParticle) * particles.size());
        vkUnmapMemory(vkDevice->device, SSBOsMemory[currentImage]);
    }*/

    //void createDescriptorPool() {
    //    std::array<VkDescriptorPoolSize, 1> poolSizes{};
    //    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //    poolSizes[0].descriptorCount = static_cast<uint32_t>(swapChain->swapChainImages.size());
    //    /*poolSizes[1].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //    poolSizes[1].descriptorCount = static_cast<uint32_t>(swapChain->swapChainImages.size());*/

    //    VkDescriptorPoolCreateInfo poolInfo{};
    //    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    //    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    //    poolInfo.pPoolSizes = poolSizes.data();
    //    poolInfo.maxSets = static_cast<uint32_t>(swapChain->swapChainImages.size()) * 50;

    //    if (vkCreateDescriptorPool(vkDevice->device, &poolInfo, nullptr, &particlesDescPool) != VK_SUCCESS) {
    //        throw std::runtime_error("failed to create descriptor pool!");
    //    }
    //}

    //void createDescriptorSetLayout() {
    //    VkDescriptorSetLayoutBinding uboLayoutBinding{};
    //    uboLayoutBinding.binding = 0;
    //    uboLayoutBinding.descriptorCount = 1;
    //    uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //    uboLayoutBinding.pImmutableSamplers = nullptr;
    //    uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

    //    /*VkDescriptorSetLayoutBinding ssboLayoutBinding{};
    //    ssboLayoutBinding.binding = 1;
    //    ssboLayoutBinding.descriptorCount = 1;
    //    ssboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //    ssboLayoutBinding.pImmutableSamplers = nullptr;
    //    ssboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;*/

    //    std::array<VkDescriptorSetLayoutBinding, 1> bindings = { uboLayoutBinding };
    //    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    //    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    //    layoutInfo.bindingCount = static_cast<uint32_t>(bindings.size());
    //    layoutInfo.pBindings = bindings.data();

    //    if (vkCreateDescriptorSetLayout(vkDevice->device, &layoutInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS) {
    //        throw std::runtime_error("failed to create descriptor set layout!");
    //    }
    //}

    /*void createParticlesSSBOs() {
        VulkanResourceHelpers::createSSBOs<bulletParticle>(vkDevice->physicalDevice, vkDevice->device, SSBOs, SSBOsMemory, swapChain->swapChainImages);
    }*/

    //template <typename UBO>
    //void createDescriptorSetsParticles(std::vector<bulletParticle>& particles) {

    //    std::vector<VkDescriptorSetLayout> layouts(swapChain->swapChainImages.size(), descriptorSetLayout);
    //    VkDescriptorSetAllocateInfo allocInfo{};
    //    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    //    allocInfo.descriptorPool = particlesDescPool;
    //    allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChain->swapChainImages.size());
    //    allocInfo.pSetLayouts = layouts.data();

    //    bulletparticleDescriptorSets.resize(swapChain->swapChainImages.size());
    //    auto allocval = vkAllocateDescriptorSets(vkDevice->device, &allocInfo, bulletparticleDescriptorSets.data());
    //    if (allocval != VK_SUCCESS) {
    //        std::cout << "descriptor alloc error: " << allocval << std::endl;
    //        throw std::runtime_error("failed to allocate descriptor sets!");
    //    }

    //    for (size_t i = 0; i < swapChain->swapChainImages.size(); i++) {
    //        VkDescriptorBufferInfo ubobufferInfo{};
    //        ubobufferInfo.buffer = uniformBuffers[i];
    //        ubobufferInfo.offset = 0;
    //        ubobufferInfo.range = sizeof(UBO);

    //        /*VkDescriptorBufferInfo ssbufferInfo{};
    //        ssbufferInfo.buffer = SSBOs[i];
    //        ssbufferInfo.offset = 0;
    //        ssbufferInfo.range = sizeof(SSBO) * particles.size();*/

    //        std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

    //        descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //        descriptorWrites[0].dstSet = bulletparticleDescriptorSets[i];
    //        descriptorWrites[0].dstBinding = 0;
    //        //descriptorWrites[0].dstArrayElement = 0;
    //        descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    //        descriptorWrites[0].descriptorCount = 1;
    //        descriptorWrites[0].pBufferInfo = &ubobufferInfo;

    //        //descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //        //descriptorWrites[1].dstSet = bulletparticleDescriptorSets[i];
    //        //descriptorWrites[1].dstBinding = 1;
    //        ////descriptorWrites[1].dstArrayElement = 0;
    //        //descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    //        //descriptorWrites[1].descriptorCount = 1;
    //        //descriptorWrites[1].pBufferInfo = &ssbufferInfo;

    //        vkUpdateDescriptorSets(vkDevice->device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
    //    }
    //}

    //void createPipelineLayout() {
    //    VulkanRenderPipelineHelpers::createPipelineLayout(vkDevice->device, { descriptorSetLayout }, particlespipelineLayout);
    //}

    /*void buildGraphicsPipeline() {
        VulkanRenderPipelineHelpers::createGraphicsPipeline2(vkDevice->device, swapChain->swapChainExtent,
            particlespipelineLayout, renderPass, particlesgraphicsPipeline, bulletparticle_points_pipelinecfg);
    }*/

    void cleanupRenderLayer() {
        //vkDestroyFramebuffer(vkDevice->device, pickingFrameBuffer, nullptr);
        //picking images and views
        /*vkDestroyBuffer(vkDevice->device, particlesvertexBuffer, nullptr);
        vkFreeMemory(vkDevice->device, particlesvertexBufferMemory, nullptr);
        vkDestroyBuffer(vkDevice->device, particlesindexBuffer, nullptr);
        vkFreeMemory(vkDevice->device, particlesindexBufferMemory, nullptr);*/

        /*for (size_t i = 0; i < SSBOs.size(); i++) {
            vkDestroyBuffer(vkDevice->device, SSBOs[i], nullptr);
            vkFreeMemory(vkDevice->device, SSBOsMemory[i], nullptr);
        }*/

        //vkDestroyPipeline(vkDevice->device, particlesgraphicsPipeline, nullptr);
        //vkDestroyPipelineLayout(vkDevice->device, particlespipelineLayout, nullptr);

        //vkDestroyDescriptorSetLayout(vkDevice->device, descriptorSetLayout, nullptr);
        //vkDestroyDescriptorPool(vkDevice->device, particlesDescPool, nullptr);

        //vkDestroyPipeline(vkDevice->device, pickingPipeline, nullptr);
        
    }

};