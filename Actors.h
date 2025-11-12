#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include "vulkantypes.h"

//#include "vulkanresourcehelpers.h"
#include "vulkandescriptorhelpers.h"
#include "vulkanresourcehelpers.h"
#include "kbctrler.h"
#include "controller.h"

#include "vkdevice_impl.h"




namespace Actors {

    enum struct shapeType {
        point,
        circle,
        rect,
        circlerect, //rendered as a rect, collides like a circle
        polygon
    };
    //a rewrite of actors, flatten static/non static into type config,
    //allow polygon collisions

    template<typename V>
    struct Actor3 {
        Actor3() {
            //no default cons?
        }

        Actor3(std::vector<V>& inrenderverts, std::vector<uint16_t>& indices_) :
            indices(indices_),
            renderverts(inrenderverts)
        {

            //stype = shapeType::polygon;
            velocity = glm::vec3(0.0f);
            position = glm::vec3(0.0f);
            //polyvertices.push_back(glm::vec2(0.0f));
            /*for (size_t i = 1; i < inrenderverts.size(); ++i) {
                polyvertices.push_back(glm::vec2(inrenderverts[i].pos.x, inrenderverts[i].pos.y));
            }*/
        }

        Actor3(std::vector<V>& inrenderverts, std::vector<uint16_t>& indices_, glm::vec3 vel, glm::vec3 ids, uint32_t matidx = 0) :
            indices(indices_),
            renderverts(inrenderverts)
        {

            //stype = shapeType::polygon;
            velocity = vel;
            position = glm::vec3(0.0f);
            materialIndex = matidx;
            objID = ids;
        }

        Actor3(glm::vec3 pointpos) {
            //stype = shapeType::point;
            position = pointpos;
            velocity = glm::vec3(0.0f);
            renderverts.push_back(VertexLineList{ {pointpos, 0.25f},glm::vec4(1.0f) });
            indices = { 0 };
        }

        ~Actor3() {}

        //physics and rendering
        //bool staticActor;
        //shapeType stype;
        glm::vec3 position;
        float angle = 0.0f;
        glm::vec3 velocity;
        bool visible = true;
        float grav = 0.000003f;
        uint32_t materialIndex;

        glm::vec3 objID = glm::vec3(117);

        std::vector<V> renderverts;
        //std::vector<glm::vec3> normals;
        std::vector<uint16_t> indices;

        void setposition(glm::vec3 newpos) {
            position = newpos;
        }
        virtual void updateposition(float delta) {
            position += velocity * delta;
            velocity.y += grav * delta;
        }
        void translate(glm::vec3 trVect) {
            position += trVect;
        }
        void scale(float scalefactor) {
            //needs to be run buffer passing to vertex buffer
            for (auto i = 0; i < renderverts.size(); ++i) {
                renderverts[i].pos *= scalefactor;
                //renderverts[i].pos.z = -0.25f;
            }

        }

        //resource management
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkBuffer indexBuffer;
        VkDeviceMemory indexBufferMemory;
        std::vector<VkBuffer> uniformBuffers;
        std::vector<VkDeviceMemory> uniformBuffersMemory;

        std::vector<VkBuffer> SSBOs;
        std::vector<VkDeviceMemory> SSBOsMemory;

        std::vector<VkDescriptorSet> descriptorSets;

        uint32_t state = 0;
        uint32_t spec_state = 0;

        void createvertexbuffer(vkDeviceInstance* vkDeviceInst, VkCommandPool& commandPool) {
            VulkanResourceHelpers::createVertexBuffer(vkDeviceInst->physicalDevice, vkDeviceInst->device, commandPool, vkDeviceInst->queues.graphicsQueue, renderverts, vertexBuffer, vertexBufferMemory);
        }
        void createIndexBuffer(vkDeviceInstance* vkDeviceInst, VkCommandPool& commandPool) {
            VulkanResourceHelpers::createIndexBuffer(vkDeviceInst->physicalDevice, vkDeviceInst->device, commandPool, vkDeviceInst->queues.graphicsQueue, indices, indexBuffer, indexBufferMemory);
        }

        template <typename UBO>
        void createuniformbuffer(vkDeviceInstance* vkDeviceInst, std::vector<VkImage>& swapChainImages) {
            VulkanResourceHelpers::createUniformBuffers<UBO>(vkDeviceInst->physicalDevice, vkDeviceInst->device, uniformBuffers, uniformBuffersMemory, swapChainImages);
        }

        template <typename SSBO>
        void createSSBOs(vkDeviceInstance* vkDeviceInst, std::vector<VkImage>& swapChainImages) {
            VulkanResourceHelpers::createSSBOs<SSBO>(vkDeviceInst->physicalDevice, vkDeviceInst->device, SSBOs, SSBOsMemory, swapChainImages);
        }

        template <typename UBO>
        void createdescriptorsets2_2(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorSetLayout& descriptorSetLayout,
            VkDescriptorPool& descriptorPool, std::vector<VkImageView> textureImageViews, VkSampler& textureSampler) {
            VulkanDescriptorHelpers::createDescriptorSets2<UBO,ShaderStorageBufferObject1>(device, swapChainImages, descriptorPool, descriptorSetLayout, descriptorSets,
                uniformBuffers, SSBOs, textureImageViews, textureSampler);
        }

        template <typename UBO>
        void createdescriptorsets3(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorSetLayout& descriptorSetLayout,
            VkDescriptorPool& descriptorPool) {
            VulkanDescriptorHelpers::createDescriptorSetsNoTextures<UBO>(device, swapChainImages, descriptorPool, descriptorSetLayout, descriptorSets,
                uniformBuffers);
        }

        template <typename UBO, typename SSBO>
        void createdescriptorsets4(VkDevice& device, std::vector<VkImage>& swapChainImages, VkDescriptorSetLayout& descriptorSetLayout,
            VkDescriptorPool& descriptorPool, std::vector<VkImageView> textureImageViews, VkSampler& textureSampler) {
            VulkanDescriptorHelpers::createDescriptorSets2Pickable<UBO,SSBO>(device, swapChainImages, descriptorPool, descriptorSetLayout, descriptorSets,
                uniformBuffers, SSBOs, textureImageViews, textureSampler);
        }

        void recreatableResourceCleanup(VkDevice device) {
            for (size_t i = 0; i < uniformBuffers.size(); i++) {
                vkDestroyBuffer(device, uniformBuffers[i], nullptr);
                vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
            }
            //vkDestroyDescriptorPool(device, this->descriptorPool, nullptr);
        }

        void recreatableResourceCleanup2(VkDevice device) {
            for (size_t i = 0; i < uniformBuffers.size(); i++) {
                vkDestroyBuffer(device, uniformBuffers[i], nullptr);
                vkFreeMemory(device, uniformBuffersMemory[i], nullptr);
            }

            for (size_t i = 0; i < SSBOs.size(); i++) {
                vkDestroyBuffer(device, SSBOs[i], nullptr);
                vkFreeMemory(device, SSBOsMemory[i], nullptr);
            }
            
        }

        void vertexindexResourceCleanup(VkDevice& device) {
            vkDestroyBuffer(device, indexBuffer, nullptr);
            vkFreeMemory(device, indexBufferMemory, nullptr);
            vkDestroyBuffer(device, vertexBuffer, nullptr);
            vkFreeMemory(device, vertexBufferMemory, nullptr);
        }

        RenderObject getRenderObject() {
            return RenderObject{
                vertexBuffer, indexBuffer, descriptorSets, indices, objID
            };
        }
        //...

    };


    //controlled actors
    //player controlled

    struct BatPlayerActor2 : public Actor3<Vertex> {
        BatPlayerActor2(GameRect gamerect_, glm::vec2 vel, glm::vec3 ids);
        BatPlayerActor2(std::vector<Vertex>& inrenderverts, std::vector<uint16_t>& indices_, glm::vec2 vel, glm::vec3 ids);
        float zpos = -0.25f; //?
        void updateposition(KeyboardControls& kbctrler, ps4Controller& ctrler, float delta);
        glm::vec2 getnextposition(KeyboardControls& kbctrler, ps4Controller& ctrler, float delta);
    };

    struct RBatPlayerActor2 : public Actor3<Vertex> {
        RBatPlayerActor2(GameRect gamerect_, glm::vec2 vel, glm::vec3 ids);
        RBatPlayerActor2(std::vector<Vertex>& inrenderverts, std::vector<uint16_t>& indices_);
        float zpos = -0.25f; //?
        void updateposition(KeyboardControls& kbctrler, ps4Controller& ctrler, float delta);
    };

    //algo controlled
    struct BehavioursActor2 : public Actor3<Vertex> {

    };


}