

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
//#define GLM_FORCE_LEFT_HANDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/type_ptr.hpp>

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
//#include <functional>
//basics / invoke.hpp
//#include <utility> // for std::invoke()
//#include <functional> // for std::forward()
//template<typename Callable, typename …Args>
//decltype(auto) call(Callable && op, Args&& …args)
//{
//    return std::invoke(std::forward<Callable>(op), //passed callable with
//        std::forward<Args>(args)…); // any additional args
//}
#include <thread>

#include <imgui.h>
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

using namespace std::chrono_literals;

#define LOGGING

#ifdef LOGGING
#include "logger.hpp"

logger Logger{};

#endif // LOGGING

#include "vulkantypes.h"
#include "vkdevice_impl.h"

#include "vulkanhelpers.h"
#include "vulkanresourcehelpers.h"
#include "vulkanrenderpipelinehelpers.h"
#include "vulkancommandbufferhelpers.h"
#include "vulkansynchelpers.h"
#include "vulkandescriptorhelpers.h"

#include "datahelpers.h"

#include "controller.h"
#include "kbctrler.h"

#include "DebugOverlay.hpp"


#include "Actors.h"
#include "physics.h"

//sdl audio
#include <SDL3/SDL.h>
#include <SDL3/SDL_mixer.h>
#undef main

//using physics::AABBCheckCollision;
//using physics::AABBCollisionBatBallDirection;
//using physics::getAABBCollisionRectRectDirection;

using Actors::BatPlayerActor2;
using Actors::RBatPlayerActor2;
using Actors::Actor3;

#include "audio/audio.hpp"
#include "crafting.h"

#ifndef TINYGLTF_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_INCLUDE_STB_IMAGE_WRITE 
#include "tiny_gltf.h"
#endif

#include "pipelineconfig.h"

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

const int MAX_FRAMES_IN_FLIGHT = 2;

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
        func(instance, debugMessenger, pAllocator);
    }
}

#include "renderlayer.h"
#include "picking.h"
#include "particles.h"
#include "appimguilayer.h"
#include "vulkanapp.h"

//ALPHA
// frame rate capping
// controller/world vars obj
// 
//IN PROG

// pong physics game
// quad tree + collisions
// audio thread
// crafting and inventory
// menu overlays
// mesh/object loader (in meshproj)
// objects from blender (in meshproj (gltf))
// shader/push const type picking
// interactible/activatable objects
// bullet particle pipeline
//  
//TODOs
// level loader/editor
// terrain gen branch
// 3D collisions, octree, kd tree ?
// actor AI interface
// sprite animation, state machines
// rain
// perf mon 
// rpg stats and rules
// serialise/deserialise game save/load state
// convo/tree system

//#define USE3DGRID
//#define USE2DGRID

std::vector<Vertex> dumbCubeVerts = {
    {{-1.0f,-1.0f,1.0f}, {-1.0f,-1.0f,-1.0f},  {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}, {1.0f,0.0f,0.0f}},
    {{1.0f,-1.0f,1.0f}, {1.0f,0.0f,-1.0f},{1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f},{1.0f,0.0f,0.0f}},
    {{1.0f,1.0f,1.0f}, {0.0f,-1.0f,1.0f},{1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 1.0f},{1.0f,0.0f,0.0f}},
    {{-1.0f,1.0f,1.0f}, {-1.0f,0.0f,-1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}, {1.0f,0.0f,0.0f}},

    {{-1.0f,-1.0f,-1.0f}, {-1.0f,0.0f,1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}},
    {{1.0f,-1.0f,-1.0f}, {0.0f,1.0f,1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}},
    {{1.0f,1.0f,-1.0f}, {-1.0f,0.0f,-1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f},{1.0f,0.0f,0.0f}},
    {{-1.0f,1.0f,-1.0f}, {1.0f,-1.0f,1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}, {0.0f, 1.0f},{1.0f,0.0f,0.0f}}
};

glm::vec3 cubePositions[] = {
    glm::vec3(0.0f,  0.0f,  0.0f),
    glm::vec3(2.0f,  5.0f, -15.0f),
    glm::vec3(-1.5f, -2.2f, -2.5f),
    glm::vec3(-3.8f, -2.0f, -12.3f),
    glm::vec3(2.4f, -0.4f, -3.5f),
    glm::vec3(-1.7f,  3.0f, -7.5f),
    glm::vec3(1.3f, -2.0f, -2.5f),
    glm::vec3(1.5f,  2.0f, -2.5f),
    glm::vec3(1.5f,  0.2f, -1.5f),
    glm::vec3(-1.3f,  1.0f, -1.5f)
};

std::vector<uint16_t> cubeindices = {
    0, 1, 2, 2, 3, 0, //front
    0, 4, 1, 4, 5, 1, //top
    2, 3, 6, 6, 7, 3, //bottom
    0, 4, 3, 3, 4, 7, //right (looking at)
    2, 1, 5, 2, 5, 6, //left looking at)
    6, 5, 7, 7, 5, 4  //back
};

std::vector<uint16_t> indices = {
    0, 1, 2, 2, 3, 0
    //4, 5, 6, 6, 7, 4
};
//VkBuffer cubevertexBuffer;
//VkDeviceMemory cubevertexBufferMemory;
//VkBuffer cubeindexBuffer;
//VkDeviceMemory cubeindexBufferMemory;

std::vector<uint16_t> lineindices{};
std::vector<VertexLineList> linerenderverts{};

unsigned int overlaythrottle = 0;
float fps_cap = 144.0f;
float target_frame_time = 1.0f / fps_cap;

enum struct scene_state {
    init_menu,
    open_game,
    in_game_menu,
    end_game
};

glm::vec3 mousepickerloc;


std::map<std::string, Material> materials{};


class CoreVulkanApplication : VulkanAppBase {
public:
    void run() {
        initWindow();
        //createBackground();
        
        initAudio();
        syndminiwavchunk = setupwavchunk("SOUND010.WAV");
        syndgunwavchunk = setupwavchunk("SOUND008.WAV");
        //sdlaudiotest();
        initVulkan();
        createActors();
        setupImguiLayer();
        //setupParticlesLayer();
        mainLoop();
        layersCleanup();
        cleanup();
    }
    ~CoreVulkanApplication() {
        delete appImguiLayer;
        delete appPickingLayer;
        //delete appParticlesLayer;
    }

    Light sceneDirLight;
    
    imguiLayer* appImguiLayer;
    pickingLayer* appPickingLayer;
    ParticlesLayer* appParticlesLayer;

    glm::vec3 calcedmouseloc{};

    const int MAX_BULLETS = 30;
    int bulletParticleCount = 0;

    void setupParticlesLayer() {
        appParticlesLayer = new ParticlesLayer(vkDevice, swapchain, commandPool, debugoverlay->renderPass);
        appParticlesLayer->particlespipelineLayout = notexturepipelineLayout;
        appParticlesLayer->particlesgraphicsPipeline = particlesgraphicsPipeline;
        appParticlesLayer->setupCmdBuffers();  
    }

    void setupImguiLayer() {
        appImguiLayer = new imguiLayer(vkDevice, swapchain, commandPool, debugoverlay->renderPass);
        appImguiLayer->layerInit();
        appImguiLayer->setupGlfwVulkan(window);
        ImGui_ImplVulkan_SetMinImageCount(MAX_FRAMES_IN_FLIGHT);
    }

    void setupPickingLayer() {
        //picking layer needs its own simple renderpass
        VulkanRenderPipelineHelpers::createRenderPass(VK_FORMAT_R32_SFLOAT, vkDevice->physicalDevice,
            vkDevice->device, pickingRenderPass, pickable_renderpasscfg);
        appPickingLayer = new pickingLayer(vkDevice, swapchain, commandPool, pickingRenderPass);
        appPickingLayer->setupPickingImagesAndViews(gwidth, gheight);
        appPickingLayer->setupPickingFrameBuffer();
        appPickingLayer->setupPickingCommandBuffers();
        appPickingLayer->createPickingPipeline(mainDescriptorSetLayout);
    }

    void layersCleanup() {
        appImguiLayer->shutdownImgui();
        appPickingLayer->cleanupRenderLayer();
        //appParticlesLayer->cleanupRenderLayer();
    }

    std::vector<std::pair<glm::vec3, glm::vec3>> normlinepairs;

    //move to mats header
    void initMaterials() {
        materials["emerald"] = Material{ {0.0215, 0.1745, 0.0215}, {0.07568, 0.61424, 0.07568},	{0.633, 0.727811, 0.633}, {0.0f,0.0f,0.6} };
        materials["jade"] = Material{ {0.135, 0.2225, 0.1575}, {0.54, 0.89, 0.63}, {0.316228, 0.316228, 0.316228}, {0.0, 0.0, 0.1 } };
        materials["obsidian"] = Material{ {0.05375, 0.05, 0.06625}, {0.18275, 0.17, 0.22525}, {0.332741, 0.328634, 0.346435}, {0.0, 0.0, 0.3 } };
        materials["pearl"] = Material{ {0.25, 0.20725, 0.20725}, {1, 0.829, 0.829}, {0.296648, 0.296648, 0.296648}, {0.0, 0.0, 0.088} };
        materials["ruby"] = Material{ {0.1745,	0.01175,	0.01175},	{0.61424,	0.04136,	0.04136},	{0.727811,	0.626959,	0.626959},	{0.0f,0.0f,0.6} };
        materials["turquoise"] = Material{ { 0.1,	0.18725,	0.1745},	{0.396,	0.74151,	0.69102}, {0.297254,	0.30829,	0.306678},	{0.0f,0.0f,0.1} };
        materials["brass"] = Material{ {0.329412,	0.223529,	0.027451},	{0.780392,	0.568627,	0.113725},	{0.992157,	0.941176,	0.807843},	{0.0f,0.0f,0.21794872} };
        materials["bronze"] = Material{ {0.2125,	0.1275,	0.054},	{0.714,	0.4284,	0.18144},	{0.393548,	0.271906,	0.166721},	{0.0f,0.0f,0.2} };
        materials["chrome"] = Material{ { 0.25,	0.25,	0.25},	{0.4,	0.4,	0.4},	{0.774597,	0.774597,	0.774597},	{0.0f,0.0f,0.6} };
        materials["copper"] = Material{ { 0.19125,	0.0735,	0.0225},	{0.7038,	0.27048,	0.0828},	{0.256777,	0.137622,	0.086014},	{0.0f,0.0f,0.1} };
        materials["gold"] = Material{ {0.24725,	0.1995,	0.0745},	{0.75164,	0.60648,	0.22648},	{0.628281,	0.555802,	0.366065},	{0.0f,0.0f,0.4} };
        materials["silver"] = Material{ {0.19225,	0.19225,	0.19225},	{0.50754,	0.50754,	0.50754},	{0.508273,	0.508273,	0.508273},	{0.0f,0.0f,0.4} };
        materials["black plastic"] = Material{ {0.0,	0.0,	0.0},	{0.01,	0.01,	0.01},	{0.50,	0.50,	0.50},	{0.0f,0.0f,.25} };
        materials["cyan plastic"] = Material{ {0.0,	0.1,	0.06},	{0.0,	0.50980392,	0.50980392},	{0.50196078,	0.50196078,	0.50196078},	{0.0f,0.0f,.25} };
        materials["green plastic"] = Material{ {0.0,	0.0,	0.0},	{0.1,	0.35,	0.1},	{0.45,	0.55,	0.45},	{0.0f,0.0f,.25 } };
        materials["red plastic"] = Material{ {0.0,	0.0,	0.0},	{0.5,	0.0,	0.0},	{0.7,	0.6,	0.6},	{0.0f,0.0f,.25} };
        materials["white plastic"] = Material{ {0.0,	0.0,	0.0},	{0.55,	0.55,	0.55},	{0.70,	0.70,	0.70},	{0.0f,0.0f,.25 } };
        materials["yellow plastic"] = Material{ {0.0,	0.0,	0.0},	{0.5,	0.5,	0.0},	{0.60,	0.60,	0.50},	{0.0f,0.0f,.25 } };
        materials["black rubber"] = Material{ {0.02,	0.02,	0.02},	{0.01,	0.01,	0.01},	{0.4,	0.4,	0.4},	{0.0f,0.0f,.078125 } };
        materials["cyan rubber"] = Material{ {0.0,	0.05,	0.05},	{0.4,	0.5,	0.5},	{0.04,	0.7,	0.7},	{0.0f,0.0f,.078125 } };
        materials["green rubber"] = Material{ {0.0,	0.05,	0.0},	{0.4,	0.5,	0.4},	{0.04,	0.7,	0.04},	{0.0f,0.0f,.078125 } };
        materials["red rubber"] = Material{ {0.05,	0.0,	0.0},	{0.5,	0.4,	0.4},	{0.7,	0.04,	0.04},	{0.0f,0.0f,.078125 } };
        materials["white rubber"] = Material{ {0.05,	0.05,	0.05},	{0.5,	0.5,	0.5},	{0.7,	0.7,	0.7}, {0.0, 0.0, 0.78125 } };
        materials["yellow rubber"] = Material{ {0.05,	0.05,	0.0},	{0.5,	0.5,	0.4},	{0.7,	0.7,	0.04}, {0.0, 0.0, 0.78125 } };
    }

    Actor3<Vertex> meshActorFromGLTFModel(std::string modelName, glm::vec3 position) {
        std::string err;
        std::string warn;

        tinygltf::Model inmodel;
        tinygltf::TinyGLTF gltfLoader;

        std::vector<Vertex> modelverts;
        std::vector<uint16_t> modelindices;

        bool ret = gltfLoader.LoadASCIIFromFile(&inmodel, &err, &warn, "E:/source/biogamedev2/vkMeshProj2/models/" + modelName + ".gltf");
        if (!ret) {
            std::cout << "issues loading model" << std::endl;
            std::cout << "warns: " << warn << std::endl;
            std::cout << "errs: " << err << std::endl;
        }
        else {
            std::cout << "loaded model ok" << std::endl;
        }

        std::cout << "image nums: " << inmodel.images.size() << std::endl;
        std::cout << "texture nums: " << inmodel.textures.size() << std::endl;

        auto cubemesh = inmodel.meshes[0];
        //inmodel.nodes[0].mesh;
        std::cout << "mesh name: " << cubemesh.name << std::endl;
        //cubemesh.primitives[0].indices;
        std::cout << "mode: " << cubemesh.primitives[0].mode << std::endl;

        std::cout << "node count: " << inmodel.scenes[0].nodes.size() << std::endl;
        auto primitive = cubemesh.primitives[0];
        std::cout << "primt pos sec: " << primitive.attributes.find("POSITION")->second << std::endl;
        auto posvertaccessor = inmodel.accessors[primitive.attributes.find("POSITION")->second];

        //primitive.
        //position vertices
        auto bufferview = inmodel.bufferViews[posvertaccessor.bufferView];
        float* positionBuffer = nullptr;
        std::cout << "vert accessor byteoffset: " << posvertaccessor.byteOffset << std::endl;
        std::cout << "vert bufferview byteoffset: " << bufferview.byteOffset << std::endl;
        std::cout << "vert in buffer chars size: " << inmodel.buffers[0].data.size() << std::endl;
        positionBuffer = reinterpret_cast<float*>(&(inmodel.buffers[bufferview.buffer].data[posvertaccessor.byteOffset + bufferview.byteOffset]));
        
        //normals
        auto normalaccessor = inmodel.accessors[primitive.attributes.find("NORMAL")->second];
        float* normBuffer = nullptr;
        auto normalbufferview = inmodel.bufferViews[normalaccessor.bufferView];
        normBuffer = reinterpret_cast<float*>(&(inmodel.buffers[normalbufferview.buffer].data[normalaccessor.byteOffset + normalbufferview.byteOffset]));

        std::cout << "norm accessor count: " << normalaccessor.count << std::endl;
        //for (size_t i = 0; i < normalaccessor.count; ++i) {
        //    std::cout << "(" << normBuffer[i * 3 + 0] << ", "// x
        //        << normBuffer[i * 3 + 1] << ", " // y
        //        << normBuffer[i * 3 + 2] << ")" // z
        //        << "\n";

        //    auto pos0 = glm::vec3(positionBuffer[i * 3 + 0], positionBuffer[i * 3 + 1], positionBuffer[i * 3 + 2]);
        //    auto pos1 = glm::vec3(normBuffer[i * 3 + 0], normBuffer[i * 3 + 1], normBuffer[i * 3 + 2]);
        //    normlinepairs.push_back({ pos0,pos1 });
        //}
        //tex uvs
        auto texuvaccessor = inmodel.accessors[primitive.attributes.find("TEXCOORD_0")->second];
        float* texuvBuffer = nullptr;
        std::cout << "tex uv accessor byteoffset: " << texuvaccessor.byteOffset << std::endl;
        std::cout << "tex uv bufferview byteoffset: " << bufferview.byteOffset << std::endl;
        //std::cout << "tex uv in buffer chars size: " << inmodel.buffers[0].data.size() << std::endl;
        auto texuvbufferview = inmodel.bufferViews[texuvaccessor.bufferView];
        texuvBuffer = reinterpret_cast<float*>(&(inmodel.buffers[texuvbufferview.buffer].data[texuvaccessor.byteOffset + texuvbufferview.byteOffset]));
        //

        //int col = 0; int row = 0;
        //for (int i = 0; i < 24*3; ++i) {
        //    //for (int j = 0; j < 3; ++j) {

        //        std::cout << i << ": " << std::fixed << std::setprecision(3) << positionBuffer[i] << " ";
        //        row++;
        //        if (row == 3) {
        //            col++;
        //            row = 0;
        //            std::cout << std::endl;
        //        }
        //        if(col == 8){
        //            std::cout << std::endl;
        //            col = 0;
        //        }
        //    //}
        //    //std::cout << std::endl;
        //}

        auto vertexCount = posvertaccessor.count;

        std::cout << "pos vert count: " << vertexCount << std::endl;

        for (size_t v = 0; v < vertexCount; v++) {
            Vertex vert{};
            vert.pos = glm::vec3(glm::make_vec3(&positionBuffer[v * 3]));
            vert.normal = glm::vec3(glm::make_vec3(&normBuffer[v * 3])); 
            //vert.pos.x *= 0.1; //scaling it down 0.1
            //vert.pos.y *= 0.1;
            //vert.pos.z *= 0.1;
            //vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
            //vert.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
            vert.color = glm::vec4(0.2f, 0.3f, 0.3f, 1.0f);

            auto texuv2 = glm::vec2(glm::make_vec2(&texuvBuffer[v * 2]));
            //std::cout << "texuv x: " << texuv2.x << " texuv y: " << texuv2.y << std::endl;

            vert.texCoord = texuv2; // glm::vec2(0.0f, 0.0f);
            modelverts.push_back(vert);
        }



        auto texuvCount = texuvaccessor.count;
        std::cout << "texuv count: " << texuvCount << std::endl;
        /*for (size_t v = 0; v < texuvCount; v++) {

            auto texuv2 = glm::vec2(glm::make_vec2(&texuvBuffer[v * 2]));
            std::cout << "texuv: " << texuv2.x << " " << texuv2.y << std::endl;*/
            //    Vertex vert{};
            //    vert.pos = glm::vec3(glm::make_vec3(&positionBuffer[v * 2]));
            //    vert.pos.x *= 0.1;
            //    vert.pos.y *= 0.1;
            //    vert.pos.z *= 0.1;
            //    //vert.normal = glm::normalize(glm::vec3(normalsBuffer ? glm::make_vec3(&normalsBuffer[v * 3]) : glm::vec3(0.0f)));
            //    //vert.uv = texCoordsBuffer ? glm::make_vec2(&texCoordsBuffer[v * 2]) : glm::vec3(0.0f);
            //    vert.color = glm::vec4(1.0f);
            //    vert.texCoord = glm::vec2(0.0f, 0.0f);
            //    modelverts.push_back(vert);
            //}


        std::cout << "indices index: " << primitive.indices << std::endl;
        auto indaccessor = inmodel.accessors[primitive.indices];
        std::cout << "indices type: " << indaccessor.type << std::endl; //scalar
        std::cout << "indices component type: " << indaccessor.componentType << std::endl; //5123 / TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT / unit16_t
        auto indbufferview = inmodel.bufferViews[indaccessor.bufferView];
        auto indbuffer = inmodel.buffers[indbufferview.buffer];
        const uint16_t* buf = reinterpret_cast<const uint16_t*>(&indbuffer.data[indaccessor.byteOffset + indbufferview.byteOffset]);
        std::cout << "indices count: " << indaccessor.count << std::endl;
        for (size_t index = 0; index < indaccessor.count; index++) {
            modelindices.push_back(buf[index]);
        }
        std::cout << "indices got: " << modelindices.size() << std::endl;

        Actor3<Vertex> modelActor = Actor3<Vertex>(modelverts, modelindices, { 0.0f,0.0f,0.0f }, { 0.5f,0.5f,0.5f });
        modelActor.setposition(position);
        return modelActor; // Actor3<Vertex>(modelverts, modelindices, { 0.0f,0.0f,0.0f }, { 0.5f,0.5f,0.5f });
    }

    Actor3<Vertex> buildSkyBox() {
        //std::vector<uint16_t> skyindices = {
        //    0, 1, 2, 2, 3, 0, //front
        //    0, 4, 1, 4, 5, 1, //top
        //    2, 3, 6, 6, 7, 3, //bottom
        //    0, 4, 3, 3, 4, 7, //right (looking at)
        //    2, 1, 5, 2, 5, 6, //left looking at)
        //    6, 5, 7, 7, 5, 4  //back
        //};
        //float skyscale = 500.0f;
        //std::vector<Vertex> skyverts{
        //    {{-skyscale,-skyscale,skyscale}, {-1.0f,-1.0f,-1.0f},  {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {skyscale,0.0f,0.0f}},
        //    {{skyscale,-skyscale,skyscale}, {1.0f,0.0f,-1.0f},{0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, skyscale},{skyscale,0.0f,0.0f}},
        //    {{skyscale,skyscale,skyscale}, {0.0f,-1.0f,1.0f},{0.0f, 0.0f, 1.0f, 1.0f}, {skyscale, skyscale},{skyscale,0.0f,0.0f}},
        //    {{-skyscale,skyscale,skyscale}, {-1.0f,0.0f,-1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {skyscale, 0.0f}, {skyscale,0.0f,0.0f}},

        //    {{-skyscale,-skyscale,-skyscale}, {-1.0f,0.0f,1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, skyscale}, {skyscale,0.0f,0.0f}},
        //    {{skyscale,-skyscale,-skyscale}, {0.0f,1.0f,1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, skyscale}, {skyscale,0.0f,0.0f}},
        //    {{skyscale,skyscale,-skyscale}, {-1.0f,0.0f,-1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, skyscale},{skyscale,0.0f,0.0f}},
        //    {{-skyscale,skyscale,-skyscale}, {1.0f,-1.0f,1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}, {0.0f, skyscale},{skyscale,0.0f,0.0f}}
        //};
        
        //meshActors.push_back(Actor3<Vertex>(skyverts, skyindices, { 0.0f,0.0f, 0.0f }, { 0.5f,0.5f,0.5f }));
        //meshActors.push_back(cubeBuilder({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, 500.0f, 1));
        return cubeBuilder({ 0.0f,0.0f,0.0f }, { 1.0f,1.0f,1.0f,1.0f }, 500.0f, 1);

    }

    Actor3<Vertex> cubeBuilder(glm::vec3 pos, glm::vec4 col, float cubesize, uint32_t matidx) {
        std::vector<uint16_t> cubeindices = {
            0, 1, 2, 2, 3, 0, //top
            4, 5, 6, 6, 7, 4, //front
            8, 9, 10, 10, 11, 8, //left (looking at)
            12, 13, 14, 14, 15, 12, //back
            16, 17, 18, 18, 19, 16, //right (looking at)
            20, 21, 22, 22, 23, 20  //bottom
        };

        glm::vec4 othercol{0.4f,0.1f,0.1f, 1.0f};
        glm::vec4 othercol2{ 0.3f,0.2f,0.4f, 1.0f };
        std::vector<Vertex> cubeverts{
            {{-cubesize,cubesize,-cubesize}, {0.0f,1.0f,0.0f},  othercol2, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //top +Y 0
            {{-cubesize,cubesize,cubesize}, {0.0f,1.0f,0.0f}, othercol2, {0.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{cubesize,cubesize,cubesize}, {0.0f,1.0f,0.0f}, othercol2, {1.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{cubesize,cubesize,-cubesize}, {0.0f,1.0f,0.0f}, othercol2, {1.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //top 3

            {{-cubesize,-cubesize,-cubesize}, {0.0f,0.0f,-1.0f}, othercol, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //front -Z 4
            {{-cubesize,cubesize,-cubesize}, {0.0f,0.0f,-1.0f}, othercol, {0.0f, 0.0f}, {1.0f,0.0f,0.0f}},
            {{cubesize,cubesize,-cubesize}, {0.0f,0.0f,-1.0f}, othercol, {1.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{cubesize,-cubesize,-cubesize}, {0.0f,0.0f,-1.0f}, othercol, {1.0f, 1.0f},{1.0f,0.0f,0.0f}}, //front 7

            {{-cubesize,-cubesize,cubesize}, {-1.0f,0.0f,0.0f},  col, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //left facing -X 8
            {{-cubesize,cubesize,cubesize}, {-1.0f,0.0f,0.0f}, col, {0.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{-cubesize,cubesize,-cubesize}, {-1.0f,0.0f,0.0f}, col, {1.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{-cubesize,-cubesize,-cubesize}, {-1.0f,0.0f,0.0f}, col, {1.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //left facing 11

            {{cubesize,-cubesize,cubesize}, {0.0f,0.0f,1.0f}, othercol2, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //back +Z 12
            {{cubesize,cubesize,cubesize}, {0.0f,0.0f,1.0f}, othercol2, {0.0f, 0.0f}, {1.0f,0.0f,0.0f}},
            {{-cubesize,cubesize,cubesize}, {0.0f,0.0f,1.0f}, othercol2, {1.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{-cubesize,-cubesize,cubesize}, {0.0f,0.0f,1.0f}, othercol2, {1.0f, 1.0f},{1.0f,0.0f,0.0f}}, //back 15

            {{cubesize,-cubesize,-cubesize}, {1.0f,0.0f,0.0f},  othercol2, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //right facing +X 16
            {{cubesize,cubesize,-cubesize}, {1.0f,0.0f,0.0f}, othercol2, {0.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{cubesize,cubesize,cubesize}, {1.0f,0.0f,0.0f}, othercol2, {1.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{cubesize,-cubesize,cubesize}, {1.0f,0.0f,0.0f}, othercol2, {1.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //right 19

            {{-cubesize,-cubesize,cubesize}, {0.0f,-1.0f,0.0f}, othercol, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //bottom -Y 20
            {{-cubesize,-cubesize,-cubesize}, {0.0f,-1.0f,0.0f}, othercol, {0.0f, 0.0f}, {1.0f,0.0f,0.0f}},
            {{cubesize,-cubesize,-cubesize}, {0.0f,-1.0f,0.0f}, othercol, {1.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{cubesize,-cubesize,cubesize}, {0.0f,-1.0f,0.0f}, othercol, {1.0f, 1.0f},{1.0f,0.0f,0.0f}} //bottom 23
        };
        Actor3<Vertex> ret = Actor3<Vertex>(cubeverts, cubeindices, { 0.0f,0.0f,0.0f }, { 0.5f,0.5f,0.5f }, matidx);
        ret.setposition(pos);
        return ret;
    }

    Actor3<Vertex> planeBuilder(glm::vec3 pos, glm::vec4 col, float size, uint32_t matidx) {
        std::vector<uint16_t> planeindices = {
            0, 1, 2, 2, 3, 0 //top/bot only
        };

        std::vector<Vertex> planeverts{
            {{-size,0.0f,-size}, {0.0f,1.0f,0.0f},  col, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}}, //top +Y 0
            {{-size,0.0f,size}, {0.0f,1.0f,0.0f}, col, {0.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{size,0.0f,size}, {0.0f,1.0f,0.0f}, col, {1.0f, 0.0f},{1.0f,0.0f,0.0f}},
            {{size,0.0f,-size}, {0.0f,1.0f,0.0f}, col, {1.0f, 1.0f}, {1.0f,0.0f,0.0f}} //top 3
        };

        Actor3<Vertex> ret = Actor3<Vertex>(planeverts, planeindices, { 0.0f,0.0f,0.0f }, { 0.5f,0.5f,0.5f }, matidx);
        ret.setposition(pos);
        return ret;


    }

    Actor3<Vertex> buildLightBox() {
        std::vector<uint16_t> lightindices = {
            0, 1, 2, 2, 3, 0, //front
            0, 4, 1, 4, 5, 1, //top
            2, 3, 6, 6, 7, 3, //bottom
            0, 4, 3, 3, 4, 7, //right (looking at)
            2, 1, 5, 2, 5, 6, //left looking at)
            6, 5, 7, 7, 5, 4  //back
        };
        float lightboxsize = 0.2f;
        std::vector<Vertex> lightverts{
            {{-lightboxsize,-lightboxsize,lightboxsize}, {-1.0f,-1.0f,-1.0f},  {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 0.0f}, {1.0f,0.0f,0.0f}},
            {{lightboxsize,-lightboxsize,lightboxsize}, {1.0f,0.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f},{1.0f,0.0f,0.0f}},
            {{lightboxsize,lightboxsize,lightboxsize}, {0.0f,-1.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 1.0f},{1.0f,0.0f,0.0f}},
            {{-lightboxsize,lightboxsize,lightboxsize}, {-1.0f,0.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {1.0f, 0.0f}, {1.0f,0.0f,0.0f}},

            {{-lightboxsize,-lightboxsize,-lightboxsize}, {-1.0f,0.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}},
            {{lightboxsize,-lightboxsize,-lightboxsize}, {0.0f,1.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f}, {1.0f,0.0f,0.0f}},
            {{lightboxsize,lightboxsize,-lightboxsize}, {-1.0f,0.0f,-1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f},{1.0f,0.0f,0.0f}},
            {{-lightboxsize,lightboxsize,-lightboxsize}, {1.0f,-1.0f,1.0f}, {1.0f, 1.0f, 1.0f, 1.0f}, {0.0f, 1.0f},{1.0f,0.0f,0.0f}}
        };
        return Actor3<Vertex>(lightverts, lightindices, { 0.0f,0.0f,0.0f }, { 0.5f,0.5f,0.5f });
    }

    void createActors() {

        sceneDirLight = Light{};
        glm::vec3 lightDir = glm::normalize(-sceneDirLight.direction);

        /*for (auto& vert : dumbCubeVerts) {
            vert.pos = vert.pos * 0.2f;
        }*/
        //we should move to loading most of this stuff in from file
        //bata = new BatPlayerActor2(dumbCubeVerts, cubeindices, { 0.0f,0.0f }, {1.0f,1.0f,1.0f});
        //bata->state = 1;

        //chngActor = new Actor2<Vertex>(GameRect{ 0.0f, -1.5f, 1.0f, 1.0f }, { 0.0f,0.0f });
        //gameactors.push_back(chngActor);

        //pickables.push_back(Actor2<Vertex>(GameRect{ 1.5f, -1.5f, 0.25f, 0.25f }, { 0.0f,0.0f }, {0.4f,0.5f,0.7f}));
        //pickables.push_back(Actor2<Vertex>(GameRect{ 1.0f, 0.2f, 0.25f, 0.25f }, { 0.0f,0.0f }, { 0.2f,0.8f,0.1f }));

        /*for (int i = 0; i < MAX_BULLETS; i++) {
            bulletParticles.push_back(glm::vec2(0.0f, 0.0f));
        }

        for (auto& bullet : bulletParticles) {
            bullet.visible = false;
        }*/

        int k = 0;
#ifdef USE3DGRID
        int j = 0;
        for (; j < 21; j += 2) {
            
            for (int i = 0; i < 21; i += 2) {
                
                lineindices.push_back(k); lineindices.push_back(k + 1);
                linerenderverts.push_back({ {10.0f, ((float)i * 1.0f - 10.0f), ((float)j * 1.0f - 10.0f)}, {1.0f,1.0f,0.4f,1.0f} });
                linerenderverts.push_back({ {-10.0f, ((float)i * 1.0f - 10.0f), ((float)j * 1.0f - 10.0f)}, {0.0f,0.3f,0.9f,1.0f} });
                k += 2;
            }

            for (int i = 0; i < 21; i += 2) {
                lineindices.push_back(k); lineindices.push_back(k + 1);
                linerenderverts.push_back({ { ((float)i * 1.0f - 10.0f), 10.0f, ((float)j * 1.0f - 10.0f)}, {0.0f,1.0f,0.4f,1.0f} });
                linerenderverts.push_back({ { ((float)i * 1.0f - 10.0f), -10.0f,((float)j * 1.0f - 10.0f)}, {0.0f,1.0f,0.4f,1.0f} });
                k += 2;
            }

            for (int i = 0; i < 21; i += 2) {

                lineindices.push_back(k); lineindices.push_back(k + 1);
                linerenderverts.push_back({ { ((float)j * 1.0f - 10.0f), ((float)j * 1.0f - 10.0f), 10.0f}, {0.0f,0.7f,0.3f,1.0f} });
                linerenderverts.push_back({ { ((float)j * 1.0f - 10.0f), ((float)j * 1.0f - 10.0f), -10.0f}, {0.5f,1.0f,0.1f,1.0f} });
                k += 2;
            }

            for (int i = 0; i < 21; i += 2) {

                lineindices.push_back(k); lineindices.push_back(k + 1);
                linerenderverts.push_back({ { 0.0f, ((float)j * 1.0f - 10.0f), 10.0f}, {0.0f,0.7f,0.3f,1.0f} });
                linerenderverts.push_back({ { 0.0f, ((float)j * 1.0f - 10.0f), -10.0f}, {0.5f,1.0f,0.1f,1.0f} });
                k += 2;
            }
        }

#endif //USE3DGRID
#ifdef USE2DGRID
        float gridstep = 1.0f;
        //float depthval = 2.5f;
        for (int i = 0; i < 21; i++) {

            lineindices.push_back(k); lineindices.push_back(k + 1);
            linerenderverts.push_back({ {10.0f, ((float)i * gridstep - 10.0f), 0.0f}, {1.0f,1.0f,0.4f,1.0f} });
            linerenderverts.push_back({ {-10.0f, ((float)i * gridstep - 10.0f), 0.0f}, {0.0f,0.3f,0.9f,1.0f} });
            k += 2;
        }

        for (int i = 0; i < 21; i++) {

            lineindices.push_back(k); lineindices.push_back(k + 1);
            linerenderverts.push_back({ {((float)i * gridstep - 10.0f), 10.0f, 0.0f}, {1.0f,1.0f,0.4f,1.0f} });
            linerenderverts.push_back({ {((float)i * gridstep - 10.0f), -10.0f, 0.0f}, {0.0f,0.3f,0.9f,1.0f} });
            k += 2;
        }
#endif
        //L2s
        /*layer2actor = new Actor2<VertexL2>(GameRect{ 5.0f,1.0f,4.5f,3.0f }, {0.0f,0.0f});
        layer2actors.push_back(layer2actor);*/
        Actor3<Vertex> skybox = buildSkyBox();
        skybox.state = 3;
        skybox.spec_state = 3;
        meshActors.push_back(skybox);
        //actorFromGLTFModel();
        for (size_t i = 0; i < 5; ++i) {
            auto mesh = meshActorFromGLTFModel("cube4", { 0.3f * float(i * 3),0.7f * float(i * 3),0.9f * float(i * 3) });
            mesh.state = 4;
            mesh.spec_state = 4;
            meshActors.push_back(mesh);
        }

        for (size_t i = 0; i < 10; ++i) {
            auto mesh = cubeBuilder(cubePositions[i] * 3.0f + glm::vec3{0.0f,15.0f,0.0f}, { 0.0f,0.0f,0.0f,1.0f }, 1.0f, 0);
            mesh.state = 3;
            mesh.spec_state = 5;
            mesh.angle = 20.0f * i;
            meshActors.push_back(mesh);
        }

        meshActors.push_back(meshActorFromGLTFModel("anvil_simple_lowp", {10.0f,15.0f,12.0f}));
        auto texbox = cubeBuilder({ -6.0f,0.0f,0.0f }, {0.0f,0.0f,0.0f,1.0f}, 1.0f, 0);
        texbox.state = 3;
        texbox.spec_state = 5;
        meshActors.push_back(texbox);

        auto icomesh = meshActorFromGLTFModel("icosphere", { -5.0f,10.0f,6.0f });
        icomesh.state = 6;
        icomesh.spec_state = 7;
        meshActors.push_back(icomesh);

        auto texplane = planeBuilder({ 0.0f,-5.0f,0.0f }, { 0.0f,5.0f,0.0f,1.0f }, 100.0f, 0);
        texplane.state = 1;
        texplane.spec_state = 1;
        meshActors.push_back(texplane);

        light1 = buildLightBox();
        light2 = buildLightBox();
        //light2.setposition(glm::vec3{ 6.0f });
        light2.translate(glm::vec3{ 7.0f,0.0f,8.0f });
        //meshActors.push_back(cubeBuilder({ 0.0f, 0.0f, 0.0f }, {0.2f,0.9f,0.2f,1.0f}, 2.0f));
        //show box normals
        for (auto& vert : meshActors[0].renderverts) {
            lineindices.push_back(k); lineindices.push_back(k + 1);
            linerenderverts.push_back({ {vert.pos}, {1.0f,1.0f,1.0f,1.0f} });
            linerenderverts.push_back({ {vert.pos + vert.normal}, {1.0f,1.0f,1.0f,1.0f} });
            k += 2;
        }
        createActorResources();
        createActorShaderBuffersAndDescriptors();
        buildRenderObjsAndCommandBuffers();

    }

    void loadMaterials() {
        initMaterials();

        ShaderStorageBufferObject1 ssbo{};
        ssbo.mats[0].ambient = glm::vec3{0.0, 0.1, 0.06}; //mat test cyan plastic
        ssbo.mats[0].diffuse = glm::vec3{ 0.0,	0.50980392,	0.50980392 };
        ssbo.mats[0].specular = glm::vec3{ 0.50196078,	0.50196078,	0.50196078 };
        ssbo.mats[0].shininess = glm::vec3{ 0.0f, 0.0f, 0.25f };

        //ssbo.mats[1].ambient = glm::vec3{ 0.2, 0.8, 0.06 };
        //ssbo.mats[1].diffuse = glm::vec3{ 0.2, 0.8, 0.06 };  //{ 0.0,	0.50980392,	0.50980392 };
        //ssbo.mats[1].specular = glm::vec3{ 0.2, 0.8, 0.06 };  //{ 0.050196078,	0.050196078,	0.050196078 };
        //ssbo.mats[1].shininess = glm::vec3{ 0.0f, 0.0f, 1.0f };
        ssbo.mats[1] = materials["emerald"];

        //if mats data is static do we need to do it for each frame? (prob not)
        for (auto& meshA : meshActors) {
            for (int i = 0; i < 3; ++i) {
                void* dataMats;
                vkMapMemory(vkDevice->device, meshA.SSBOsMemory[i], 0, sizeof(ssbo), 0, &dataMats);
                memcpy(dataMats, &ssbo, sizeof(ssbo));
                vkUnmapMemory(vkDevice->device, meshA.SSBOsMemory[i]);
            }
        }

    }

    void createActorShaderBuffersAndDescriptors() {
        /*bata->createuniformbuffer<UniformBufferObject>(vkDevice->physicalDevice, vkDevice->device, swapchain->swapChainImages);
        bata->createSSBOs<ShaderStorageBufferObject1>(vkDevice->physicalDevice, vkDevice->device, swapchain->swapChainImages);*/
        for (auto& w : walls) {
            w.createuniformbuffer<UniformBufferObject>(vkDevice, swapchain->swapChainImages);
        }
        for (auto& p : pickables) {
            p.createuniformbuffer<UniformBufferObject>(vkDevice, swapchain->swapChainImages);
        }
        for (auto& l2actor : layer2actors) {
            l2actor->createuniformbuffer<UniformBufferObjectL2>(vkDevice, swapchain->swapChainImages);
        }
        for (auto& bp : bulletParticles) {
            bp.createuniformbuffer<UniformBufferObject>(vkDevice, swapchain->swapChainImages);
        }
        light1.createuniformbuffer<UniformBufferObject>(vkDevice, swapchain->swapChainImages);
        light2.createuniformbuffer<UniformBufferObject>(vkDevice, swapchain->swapChainImages);

        /*bata->createuniformbuffer<UniformBufferObject>(physicalDevice, device, swapChainImages);
        bata->createSSBOs<ShaderStorageBufferObject1>(physicalDevice, device, swapChainImages);*/
        //octobat2->createuniformbuffer<UniformBufferObject>(physicalDevice, device, swapChainImages);
        VulkanResourceHelpers::createUniformBuffers<UniformBufferObject>(vkDevice->physicalDevice, vkDevice->device, notextureUniformBuffers, notextureUniformBuffersMemory, swapchain->swapChainImages);

        //VulkanResourceHelpers::createSSBOs<ShaderStorageBufferObject1>(physicalDevice, device, pickingsSSBOs, pickingsSSBOsMemory, swapChainImages);
        for (auto& w : walls) {
            w.createSSBOs<ShaderStorageBufferObject1>(vkDevice, swapchain->swapChainImages);
        }
        for (auto& p : pickables) {
            p.createSSBOs<ShaderStorageBufferObject1>(vkDevice, swapchain->swapChainImages);
        }
        light1.createSSBOs<ShaderStorageBufferObject1>(vkDevice, swapchain->swapChainImages);
        light2.createSSBOs<ShaderStorageBufferObject1>(vkDevice, swapchain->swapChainImages);

        for (auto& w : walls) {
            w.createdescriptorsets4<UniformBufferObject, ShaderStorageBufferObject1>(vkDevice->device, swapchain->swapChainImages, mainDescriptorSetLayout, mainDescriptorPool,
                imageviews,
                mainTextureSampler1);
        }
        for (auto& p : pickables) {
            p.createdescriptorsets4<UniformBufferObject, ShaderStorageBufferObject1>(vkDevice->device, swapchain->swapChainImages, mainDescriptorSetLayout, mainDescriptorPool,
                imageviews,
                mainTextureSampler1);
        }
        for (auto& bp : bulletParticles) {
            bp.createdescriptorsets3<UniformBufferObject>(vkDevice->device, swapchain->swapChainImages, notextureDescriptorSetLayout, mainDescriptorPool);
        }
        light1.createdescriptorsets4<UniformBufferObject, ShaderStorageBufferObject1>(vkDevice->device, swapchain->swapChainImages, mainDescriptorSetLayout, mainDescriptorPool,
            imageviews,
            mainTextureSampler1);
        light2.createdescriptorsets4<UniformBufferObject, ShaderStorageBufferObject1>(vkDevice->device, swapchain->swapChainImages, mainDescriptorSetLayout, mainDescriptorPool,
            imageviews,
            mainTextureSampler1);
        /*for (auto& gactor : gameactors) {
            gactor->createdescriptorsets2_2<UniformBufferObject>(device, swapChainImages, mainDescriptorSetLayout, mainDescriptorPool,
                imageviews,
                mainTextureSampler1);
        }*/

        /*bata->createdescriptorsets4<UniformBufferObject, ShaderStorageBufferObject1>(vkDevice->device, swapchain->swapChainImages, mainDescriptorSetLayout, mainDescriptorPool,
            imageviews,
            mainTextureSampler1);*/
        VulkanDescriptorHelpers::createDescriptorSetsNoTextures<UniformBufferObject>(vkDevice->device, swapchain->swapChainImages, notextureDescriptorPool, notextureDescriptorSetLayout, notextureDescriptorSets,
            notextureUniformBuffers);

        for (auto& meshA : meshActors) {
            meshA.createuniformbuffer<UniformBufferObject>(vkDevice, swapchain->swapChainImages);
            meshA.createSSBOs<ShaderStorageBufferObject1>(vkDevice, swapchain->swapChainImages);
            meshA.createdescriptorsets4<UniformBufferObject, ShaderStorageBufferObject1>(vkDevice->device, swapchain->swapChainImages, mainDescriptorSetLayout, mainDescriptorPool,
                imageviews,
                mainTextureSampler1);
        }

        loadMaterials();

    }

    void buildRenderObjsAndCommandBuffers() {
        std::vector<RenderObject> main_render_objs{
            //bata->getRenderObject()
        };
        for (auto& w : walls) {
            main_render_objs.push_back(w.getRenderObject());
        }
        for (auto& p : pickables) {
            main_render_objs.push_back(p.getRenderObject());
        }

        for (auto& meshA : meshActors) {
            main_render_objs.push_back(meshA.getRenderObject());
        }
        main_render_objs.push_back(light1.getRenderObject());
        main_render_objs.push_back(light2.getRenderObject());

        //std::vector<RenderObject> layer2_render_objs{};
        std::vector<RenderObject> linelist_render_objs{};
        std::vector<RenderObject> particle_render_objs{};
        RenderObject lineobj{ linelistvertexBuffer , linelistindexBuffer, notextureDescriptorSets, lineindices, {22,22,22} };
        linelist_render_objs.push_back(lineobj);

        /*RenderObject particleobj{ particlesvertexBuffer , particlesindexBuffer, notextureDescriptorSets, particleindices, {22,22,22} };
        particle_render_objs.push_back(particleobj);*/

        VulkanCommandBufferHelpers::createCommandBuffersv4(vkDevice->device, graphicsPipeline, pipelineLayout, renderPass, commandBuffers, commandPool,
            swapchain->swapChainFramebuffers, swapchain->swapChainExtent, main_render_objs, lineslistgraphicsPipeline, notexturepipelineLayout, linelist_render_objs);
    }

    void initVulkanDevice() {
        vkDevice = new vkDeviceInstance{};
        vkDevice->createInstance(enableValidationLayers);
        VulkanHelper::setupDebugMessenger(debugMessenger, vkDevice->instance, enableValidationLayers);
        vkDevice->createSurface(window);
        vkDevice->pickPhysicalDevice();
        vkDevice->createLogicalDeviceAndSetQueues(enableValidationLayers);  
    }

    void buildSwapchain() {
        createSwapChain(window);
        createSwapchainImageViews();
    }

    void createActorResources() {
        for (auto& w : walls) {
            w.createvertexbuffer(vkDevice, commandPool);
        }
        for (auto& p : pickables) {
            p.createvertexbuffer(vkDevice, commandPool);
        }
        for (auto& l2actor : layer2actors) {
            l2actor->createvertexbuffer(vkDevice, commandPool);
        }
        //bata->createvertexbuffer(vkDevice->physicalDevice, vkDevice->device, commandPool, vkDevice->queues.graphicsQueue);
        light1.createvertexbuffer(vkDevice, commandPool);
        light1.createIndexBuffer(vkDevice, commandPool);
        light2.createvertexbuffer(vkDevice, commandPool);
        light2.createIndexBuffer(vkDevice, commandPool);

        for (auto& w : walls) {
            w.createIndexBuffer(vkDevice, commandPool);
        }
        for (auto& p : pickables) {
            p.createIndexBuffer(vkDevice, commandPool);
        }
        for (auto& l2actor : layer2actors) {
            l2actor->createIndexBuffer(vkDevice, commandPool);
        }
        for (auto& bp : bulletParticles) {
            bp.createvertexbuffer(vkDevice, commandPool);
            bp.createIndexBuffer(vkDevice, commandPool);
        }

        //bata->createIndexBuffer(vkDevice->physicalDevice, vkDevice->device, commandPool, vkDevice->queues.graphicsQueue);

        for (auto& meshA : meshActors) {
            meshA.createvertexbuffer(vkDevice, commandPool);
            meshA.createIndexBuffer(vkDevice, commandPool);
        }

        VulkanResourceHelpers::createVertexBuffer<VertexLineList>(vkDevice->physicalDevice, vkDevice->device, commandPool, vkDevice->queues.graphicsQueue, linerenderverts,
            linelistvertexBuffer, linelistvertexBufferMemory);
        VulkanResourceHelpers::createIndexBuffer(vkDevice->physicalDevice, vkDevice->device, commandPool, vkDevice->queues.graphicsQueue, lineindices, linelistindexBuffer, linelistindexBufferMemory);

    }

    void initVulkan() {
        
        initVulkanDevice();
        swapchain = new vkSwapChain{};
        buildSwapchain();
        initOrthoScreenVars();

        //recreateable
        VulkanRenderPipelineHelpers::createRenderPass(swapchain->swapChainImageFormat, vkDevice->physicalDevice, vkDevice->device, renderPass, main_layers_renderpasscfg);
        //
        VulkanDescriptorHelpers::createDescriptorSetLayout2(vkDevice->device, mainDescriptorSetLayout, 10);
        VulkanDescriptorHelpers::createDescriptorSetLayoutNoTexture(vkDevice->device, notextureDescriptorSetLayout);
        //one descriptor pool for all objects' descriptors here
        VulkanDescriptorHelpers::createDescriptorPool2(vkDevice->device, swapchain->swapChainImages, mainDescriptorPool);
        VulkanDescriptorHelpers::createDescriptorPoolnotextures(vkDevice->device, swapchain->swapChainImages, notextureDescriptorPool);
        //recreateable
        createPipelines();
        //
        VulkanHelper::createCommandPool(vkDevice->physicalDevice, vkDevice->device, vkDevice->surface, commandPool);
        //recreateable
        createDepthResourcesandFrameBuffers();

        loadTextures();
        createTextureViewList();

        //we only need one sampler really so far
        VulkanResourceHelpers::createTextureSampler(vkDevice->device, mainTextureSampler1);

        VulkanSyncHelpers::createSyncObjects(vkDevice->device, imageAvailableSemaphores, renderFinishedSemaphores,
            inFlightFences, imagesInFlight, swapchain->swapChainImages, MAX_FRAMES_IN_FLIGHT);

        setupPickingLayer();

        debugoverlay = new overlays::DebugOverlay(vkDevice->surface, vkDevice->physicalDevice, vkDevice->device, vkDevice->queues.graphicsQueue, swapchain->swapChainFramebuffers, swapchain->swapChainImageFormat,
            VulkanResourceHelpers::findDepthFormat(vkDevice->physicalDevice),&gwidth,&gheight, debug_overlay_renderpasscfg, debug_overlay_pipelinecfg, swapchain->swapChainExtent);

        updateDebugOverlay(0.0f);

        
    }

    void recreateSwapChain() {
        std::cout << "recreating swap chain" << std::endl;
        int width = 0, height = 0;
        glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        gwidth = (uint32_t)width;
        gheight = (uint32_t)height;

        vkDeviceWaitIdle(vkDevice->device);

        appPickingLayer->cleanupRenderLayer();
        cleanupSwapChain();
        buildSwapchain();
        
        //vkDestroyPipeline(vkDevice->device, appParticlesLayer->particlesgraphicsPipeline, nullptr);
        //vkDestroyPipelineLayout(vkDevice->device, appParticlesLayer->particlespipelineLayout, nullptr);

        //vkDestroyDescriptorSetLayout(vkDevice->device, appParticlesLayer->descriptorSetLayout, nullptr);
        //vkDestroyDescriptorPool(vkDevice->device, appParticlesLayer->particlesDescPool, nullptr);

        appPickingLayer->setupPickingImagesAndViews(gwidth, gheight);
        VulkanRenderPipelineHelpers::createRenderPass(swapchain->swapChainImageFormat, vkDevice->physicalDevice, vkDevice->device, renderPass, main_layers_renderpasscfg);
        VulkanRenderPipelineHelpers::createRenderPass(VK_FORMAT_R32_SFLOAT, vkDevice->physicalDevice, vkDevice->device, pickingRenderPass, pickable_renderpasscfg);

        appPickingLayer->setupPickingFrameBuffer();
        
        createPipelines();
        appPickingLayer->createPickingPipeline(mainDescriptorSetLayout);
        //appParticlesLayer->pipelineLayout = notexturepipelineLayout;
        /*appParticlesLayer->particlespipelineLayout = notexturepipelineLayout;
        appParticlesLayer->particlesgraphicsPipeline = particlesgraphicsPipeline;*/
        

        createDepthResourcesandFrameBuffers();

        createActorShaderBuffersAndDescriptors();


        //appParticlesLayer->setupVertexAndIndexBuffers(bulletParticles);
        buildRenderObjsAndCommandBuffers();

        //arbitrary timing step value to init overlay
        updateDebugOverlay(0.1f);
    }
    // game loop stuff...
    /*float yscroll = 0.0f;
    void scrollY(float delta) {
        yscroll += delta * 0.002f;
    }*/

    int mousepickloccnter = 0;
    //bool flipthingy = false;
    double lastmousex = mousex;
    double lastmousey = mousey;
    double dx = 0.0, dy = 0.0;
    double altoffset = 0.0;

    void mainLoop() {

        //need to init last frame time somewhere to bootstrap frame counter
        std::chrono::steady_clock::time_point lastTime = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < 16; ++i) {
            if (glfwJoystickPresent(i)) {
                std::cout << "joystick present: " << i << " " << glfwGetJoystickName(i) << std::endl;
                if (glfwJoystickIsGamepad(i)) {
                    std::cout << "is gamepad" << std::endl;
                }
            }
        }

        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        if (glfwRawMouseMotionSupported()) {
            std::cout << "setting raw mouse input mode" << std::endl;
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
        }
        
        int bulletIdx = 0;
        std::chrono::steady_clock::time_point lastBulletTime = std::chrono::high_resolution_clock::now();
        auto bulletIt = bulletParticles.begin();

        //for calc mouse grid pos coordinates
        float ymultip = 8.0f; //half height in grid squares in view
        float aspectratio = (float)swapchain->swapChainExtent.width / (float)swapchain->swapChainExtent.height;
        std::cout << "ratio: " << aspectratio << std::endl;
        float xmultip = ymultip * aspectratio;
        float bulletDelay = 100.0f;

        while (!glfwWindowShouldClose(window)) {
            auto thisTime = std::chrono::high_resolution_clock::now();
            float frameTime_ms = std::chrono::duration<float, std::chrono::milliseconds::period>(thisTime - lastTime).count();
            lastTime = thisTime;

            //update inputs
            glfwPollEvents();

            appImguiLayer->imguiLayerDrawRender();
            draw_data = ImGui::GetDrawData();

            float mousexNDC = ((mousex / (float)swapchain->swapChainExtent.width) * 2.0f) - 1.0f;
            float mouseyNDC = ((mousey / (float)swapchain->swapChainExtent.height) * 2.0f) - 1.0f;
            
            if (glfwGetKey(window, GLFW_KEY_TAB)) {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                /*bulletParticles.begin()->position = { bata->position.x,bata->position.y };
                bulletParticles.begin()->velocity = { 0.0f, 0.0f };*/
                /*bulletIt = bulletParticles.begin();
                for (auto it = bulletParticles.begin(); it != bulletParticles.end(); ++it) {
                    it->visible = false;
                    it->position = glm::vec2(0.0f, 0.0f);
                    it->velocity = glm::vec2(0.0f, 0.0f);
                }
                bulletDelay = 1000.0f;*/
                //std::cout << "setting bullet delay" << bulletDelay << std::endl;
                //bulletParticleCount--;
            }
            else if (glfwGetKey(window, GLFW_KEY_H)) {
                std::cout << "H" << std::endl;
            }
            else if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
                std::cout << "esc, exiting" << std::endl;
                glfwSetWindowShouldClose(window, 1);
            }

            //cam rot get deltas and rot view
            if (msctrler.buttons[0]) {
                dx += mousex - lastmousex;
                dy += mousey - lastmousey;
                float rotdamping = 0.1;
                dx *= rotdamping;
                dy *= rotdamping;
            }
            else {
                dx = 0.0f;
                dy = 0.0f;
            }



            kbctrler.glfwUpdate(window);
            ctrler.updatecontrollerstate();
            //toggle overlay
            // needs to be smarter, handle keyup/kepdown events
            if (kbctrler.o) showdebugoverlay = true;
            if (kbctrler.i) showdebugoverlay = false;

            if (kbctrler.sp) altoffset += 0.1f;
            if (kbctrler.lctrl) altoffset -= 0.1f;


            //theta=acos((v1 DOT v2)/(|v1|*|v2|))
            glm::vec2 upref{ 0.0f, 1.0f };
            
            //float lastBulletTime = thisTime;
            //if (msctrler.buttons[1]) {
            //    //std::cout << "rb pressed" << std::endl;
            //    if (std::chrono::duration<float, std::chrono::milliseconds::period>(thisTime - lastBulletTime).count() > bulletDelay) {
            //        //std::cout << "actual bullet delay: " << std::chrono::duration<float, std::chrono::milliseconds::period>(thisTime - lastBulletTime).count() << std::endl;
            //        //std::cout << "set bullet delay: " << bulletDelay << std::endl;
            //        bulletDelay = 100.0f;
            //        lastBulletTime = thisTime;
            //        //std::cout << "time passed ok" << std::endl;
            //        //if (bulletParticles.size() < MAX_BULLETS) {
            //        bulletIt = std::find_if(bulletIt, bulletParticles.end(), [](auto& bullet) {return bullet.visible == false; });
            //        if (bulletIt == bulletParticles.end()) {
            //            //click
            //        }
            //        else {
            //            //std::cout << "within bullet lims" << std::endl;
            //            bulletIt->visible = true;
            //            glm::vec2 startpos{ bata->position.x, bata->position.y };
            //            //std::cout << "mouse NDC x: " << mousexNDC * xmultip << " y:" << mouseyNDC * ymultip << std::endl;
            //            glm::vec2 targetpos{ mousexNDC * xmultip , mouseyNDC * ymultip };
            //            glm::vec2 bulletvec = glm::normalize(targetpos - startpos) * 0.4f;
            //            bulletIt->position = startpos;
            //            bulletIt->velocity = bulletvec;
            //            if (syndgunwavchunk) {
            //                Mix_PlayChannel(-1, syndgunwavchunk, 0);
            //            }
            //            else {
            //                std::cout << "no sound yo!" << std::endl;
            //            }
            //        }
            //            
            //            //create/display a new bullet particle where player is and give it a velocity
            //            //Actor2<VertexLineList> newBullet(glm::vec2(bata->position.x, bata->position.y));
            //            //this all needs to be more efficient
            //            
            //            //bulletParticles.push_back(newBullet);
            //            
            //            
            //            //bulletParticles[bulletIdx].position = bulletParticles[bulletIdx].position + bulletvec; // glm::vec2(0.2f, 0.2f);
            //            //bulletParticles.begin()->velocity = bulletvec;
            //            //bulletParticleCount++;
            //        //}
            //    }
            //}
            //for (auto& bullet : bulletParticles) {
            //    bullet.position = bullet.position + bullet.velocity;
            //}
            
            //auto mags = (glm::length(upref) * glm::length(mouselookvec));
            //auto dots = glm::dot(mouselookvec, upref);
            //float batobjx = (bata->position.x / halfviewwidth); // *2.0f) - 1.0f;
            //float batobjy = (bata->position.y / halfviewheight); // *2.0f) - 1.0f;
            //glm::vec2 mouselookvec = glm::normalize(glm::vec2(mousexNDC - batobjx, mouseyNDC - batobjy));
            //auto anglerads = glm::angle(glm::normalize(upref), mouselookvec);
            //auto theta = glm::degrees(anglerads);
            //if (mousexNDC < batobjx)
            //    theta = 180.0f + (180.0f - theta);

            /*mousepickloccnter++;
            if (mousepickloccnter > 300) {
                std::cout << "obj pos x: " << bata->position.x << " obj pos y: " << bata->position.y << std::endl;
                std::cout << "obj loc x: " << batobjx << " obj loc y: " << batobjy << std::endl;
                std::cout << "viewwidthx: " << viewwidth << " viewheight: " << viewheight << std::endl;
                std::cout << "mspickloc: " << glm::to_string(mousepickerloc) << std::endl;
                std::cout << "ms calced loc: " << glm::to_string(calcedmouseloc) << std::endl;
                std::cout << "ms pic vec mag: " << glm::length(mousepickerloc) << std::endl;
                std::cout << "theta: " << theta << std::endl;
                mousepickloccnter = 0;
            }*/

            /*if ((theta >= 0.0f && theta <= 22.5f) || (theta > 337.5f && theta <= 360.0f)) {
                bata->state = 1;
            } 
            else if (theta > 22.5f && theta <= 67.5f) {
                bata->state = 3;
            }
            else if (theta > 67.5f && theta <= 112.5f) {
                bata->state = 4;
            }
            else if (theta > 112.5f && theta <= 157.5f) {
                bata->state = 5;
            }
            else if (theta > 157.5f && theta <= 202.5f) {
                bata->state = 6;
            }
            else if (theta > 202.5f && theta <= 247.5f) {
                bata->state = 7;
            }
            else if (theta > 247.5f && theta <= 292.5f) {
                bata->state = 8;
            }
            else if (theta > 292.5f && theta <= 337.5f) {
                bata->state = 9;
            }*/
            
            //glm::vec2 nextpos = bata->getnextposition(kbctrler, ctrler, frameTime_ms);
            //auto collfunc = [&](Actor2<Vertex> w) { return AABBCheckCollision(bata->getAABBgamerect(), w.getAABBgamerect()); };
            //if (auto it = std::find_if(walls.begin(), walls.end(), collfunc); it != walls.end()) {
            //    //do nothing it would collide if moved with wall?
            //    //bata->setposition
            //}
            //else {
                //bata->updateposition(kbctrler, ctrler, frameTime_ms);
            //}
            ////octobat2->updateposition(kbctrler, ctrler, frameTime_ms);
            //
            //for (auto& w : walls) {
            //    if (AABBCheckCollision(bata->getAABBgamerect(), w.getAABBgamerect())) {
            //        glm::vec2 coldir = getAABBCollisionRectRectDirection(bata->getAABBgamerect(), w.getAABBgamerect(), kbctrler, ctrler);
            //        bata->position = bata->position + (coldir * 0.02f);
            //        //std::cout << "hit a wall rect: " << w.gamerect.posx << " " << w.gamerect.posy << " " << w.gamerect.sizex << " " << w.gamerect.sizey << std::endl;
            //    }
            //}

            /*glm::vec3 lookingvec = worldviewvec - lookatcentrevec;
            if (kbctrler.a || ctrler.ctrlrstate.dpadleft) {
                worldviewvec.x += 0.05 * frameTime_ms;
                lookatcentrevec.x += 0.05 * frameTime_ms;
            }
            if (kbctrler.d || ctrler.ctrlrstate.dpadright) {
                worldviewvec.x -= 0.05 * frameTime_ms;
                lookatcentrevec.x -= 0.05 * frameTime_ms;
            }

            if (kbctrler.s || ctrler.ctrlrstate.dpaddown) {
                worldviewvec.z -= 0.05 * frameTime_ms;
                lookatcentrevec.z -= 0.05 * frameTime_ms;
            }
            if (kbctrler.w || ctrler.ctrlrstate.dpadup) {
                worldviewvec.z += 0.05 * frameTime_ms;
                lookatcentrevec.z += 0.05 * frameTime_ms;
            }*/

            //replace with some collideables

            //for (auto& ball : balls) {
            //    ball->updateposition(frameTime_ms);

            //    if (ball->position.y < -4.1f && ball->velocity.y < 0.0f) {
            //        ball->velocity.y = 0.0f - ball->velocity.y;
            //    }
            //    else if (ball->position.y > 4.1f && ball->velocity.y > 0.0f) {
            //        ball->velocity.y = (0.0f - ball->velocity.y) * 0.95f;
            //    }

            //    if (ball->position.x < -7.4f && ball->velocity.x < 0.0f) {
            //        ball->velocity.x = 0.0f - ball->velocity.x;
            //    }
            //    else if (ball->position.x > 7.4f && ball->velocity.x > 0.0f) {
            //        ball->velocity.x = 0.0f - ball->velocity.x;
            //    }

            //    if (AABBCheckCollision(bata->getAABBgamerect(), ball->getAABBgamerect())) {
            //        AABBCollisionBatBallDirection(*ball, *bata, kbctrler, ctrler);
            //    }
            //    //if (AABBCheckCollision(octobat2->getAABBgamerect(), ball->getAABBgamerect())) {

            //    //    physics::PolygonCollisionResult polyres = physics::PolygonCollision(
            //    //        octobat2->getCollPolygon(), ball->getCollPolygon(),
            //    //        ball->velocity);
            //    //    if (polyres.Intersect) {
            //    //        octobat2->state = octobat2->state == 0 ? 1 : 0;
            //    //        /*countthing++;
            //    //        std::cout << "oct an ball coll: " << countthing << polyres.MinimumTranslationVector.x << " "
            //    //            << polyres.MinimumTranslationVector.y << std::endl;*/

            //    //        //TODO polygon intersection/collision works fine, subsequent translation/bounce is a bit weird
            //    //        //ball->position += polyres.MinimumTranslationVector * 4.5f;
            //    //        //ball->translate(polyres.MinimumTranslationVector * ball->velocity);
            //    //        ball->velocity = polyres.MinimumTranslationVector * -0.2f ; // *-0.5f;
            //    //        //ball->yvel = polyres.MinimumTranslationVector.y;
            //    //    }
            //    //}

            //    if (AABBCheckCollision(chngActor->getAABBgamerect(), ball->getAABBgamerect())) {
            //        AABBCollisionBatBallDirection(*ball, *chngActor, kbctrler, ctrler);
            //        chngActor->state = chngActor->state == 0 ? 1 : 0;
            //        /*if (syndminiwavchunk)
            //            Mix_PlayChannel(-1, syndminiwavchunk, 0);*/
            //    }
            //}

            //physics::collideAABBBalls(balls);
            //..
            //this will turn into some O(n^2) type thing need quad/octtree
            /*if (AABBCheckCollision(bata->gamerect, balla2->gamerect)) {
                AABBCollisionBatBallDirection(*balla2, *bata);
            }
            if (AABBCheckCollision(wall1a->gamerect, balla2->gamerect)) {
                AABBCollisionWallBallDirection(*balla2, *wall1a);
            }
            
            if (AABBCheckCollision(wallt->gamerect, balla2->gamerect)) {
                AABBCollisionWallBallDirection(*balla2, *wallt);
            }*/
            //...

            lastmousex = mousex;
            lastmousey = mousey;
            //this will be more important when we're controlling camera
            updateWorldViewVec(frameTime_ms);
            auto compTime = std::chrono::high_resolution_clock::now();
            auto stepTime_ms = std::chrono::duration<float, std::chrono::milliseconds::period>(compTime - thisTime);
            std::this_thread::sleep_for(6.944ms - stepTime_ms);
            drawFrame(frameTime_ms);
        }

        
        auto err = vkDeviceWaitIdle(vkDevice->device);
        check_vk_result(err);

        vkDeviceWaitIdle(vkDevice->device);
    }

    glm::vec3 modelmouseloc(glm::vec3 P, glm::vec3 C) {
        glm::vec3 ret{};
        glm::vec3 prmd = P - C;
        //prevent div by z
        if (prmd.z == 0.0f)
            return { 0.0f, 0.0f, 0.0f };
        float l = P.z / prmd.z;
        ret = C + (l * prmd);

        return ret;
    }

    //int matdispcounter = 0;
    int ssbotimer = 0;
    float timeincr = 0.0f;
    float timestep = 0.2f;
    float lightposx = 0.0f;
    float lightposy = 0.0f;
    float lightposz = 0.0f;
    //glm::vec3 lightpos2 = light2.position; //{ -20.0f,5.0f,8.0f };
    //float yaw = -90.0f;
    //float pitch = 0.0f;
    float xoffset = 0.0f;
    float yoffset = 0.0f;

    void updateUniformBuffer(uint32_t currentImage) {
        //matdispcounter++;
        //ubo
        UniformBufferObject ubo{};
        ShaderStorageBufferObject1 ssbo{};
        UniformBufferObjectL2 ubol2{};
        
        ubo.lightpos2 = light2.position;
        
        if (timeincr > 30.0f) {
            timestep = -0.2f;
        }
        else if (timeincr <= -30.0f) {
            timestep = 0.2f;
        }
        timeincr += timestep;
        //std::cout << "timeincr: " << timeincr << std::endl;

        //world view and proj setup
        
        xoffset -= dx;
        yoffset -= dy;
        float pitch = 0.0f;
        float yaw = -90.0f;
        pitch += yoffset;
        yaw += xoffset;

        if (pitch > 110.0f)
            pitch = 110.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        //std::cout << "yaw: " << yaw << std::endl;
        

        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)); // Note that we convert the angle to radians first
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw));
        glm::vec3 cameraFront = glm::normalize(direction);

        glm::vec3 cameraUp = glm::vec3(0.0f, -1.0f, 0.0f);
        const float cameraSpeed = 0.05f; // adjust accordingly
        if (kbctrler.w)
            worldviewvec += cameraSpeed * cameraFront;
        if (kbctrler.s)
            worldviewvec -= cameraSpeed * cameraFront;
        if (kbctrler.a)
            worldviewvec -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (kbctrler.d)
            worldviewvec += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;

        worldviewvec.y = 0.0f + altoffset;

        ubo.view = glm::lookAt(worldviewvec, worldviewvec + cameraFront, cameraUp);
        ubo.cameraFront = cameraFront;
        /*ubo.view = glm::mat4(1.0f);
        ubo.view = glm::translate(ubo.view, glm::vec3(0.0f, 0.0f, -50.0f));
        ubo.view = glm::rotate(ubo.view, (float)glm::radians(dx), glm::vec3(0.0f, 1.0f, 0.0f));
        ubo.view = glm::rotate(ubo.view, (float)glm::radians(dy), glm::vec3(1.0f, 0.0f, 0.0f));*/
        ubo.proj = glm::perspective(glm::radians(65.0f) + perspectivedelta, (float)swapchain->swapChainExtent.width / (float)swapchain->swapChainExtent.height, 0.1f, 1500.0f);
        ubo.viewpos = worldviewvec;
        //ubo.imgstate = 0;
        //ubo.visstate = 6;

        lightposx = timeincr;
        lightposy = timeincr * 0.3;
        lightposz = timeincr * 1.5;
        //ubo.model = glm::translate(glm::mat4(1.0f), gridshiftvec);
        ubo.lightpos = glm::vec3(0.0f + lightposx,-1.0f + lightposy, -8.0f + lightposz);
        ubo.CLQ1 = glm::vec3(1.0f,0.06f,0.016f);
        //std::cout << "lightposx: " << lightposx << std::endl;
        //std::cout << "timeincr: " << timeincr << std::endl;
        //default world view ubo for lines and pickables etc
        /*void* datalines;
        vkMapMemory(vkDevice->device, notextureUniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &datalines);
        memcpy(datalines, &ubo, sizeof(ubo));
        vkUnmapMemory(vkDevice->device, notextureUniformBuffersMemory[currentImage]);*/

        //Bat, player(s) should really be just another actor for drawing
        //ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3((0.0f + bata->position.x ), (0.0f + bata->position.y), (0.0f + bata->zpos)));
        //ubo.imgstate = 6; // bata->state;
        /*void* dataBat;
        vkMapMemory(vkDevice->device, bata->uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &dataBat);
        memcpy(dataBat, &ubo, sizeof(ubo));
        vkUnmapMemory(vkDevice->device, bata->uniformBuffersMemory[currentImage]);*/
        //float zdiff = 10.0f - worldviewvec.z;
        //
        /*ubol2.proj = ubo.proj;
        ubol2.testval = 1;
        ubol2.view = ubo.view;
        ubol2.model = ubo.model;*/
        ubo.model = glm::mat4(1.0f);

        ubo.ambient = glm::vec3{0.0, 0.1, 0.06}; //mat test cyan plastic
        ubo.diffuse = glm::vec3{ 0.0,	0.50980392,	0.50980392 };
        ubo.specular = glm::vec3{ 0.50196078,	0.50196078,	0.50196078 };
        //ubo.shininess = 0.25f;

        void* datalines;
        vkMapMemory(vkDevice->device, notextureUniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &datalines);
        memcpy(datalines, &ubo, sizeof(ubo));
        vkUnmapMemory(vkDevice->device, notextureUniformBuffersMemory[currentImage]);

        for (auto& w : walls) {
            ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3((0.0f + w.position.x), (0.0f + w.position.y), 0.2f));
            ubo.imgstate = w.state;
            void* dataWall;
            vkMapMemory(vkDevice->device, w.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &dataWall);
            memcpy(dataWall, &ubo, sizeof(ubo));
            vkUnmapMemory(vkDevice->device, w.uniformBuffersMemory[currentImage]);
        }

        //ShaderStorageBufferObject1 ssbo2{};
        //ssbo2.ambient = glm::vec3{ 1.0, 0.1, 0.06 }; //mat test cyan plastic
        //ssbo2.diffuse = glm::vec3{ 0.0,	0.50980392,	0.50980392 };
        //ssbo2.specular = glm::vec3{ 0.50196078,	0.50196078,	0.50196078 };
        //ssbo2.shininess = 0.25f;

        for (auto& meshA : meshActors) {
            ubo.model = glm::translate(glm::mat4(1.0f), meshA.position);
            ubo.model = glm::rotate(ubo.model, glm::radians(meshA.angle), glm::vec3{ 1.0f, 0.3f, 0.5f });
            ubo.imgstate = meshA.state;
            ubo.visstate = meshA.spec_state;
            ubo.matIdx = 0; // meshA.materialIndex;
            void* dataMeshA;
            vkMapMemory(vkDevice->device, meshA.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &dataMeshA);
            memcpy(dataMeshA, &ubo, sizeof(ubo));
            vkUnmapMemory(vkDevice->device, meshA.uniformBuffersMemory[currentImage]);


            /*void* dataMats;
            vkMapMemory(vkDevice->device, meshA.SSBOsMemory[currentImage], 0, sizeof(ssbo2), 0, &dataMats);
            memcpy(dataMats, &ssbo2, sizeof(ssbo2));
            vkUnmapMemory(vkDevice->device, meshA.SSBOsMemory[currentImage]);*/

        }
        void* dataLight1;
        ubo.model = glm::translate(glm::mat4(1.0f), ubo.lightpos);
        vkMapMemory(vkDevice->device, light1.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &dataLight1);
        memcpy(dataLight1, &ubo, sizeof(ubo));
        vkUnmapMemory(vkDevice->device, light1.uniformBuffersMemory[currentImage]);

        void* dataLight2;
        ubo.model = glm::translate(glm::mat4(1.0f), light2.position); // ubo.lightpos2);
        vkMapMemory(vkDevice->device, light2.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &dataLight2);
        memcpy(dataLight2, &ubo, sizeof(ubo));
        vkUnmapMemory(vkDevice->device, light2.uniformBuffersMemory[currentImage]);

        /*ssbo.model = ubo.model;
        ssbo.view = ubo.view;
        ssbo.proj = ubo.proj;
        ssbo.IDs = glm::vec3(113.0f);*/
        //ubo.imgstate = 1;

        //for (auto& p : pickables) {
        //    //ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3((0.0f + p.position.x), (0.0f + p.position.y), 0.2f));
        //    //ubo.testval = p.state;
        //    ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3((0.0f + p.position.x), (0.0f + p.position.y), 0.2f));
        //    void* dataPickubo;
        //    vkMapMemory(vkDevice->device, p.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &dataPickubo);
        //    memcpy(dataPickubo, &ubo, sizeof(ubo));
        //    vkUnmapMemory(vkDevice->device, p.uniformBuffersMemory[currentImage]);

        //    if (ssbotimer > 300) {
        //        void* dataPickssbo;
        //        vkMapMemory(vkDevice->device, p.SSBOsMemory[currentImage], 0, sizeof(ssbo), 0, &dataPickssbo);
        //        memcpy(&ssbo, dataPickssbo, sizeof(ssbo));
        //        vkUnmapMemory(vkDevice->device, p.SSBOsMemory[currentImage]);
        //        std::cout << "IDs: " << glm::to_string(ssbo.IDs) << std::endl;
        //    }
        //}

        /*for (auto& bp : bulletParticles) {
            ubo.model = glm::translate(glm::mat4(1.0f), glm::vec3((0.0f + bp.position.x), (0.0f + bp.position.y), 0.0f));
            void* dataBPubo;
            vkMapMemory(vkDevice->device, bp.uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &dataBPubo);
            memcpy(dataBPubo, &ubo, sizeof(ubo));
            vkUnmapMemory(vkDevice->device, bp.uniformBuffersMemory[currentImage]);
        }*/

        /*if (ssbotimer > 300)
            ssbotimer = 0;
        
        ssbotimer++;*/
    }


    void drawFrame(float frameTime_ms) {

#ifdef LOGGING
        Logger.log(std::to_string(frameTime_ms));
#endif

        vkWaitForFences(vkDevice->device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

        uint32_t imageIndex;
        VkResult result = vkAcquireNextImageKHR(vkDevice->device, swapchain->swapChain, UINT64_MAX, imageAvailableSemaphores[currentFrame], VK_NULL_HANDLE, &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }
        else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("failed to acquire swap chain image!");
        }
        updateUniformBuffer(imageIndex);

        if (imagesInFlight[imageIndex] != VK_NULL_HANDLE) {
            vkWaitForFences(vkDevice->device, 1, &imagesInFlight[imageIndex], VK_TRUE, UINT64_MAX);
        }
        imagesInFlight[imageIndex] = inFlightFences[currentFrame];

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[currentFrame] };
        VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;

        //imgui
        appImguiLayer->FrameRender(draw_data, imageIndex);
        //...

        std::vector<VkCommandBuffer> dispatchcommandBuffers = {
            commandBuffers[imageIndex]
        };
        submitInfo.commandBufferCount = 1;
        if (msctrler.buttons[0]) {
            appPickingLayer->buildPickingCommandBuffers(pickables, imageIndex, mousex, mousey);
            dispatchcommandBuffers.push_back(appPickingLayer->pickingCommandBuffers[imageIndex]);
            submitInfo.commandBufferCount++;
        }
        if (showdebugoverlay) {
            overlaythrottle++;
            if (overlaythrottle > 30) {
                updateDebugOverlay(frameTime_ms);
                overlaythrottle = 0;
            }
            dispatchcommandBuffers.push_back(debugoverlay->cmdBuffers[imageIndex]);
            submitInfo.commandBufferCount++;
        }
        //bullet in flight
        //if (msctrler.buttons[0]) {
            /*bulletParticles[0].vertex.pos.x += 0.1f;
            bulletParticles[0].model = glm::translate(bulletParticles[0].model,
                glm::vec3(bulletParticles[0].vertex.pos.x, bulletParticles[0].vertex.pos.y, 0.0f));
            appParticlesLayer->updatebulletparticleSSBOs(bulletParticles, imageIndex);*/
        //}
        //appParticlesLayer->buildFrameCmdBuffers(bulletParticles, imageIndex);
        //dispatchcommandBuffers.push_back(appParticlesLayer->particlesCommandBuffers[imageIndex]);
        //submitInfo.commandBufferCount++;
        //

        dispatchcommandBuffers.push_back(appImguiLayer->imguiCommandBuffers[imageIndex]);
        submitInfo.commandBufferCount++;

        submitInfo.pCommandBuffers = dispatchcommandBuffers.data();

        VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[currentFrame] };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;

        vkResetFences(vkDevice->device, 1, &inFlightFences[currentFrame]);

        VkResult subres = vkQueueSubmit(vkDevice->queues.graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]);
        if (subres != VK_SUCCESS) {
            std::cout << "sub res: " << subres << std::endl;
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;

        VkSwapchainKHR swapChains[] = { swapchain->swapChain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapChains;
        presentInfo.pImageIndices = &imageIndex;

        result = vkQueuePresentKHR(vkDevice->queues.presentQueue, &presentInfo);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized) {
            framebufferResized = false;
            recreateSwapChain();
        }
        else if (result != VK_SUCCESS) {
            throw std::runtime_error("failed to present swap chain image!");
        }

        currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
    }
};

int main() {
    CoreVulkanApplication app;

    try {
        app.run();
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
