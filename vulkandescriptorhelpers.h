#pragma once

#include <vulkan/vulkan.h>

#include <array>
#include <vector>
#include <stdexcept>
#include <iostream>

namespace VulkanDescriptorHelpers {

    // descriptor sets
    //TODO reduce to one func
    void createDescriptorSetLayout(VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout);
    void createDescriptorSetLayout2(VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout, size_t textureCount);
    void createDescriptorSetLayoutNoTexture(VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout);
    void createDescriptorSetLayout2Pickable(VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout, size_t textureCount);

    void createDescriptorSetLayoutOverlay(VkDevice& device, VkDescriptorSetLayout& descriptorSetLayout);

    //TODO poss replace swapchain images param with size int and reduce these to one func
    //void createDescriptorPool(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool);
    void createDescriptorPool2(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool);
    void createDescriptorPoolnotextures(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool);
    void createDescriptorPoolOverlay(VkDevice& device, VkDescriptorPool& descriptorPool);

    void createDescriptorSets(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool,
        VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers,
        VkImageView& textureImageView, VkSampler& textureSampler);

    template <typename UBO, typename SSBO>
    void createDescriptorSets2(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool,
        VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers,
        std::vector<VkBuffer>& SSBOs, std::vector<VkImageView>& textureImageViews, VkSampler& textureSampler) {

        std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
        allocInfo.pSetLayouts = layouts.data();

        descriptorSets.resize(swapChainImages.size());
        auto allocval = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());
        if (allocval != VK_SUCCESS) {
            std::cout << "descriptor alloc error: " << allocval << std::endl;
            throw std::runtime_error("failed to allocate descriptor sets!");
        }

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UBO);

            VkDescriptorBufferInfo ssbufferInfo{};
            bufferInfo.buffer = SSBOs[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(SSBO);

            std::vector<VkDescriptorImageInfo> imageInfos;
            for (auto j = 0; j < textureImageViews.size(); ++j) {
                VkDescriptorImageInfo imageInfo;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = textureImageViews[j];
                imageInfo.sampler = textureSampler;
                imageInfos.push_back(imageInfo);
            }

            std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = &ssbufferInfo;

            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = descriptorSets[i];
            descriptorWrites[2].dstBinding = 1;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[2].descriptorCount = imageInfos.size();
            descriptorWrites[2].pImageInfo = imageInfos.data();

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    template <typename UBO, typename SSBO>
    void createDescriptorSets2Pickable(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool,
        VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers,
        std::vector<VkBuffer>& SSBOs, std::vector<VkImageView>& textureImageViews, VkSampler& textureSampler) {

        //lets not realloc descsets every time, because you don't delete them
        //just re-update
        if (!descriptorSets.size()) {
            std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets.resize(swapChainImages.size());
            auto allocval = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());
            if (allocval != VK_SUCCESS) {
                std::cout << "descriptor alloc error: " << allocval << std::endl;
                throw std::runtime_error("failed to allocate descriptor sets!");
            }
        }

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkDescriptorBufferInfo ubobufferInfo{};
            ubobufferInfo.buffer = uniformBuffers[i];
            ubobufferInfo.offset = 0;
            ubobufferInfo.range = sizeof(UBO);

            VkDescriptorBufferInfo ssbufferInfo{};
            ssbufferInfo.buffer = SSBOs[i];
            ssbufferInfo.offset = 0;
            ssbufferInfo.range = sizeof(SSBO);

            std::vector<VkDescriptorImageInfo> imageInfos;
            for (auto j = 0; j < textureImageViews.size(); ++j) {
                VkDescriptorImageInfo imageInfo;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = textureImageViews[j];
                imageInfo.sampler = textureSampler;
                imageInfos.push_back(imageInfo);
            }


            std::array<VkWriteDescriptorSet, 3> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            //descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &ubobufferInfo;

            descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            //descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            descriptorWrites[1].descriptorCount = 1;
            descriptorWrites[1].pBufferInfo = &ssbufferInfo;

            descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[2].dstSet = descriptorSets[i];
            descriptorWrites[2].dstBinding = 2;
            descriptorWrites[2].dstArrayElement = 0;
            descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[2].descriptorCount = imageInfos.size();
            descriptorWrites[2].pImageInfo = imageInfos.data();

            

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    

    template <typename UBO>
    void createDescriptorSetsNoTextures(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool,
        VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers) {

        //lets not realloc descsets every time, because you don't delete them
        //just re-update
        if (!descriptorSets.size()) {
            std::vector<VkDescriptorSetLayout> layouts(swapChainImages.size(), descriptorSetLayout);
            VkDescriptorSetAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
            allocInfo.descriptorPool = descriptorPool;
            allocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());
            allocInfo.pSetLayouts = layouts.data();

            descriptorSets.resize(swapChainImages.size());
            auto allocval = vkAllocateDescriptorSets(device, &allocInfo, descriptorSets.data());
            if (allocval != VK_SUCCESS) {
                std::cout << "descriptor alloc error: " << allocval << std::endl;
                throw std::runtime_error("failed to allocate no text type descriptor sets!");
            }
        }

        for (size_t i = 0; i < swapChainImages.size(); i++) {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = sizeof(UBO);

            /*std::vector<VkDescriptorImageInfo> imageInfos;
            for (auto j = 0; j < textureImageViews.size(); ++j) {
                VkDescriptorImageInfo imageInfo;
                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                imageInfo.imageView = textureImageViews[j];
                imageInfo.sampler = textureSampler;
                imageInfos.push_back(imageInfo);
            }*/

            std::array<VkWriteDescriptorSet, 1> descriptorWrites{};

            descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[0].dstSet = descriptorSets[i];
            descriptorWrites[0].dstBinding = 0;
            descriptorWrites[0].dstArrayElement = 0;
            descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrites[0].descriptorCount = 1;
            descriptorWrites[0].pBufferInfo = &bufferInfo;

            /*descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrites[1].dstSet = descriptorSets[i];
            descriptorWrites[1].dstBinding = 1;
            descriptorWrites[1].dstArrayElement = 0;
            descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            descriptorWrites[1].descriptorCount = imageInfos.size();
            descriptorWrites[1].pImageInfo = imageInfos.data();*/

            vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorWrites.size()), descriptorWrites.data(), 0, nullptr);
        }
    }

    void createDescriptorSetsLayer2(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorPool& descriptorPool,
        VkDescriptorSetLayout& descriptorSetLayout, std::vector<VkDescriptorSet>& descriptorSets, std::vector<VkBuffer>& uniformBuffers,
        std::vector<VkImageView>& textureImageViews, VkSampler& textureSampler);

    void createDescriptorSetsOverlay(VkDevice& device, VkDescriptorPool& descriptorPool, VkDescriptorSetLayout& descriptorSetLayout,
        VkDescriptorSet& descriptorSet, VkImageView& imageView, VkSampler& sampler);

}