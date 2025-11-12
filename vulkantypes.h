#pragma once

#include <vulkan/vulkan.h>

#include <optional>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "stb_font_consolas_24_latin1.inl"
#endif


#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <vector>
#include <list>
#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <array>
#include <optional>
#include <set>
#include <iomanip>

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct GameRect {
    float posx;
    float posy;
    float sizex;
    float sizey;

    /*std::vector<glm::vec2> getCollPolygon() {
        std::vector<glm::vec2> ret;
        ret.push_back({(posx + (posx+sizex))/2.0f, (posy + (posy+sizey))/2.0f});
        ret.push_back({ posx,posy });
        ret.push_back({ posx+sizex,posy });
        ret.push_back({ posx+sizex,posy+sizey });
        ret.push_back({ posx, posy+sizey });

        return ret;
    }*/

    
};


struct Polygon {
    std::vector<glm::vec2> points;
    std::vector<glm::vec2> edges;

    std::vector<glm::vec2> getCollPolygon() {
        std::vector<glm::vec2> ret;
        for (int i = 0; i < points.size(); ++i) {
            ret.push_back({ points[i].x, points[i].y });
        }

        return ret;
    }
};

struct Vertex {
    glm::vec3 pos;
    glm::vec3 normal;
    glm::vec4 color;
    glm::vec2 texCoord;
    glm::vec3 objID;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(Vertex);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{5};

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, normal);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(Vertex, color);

        attributeDescriptions[3].binding = 0;
        attributeDescriptions[3].location = 3;
        attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[3].offset = offsetof(Vertex, texCoord);

        attributeDescriptions[4].binding = 0;
        attributeDescriptions[4].location = 4;
        attributeDescriptions[4].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[4].offset = offsetof(Vertex, objID);

        return attributeDescriptions;
    }
};

struct VertexL2 {
    glm::vec3 pos;
    glm::vec4 color;
    glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexL2);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{ 3 };

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexL2, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexL2, color);

        attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VertexL2, texCoord);

        return attributeDescriptions;
    }
};

struct VertexLineList {
    glm::vec3 pos;
    glm::vec4 color;
    //glm::vec2 texCoord;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.stride = sizeof(VertexLineList);
        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

        return bindingDescription;
    }

    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{ 2 };

        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(VertexL2, pos);

        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(VertexL2, color);

        /*attributeDescriptions[2].binding = 0;
        attributeDescriptions[2].location = 2;
        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
        attributeDescriptions[2].offset = offsetof(VertexL2, texCoord);*/

        return attributeDescriptions;
    }
};

//struct VertexPickable {
//    glm::vec3 pos;
//    glm::vec4 color;
//    glm::vec2 texCoord;
//    glm::vec4 objID;
//
//    static VkVertexInputBindingDescription getBindingDescription() {
//        VkVertexInputBindingDescription bindingDescription{};
//        bindingDescription.binding = 0;
//        bindingDescription.stride = sizeof(VertexPickable);
//        bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
//
//        return bindingDescription;
//    }
//
//    static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions() {
//        std::vector<VkVertexInputAttributeDescription> attributeDescriptions{ 4 };
//
//        attributeDescriptions[0].binding = 0;
//        attributeDescriptions[0].location = 0;
//        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
//        attributeDescriptions[0].offset = offsetof(VertexPickable, pos);
//
//        attributeDescriptions[1].binding = 0;
//        attributeDescriptions[1].location = 1;
//        attributeDescriptions[1].format = VK_FORMAT_R32G32B32A32_SFLOAT;
//        attributeDescriptions[1].offset = offsetof(VertexPickable, color);
//
//        attributeDescriptions[2].binding = 0;
//        attributeDescriptions[2].location = 2;
//        attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
//        attributeDescriptions[2].offset = offsetof(VertexPickable, texCoord);
//
//        attributeDescriptions[3].binding = 0;
//        attributeDescriptions[3].location = 3;
//        attributeDescriptions[3].format = VK_FORMAT_R32G32B32A32_SFLOAT;
//        attributeDescriptions[3].offset = offsetof(VertexPickable, objID);
//
//        return attributeDescriptions;
//    }
//};

struct UniformBufferObject {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) glm::vec3 lightpos;
    alignas(16) glm::vec3 lightpos2;
    alignas(16) glm::vec3 viewpos;
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
    alignas(16) glm::vec3 globalLightDir;
    alignas(16) glm::vec3 CLQ1;
    alignas(16) glm::vec3 cameraFront;
    uint32_t imgstate;
    uint32_t visstate;
    uint32_t matIdx;
};

struct UniformBufferObjectL2 {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(16) float timeval;
    uint32_t testval;
};

//struct ShaderStorageBufferObject1 {
//    glm::vec4 objids;
//};

struct Material {
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
    alignas(16) glm::vec3 shininess;
};

struct Material2 {

};

struct Light {
    alignas(16) glm::vec3 direction;
    alignas(16) glm::vec3 ambient;
    alignas(16) glm::vec3 diffuse;
    alignas(16) glm::vec3 specular;
};

struct ShaderStorageBufferObject1 {
    Material mats[2];
};

struct Particle1 {
    glm::vec3 pos;
};
struct ShaderStorageBufferObject2 {
    Particle1 particles[];
};

struct RenderObject {
    VkBuffer& vertexBuffer;
    VkBuffer& indexBuffer;
    std::vector<VkDescriptorSet>& descriptorSets;
    const std::vector<uint16_t>& indices;
    glm::vec3 objID;
};

struct TextureBufferSet {
    VkImage textureImage;
    VkDeviceMemory textureImageMemory;
    VkImageView textureImageView;
};

struct HostDeviceBufferPair {
    VkBuffer buffer;
    VkDeviceMemory bufferMemory;
};


struct vkSwapChain {
    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;
};

struct vkQueues {
    QueueFamilyIndices queueindices;
    VkQueue graphicsQueue;
    VkQueue presentQueue;
};

//struct bulletParticle {
//    VertexLineList vertex;
//    glm::vec2 velocity;
//};
//
//struct bulletParticle2 {
//    glm::vec3 pos;
//    glm::vec4 color;
//    glm::vec2 velocity;
//};