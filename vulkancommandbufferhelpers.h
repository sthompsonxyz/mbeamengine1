#pragma once


#include <vulkan/vulkan.h>
#include <vector>

namespace VulkanCommandBufferHelpers {

    void createCommandBuffersv4(VkDevice& device, VkPipeline& graphicsPipeline, VkPipelineLayout& pipelineLayout, VkRenderPass& renderPass,
        std::vector<VkCommandBuffer>& commandBuffers, VkCommandPool& commandPool, std::vector<VkFramebuffer>& swapChainFramebuffers,
        VkExtent2D& swapChainExtent, std::vector<RenderObject>& primary_render_objs, VkPipeline& lineslistgraphicsPipeline, VkPipelineLayout& linespipelineLayout,
        std::vector<RenderObject>& line_render_objs) {

        commandBuffers.resize(swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)commandBuffers.size();

        if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate command buffers!");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("failed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = renderPass;
            renderPassInfo.framebuffer = swapChainFramebuffers[i];
            renderPassInfo.renderArea.offset = { 0, 0 };
            renderPassInfo.renderArea.extent = swapChainExtent;

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.0f, 0.0f, 0.0f, 0.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            //TODO vkCmdBeginRendering may be newer/neater this is a VK 1_0 -> 1_3 thing
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            //bind main pipeline
            vkCmdSetScissor(commandBuffers[i], 0, 1, &renderPassInfo.renderArea);
            // Setup viewport:
            {
                VkViewport viewport;
                viewport.x = 0;
                viewport.y = 0;
                viewport.width = (float)swapChainExtent.width;
                viewport.height = (float)swapChainExtent.height;
                viewport.minDepth = 0.0f;
                viewport.maxDepth = 1.0f;
                vkCmdSetViewport(commandBuffers[i], 0, 1, &viewport);
            }

            VkDeviceSize offsets[] = { 0 };
            //...
            //L2
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, lineslistgraphicsPipeline);
            for (size_t j = 0; j < line_render_objs.size(); ++j) {
                VkBuffer vertexBuffers[] = { line_render_objs[j].vertexBuffer };

                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer(commandBuffers[i], line_render_objs[j].indexBuffer, 0, VK_INDEX_TYPE_UINT16);

                vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, linespipelineLayout, 0, 1, &(line_render_objs[j].descriptorSets[i]), 0, nullptr);

                vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(line_render_objs[j].indices.size()), 1, 0, 0, 0);
            }

            //L1?
            vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
            for (size_t j = 0; j < primary_render_objs.size(); ++j) {
                VkBuffer vertexBuffers[] = { primary_render_objs[j].vertexBuffer };

                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer(commandBuffers[i], primary_render_objs[j].indexBuffer, 0, VK_INDEX_TYPE_UINT16);

                vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0, 1, &(primary_render_objs[j].descriptorSets[i]), 0, nullptr);

                vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(primary_render_objs[j].indices.size()), 1, 0, 0, 0);
            }

            //particles
            /*vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, particlesPipeline);
            for (size_t j = 0; j < particle_render_objs.size(); ++j) {
                VkBuffer vertexBuffers[] = { particle_render_objs[j].vertexBuffer };

                vkCmdBindVertexBuffers(commandBuffers[i], 0, 1, vertexBuffers, offsets);

                vkCmdBindIndexBuffer(commandBuffers[i], particle_render_objs[j].indexBuffer, 0, VK_INDEX_TYPE_UINT16);

                vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, linespipelineLayout, 0, 1, &(particle_render_objs[j].descriptorSets[i]), 0, nullptr);

                vkCmdDrawIndexed(commandBuffers[i], static_cast<uint32_t>(particle_render_objs[j].indices.size()), 1, 0, 0, 0);
            }*/
            

            vkCmdEndRenderPass(commandBuffers[i]);

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("failed to record command buffer!");
            }
        }
    }


}
