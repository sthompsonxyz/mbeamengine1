#pragma once


#ifdef LOGGING
#include "logger.hpp"

//logger Logger{};

#endif // LOGGING

#include "controller.h"
#include "kbctrler.h"
#include "Actors.h"

using Actors::BatPlayerActor2;
using Actors::RBatPlayerActor2;
using Actors::Actor3;

double mousex = 0.0f, mousey = 0.0f;

int y = 0;

struct mousestate {
    std::vector<bool> buttons{ false,false,false,false,false,false,false };
    short int scroll = 0;
};

mousestate msctrler{};

//some of these things need to be globals due to glfw callbacks not capturing
void mouseButtonCallBack(GLFWwindow* window, int button, int action, int mods) {
    msctrler.buttons[button] = action == 1 ? true : false;
}

void mousePosCallBack(GLFWwindow* window, double xpos, double ypos) {
    mousex = xpos; mousey = ypos;
}

struct VulkanAppBase {

    GLFWwindow* window;

    vkDeviceInstance* vkDevice;

    vkSwapChain* swapchain;

    /*VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;*/

    uint32_t gwidth = 2800;
    uint32_t gheight = 1600;
    bool framebufferResized = false;

    //for imgui image
    ImDrawData* draw_data;
    //..
    overlays::DebugOverlay* debugoverlay = nullptr;

    bool showdebugoverlay = false;
    ps4Controller ctrler{};
    KeyboardControls kbctrler{};
    glm::vec3 worldviewvec{ 0.0f,0.0f,-30.0f };
    glm::vec3 lookatcentrevec{ 0.0f,0.0f,0.0f };
    glm::vec3 gridshiftvec{ 0.0f,0.0f,0.0f };

    float viewwidth;
    float viewheight;
    float halfviewwidth;
    float halfviewheight;

    VkRenderPass renderPass;
    //VkDescriptorSetLayout descriptorSetLayout;
    VkDescriptorSetLayout mainDescriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkDescriptorSetLayout notextureDescriptorSetLayout;
    VkPipelineLayout notexturepipelineLayout;
    VkPipeline graphicsPipeline;
    //VkPipeline l2graphicsPipeline;
    VkPipeline lineslistgraphicsPipeline;

    VkCommandPool commandPool;

    VkDescriptorPool mainDescriptorPool;
    VkDescriptorPool notextureDescriptorPool;
    std::vector<VkDescriptorSet> mainDescriptorSets;
    std::vector<VkDescriptorSet> notextureDescriptorSets;
    std::vector<VkBuffer> notextureUniformBuffers;
    std::vector<VkDeviceMemory> notextureUniformBuffersMemory;

    VkImage depthImage;
    VkDeviceMemory depthImageMemory;
    VkImageView depthImageView;

    VkSampler mainTextureSampler1;

    std::list<TextureBufferSet> textures;

    std::vector<VkImageView> imageviews;

    //BatPlayerActor2* bata;
    //RBatPlayerActor2* octobat2;
    //physics::Quadtree* quadtree;

    //std::list<Actor2<Vertex>*> balls;
    std::list<Actor3<Vertex>*> gameactors;
    std::list<Actor3<VertexL2>*> layer2actors;
    std::list<Actor3<VertexLineList>*> linelistactors;
    std::list<Actor3<Vertex> > walls;
    std::list<Actor3<Vertex> > pickables;
    std::list<Actor3<VertexLineList>> bulletParticles;
    std::vector<Actor3<Vertex> > meshActors{};
    Actor3<Vertex>* skybox;
    Actor3<Vertex> light1;
    Actor3<Vertex> light2;

    //Actor2<Vertex>* chngActor;

    //L2
    //Actor2<VertexL2>* layer2actor;
    //...
    Actor3<VertexLineList>* linelistsActor;

    VkBuffer linelistvertexBuffer;
    VkDeviceMemory linelistvertexBufferMemory;
    VkBuffer linelistindexBuffer;
    VkDeviceMemory linelistindexBufferMemory;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    std::vector<VkFence> imagesInFlight;
    size_t currentFrame = 0;

    Mix_Chunk* syndminiwavchunk;
    Mix_Chunk* syndgunwavchunk;

    VkRenderPass pickingRenderPass;

    VkPipeline particlesgraphicsPipeline;
    //particles stuff vert line list is already right type (no texture), should rename
    std::vector<VertexLineList> particles{
        {{0.5f,0.5,0.5f}, {0.5f,0.5,0.5f, 1.0f}},
        {{0.2f,-0.5,0.5f}, {0.2f,0.7,0.8f, 1.0f}}
    };
    std::vector<uint16_t> particleindices{ 0,1 };

    VkBuffer particlesvertexBuffer;
    VkDeviceMemory particlesvertexBufferMemory;
    VkBuffer particlesindexBuffer;
    VkDeviceMemory particlesindexBufferMemory;

    VkDebugUtilsMessengerEXT debugMessenger;

    float perspectivedelta{};
    glm::vec3 lookatupdelta{ 0.0f };

    void updateWorldViewVec(float delta) {
        if (kbctrler.lshift) {
            if (kbctrler.left) {
                lookatupdelta.x += 0.04f;
            }
            if (kbctrler.right) {
                lookatupdelta.x -= 0.04f;
            }
            if (kbctrler.up) {
                lookatupdelta.y += 0.04f;
            }
            if (kbctrler.down) {
                lookatupdelta.y -= 0.04f;
            }
            if (kbctrler.lbracket) {
                lookatupdelta.z += 0.04f;
            }
            if (kbctrler.rbracket) {
                lookatupdelta.z -= 0.04f;
            }
        }
        else if (kbctrler.lalt) {
            if (kbctrler.left) {
                gridshiftvec.x += 0.04f;
            }
            if (kbctrler.right) {
                gridshiftvec.x -= 0.04f;
            }
            if (kbctrler.up) {
                gridshiftvec.y += 0.04f;
            }
            if (kbctrler.down) {
                gridshiftvec.y -= 0.04f;
            }
            if (kbctrler.lbracket) {
                gridshiftvec.z += 0.04f;
            }
            if (kbctrler.rbracket) {
                gridshiftvec.z -= 0.04f;
            }
        }
        else if (kbctrler.lctrl) {
            if (kbctrler.left) {
                lookatcentrevec.x += 0.04f;
                worldviewvec.x += 0.04f;
            }
            if (kbctrler.right) {
                lookatcentrevec.x -= 0.04f;
                worldviewvec.x -= 0.04f;
            }
            if (kbctrler.up) {
                lookatcentrevec.y += 0.04f;
                worldviewvec.y += 0.04f;
            }
            if (kbctrler.down) {
                lookatcentrevec.y -= 0.04f;
                worldviewvec.y -= 0.04f;
            }
            if (kbctrler.lbracket) {
                lookatcentrevec.z += 0.04f;
                worldviewvec.z += 0.04f;
            }
            if (kbctrler.rbracket) {
                lookatcentrevec.z -= 0.04f;
                worldviewvec.z -= 0.04f;
            }
        }
        else {
            if (kbctrler.left) {
                worldviewvec[0] += 0.04f;
            }
            if (kbctrler.right) {
                worldviewvec[0] -= 0.04f;
            }
            if (kbctrler.up) {
                worldviewvec[1] += 0.04f;
            }
            if (kbctrler.down) {
                worldviewvec[1] -= 0.04f;
            }
            if (kbctrler.lbracket) {
                worldviewvec[2] += 0.04f;
            }
            if (kbctrler.rbracket) {
                worldviewvec[2] -= 0.04f;
            }
        }
        if (kbctrler.t) {
            perspectivedelta -= 0.001f;
        }
        if (kbctrler.y) {
            perspectivedelta += 0.001f;
        }
    }

    void initWindow() {
#ifdef LOGGING
        Logger.log("initialising window");
#endif
        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

        window = glfwCreateWindow(gwidth, gheight, "Vulkan", nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);

        glfwSetFramebufferSizeCallback(window, [](GLFWwindow* window, int width, int height) {
            auto app = reinterpret_cast<VulkanAppBase*>(glfwGetWindowUserPointer(window));
            app->framebufferResized = true;
            });

        glfwSetMouseButtonCallback(window,
            mouseButtonCallBack
        );

        glfwSetCursorPosCallback(window,
            mousePosCallBack);

        glfwSetScrollCallback(window,
            [](GLFWwindow* window, double xoffset, double yoffset) {
                std::cout << "mouse scroll x: " << xoffset << " y: " << yoffset << std::endl;
            });

    }

    void updateDebugOverlay(float frametime)
    {
        debugoverlay->beginTextUpdate();

        debugoverlay->addText("Mouse!", 20.0f, 5.0f, overlays::DebugOverlay::alignLeft);
        debugoverlay->addText("X: " + std::to_string(mousex), 20.0f, 105.0f, overlays::DebugOverlay::alignLeft);
        debugoverlay->addText("Y: " + std::to_string(mousey), 20.0f, 155.0f, overlays::DebugOverlay::alignLeft);

        std::stringstream ssl;
        ssl << "mbuttons: ";
        for (int i = 0; i < msctrler.buttons.size(); ++i) {
            ssl << (msctrler.buttons[i] ? 1 : 0) << " ";
        }
        debugoverlay->addText(ssl.str(), 20.0f, 70.0f, overlays::DebugOverlay::alignLeft);

        std::stringstream ssr;
        ssr << "frame time: " << std::to_string(frametime) << " ms";
        debugoverlay->addText(ssr.str(), 20.0f, 300.0f, overlays::DebugOverlay::alignLeft);
        //timingss.clear();
        ssr.str("");
        ssr << "fps: " << std::to_string(1000.0f / frametime);
        debugoverlay->addText(ssr.str(), 20.0f, 350.0f, overlays::DebugOverlay::alignLeft);
        //...

        //controller
        //dpad
        ssr.str("");
        ssr << "d-pad: ";
        ssr << ctrler.ctrlrstate.dpadleft ? "1" : "0";
        debugoverlay->addText(ssr.str(), 800.0f, 200.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << ctrler.ctrlrstate.dpadup ? "1" : "0";
        debugoverlay->addText(ssr.str(), 900.0f, 170.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << ctrler.ctrlrstate.dpadright ? "1" : "0";
        debugoverlay->addText(ssr.str(), 930.0f, 200.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << ctrler.ctrlrstate.dpaddown ? "1" : "0";
        debugoverlay->addText(ssr.str(), 900.0f, 230.0f, overlays::DebugOverlay::alignLeft);
        //...
        //buttons
        ssr.str("");
        ssr << ctrler.ctrlrstate.triangle ? "1" : "0";
        debugoverlay->addText(ssr.str(), 1100.0f, 170.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << ctrler.ctrlrstate.circle ? "1" : "0";
        debugoverlay->addText(ssr.str(), 1130.0f, 200.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << ctrler.ctrlrstate.cross ? "1" : "0";
        debugoverlay->addText(ssr.str(), 1100.0f, 230.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << ctrler.ctrlrstate.square ? "1" : "0";
        debugoverlay->addText(ssr.str(), 1070.0f, 200.0f, overlays::DebugOverlay::alignLeft);
        //bumpers l1/r1
        ssr.str("");
        ssr << ctrler.ctrlrstate.l1 ? "1" : "0";
        debugoverlay->addText(ssr.str(), 900.0f, 130.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << ctrler.ctrlrstate.r1 ? "1" : "0";
        debugoverlay->addText(ssr.str(), 1100.0f, 130.0f, overlays::DebugOverlay::alignLeft);
        //triggers l2/r2
        ssr.str("");
        ssr << std::to_string(ctrler.ctrlrstate.l2);
        debugoverlay->addText(ssr.str(), 900.0f, 110.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << std::to_string(ctrler.ctrlrstate.r2);
        debugoverlay->addText(ssr.str(), 1100.0f, 110.0f, overlays::DebugOverlay::alignLeft);
        //thumbs l3/r3
        ssr.str("");
        ssr << ctrler.ctrlrstate.l3 ? "1" : "0";
        debugoverlay->addText(ssr.str(), 900.0f, 300.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << ctrler.ctrlrstate.r3 ? "1" : "0";
        debugoverlay->addText(ssr.str(), 1100.0f, 300.0f, overlays::DebugOverlay::alignLeft);
        // sticks
        // L
        ssr.str("");
        ssr << std::fixed << std::setprecision(2) << ctrler.ctrlrstate.lstickLR;
        debugoverlay->addText(ssr.str(), 800.0f, 300.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << std::fixed << std::setprecision(2) << ctrler.ctrlrstate.lstickUD;
        debugoverlay->addText(ssr.str(), 900.0f, 350.0f, overlays::DebugOverlay::alignLeft);
        // R
        ssr.str("");
        ssr << std::fixed << std::setprecision(2) << ctrler.ctrlrstate.rstickLR;
        debugoverlay->addText(ssr.str(), 1200.0f, 300.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << std::fixed << std::setprecision(2) << ctrler.ctrlrstate.rstickUD;
        debugoverlay->addText(ssr.str(), 1100.0f, 350.0f, overlays::DebugOverlay::alignLeft);

        //worldvecs
        ssr.str("");
        ssr << "wv 1: " << std::to_string(worldviewvec[0]);
        debugoverlay->addText(ssr.str(), 20.0f, 390.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << "wv 2: " << std::to_string(worldviewvec[1]);
        debugoverlay->addText(ssr.str(), 20.0f, 410.0f, overlays::DebugOverlay::alignLeft);
        ssr.str("");
        ssr << "wv 3: " << std::to_string(worldviewvec[2]);
        debugoverlay->addText(ssr.str(), 20.0f, 430.0f, overlays::DebugOverlay::alignLeft);
        //...


        debugoverlay->endTextUpdate();

    }

    void initOrthoScreenVars() {
        viewwidth = (float)swapchain->swapChainExtent.width / 100.0f;
        viewheight = (float)swapchain->swapChainExtent.height / 100.0f;
        halfviewwidth = viewwidth / 2.0f;
        halfviewheight = viewheight / 2.0f;
    }

    void createPipelines() {
        //recreateable
        VulkanRenderPipelineHelpers::createPipelineLayout(vkDevice->device, { mainDescriptorSetLayout }, pipelineLayout);
        VulkanRenderPipelineHelpers::createPipelineLayout(vkDevice->device, { notextureDescriptorSetLayout }, notexturepipelineLayout);
        VulkanRenderPipelineHelpers::createGraphicsPipeline2(vkDevice->device, swapchain->swapChainExtent, pipelineLayout, renderPass, graphicsPipeline, main_triangles_pipelinecfg);
        //VulkanRenderPipelineHelpers::createGraphicsPipeline2(device, swapChainExtent, pipelineLayout, renderPass, l2graphicsPipeline, layer2_triangles_pipelinecfg);
        VulkanRenderPipelineHelpers::createGraphicsPipeline2(vkDevice->device, swapchain->swapChainExtent, notexturepipelineLayout, renderPass, lineslistgraphicsPipeline, lines_pipelinecfg);
        VulkanRenderPipelineHelpers::createGraphicsPipeline2(vkDevice->device, swapchain->swapChainExtent, notexturepipelineLayout, renderPass, particlesgraphicsPipeline, particle_points_pipelinecfg);
        
        //createPickingPipeline();
        //
    }

    void createDepthResourcesandFrameBuffers() {
        VulkanResourceHelpers::createDepthResources(vkDevice->physicalDevice, vkDevice->device, swapchain->swapChainExtent, depthImageView, depthImage, depthImageMemory);
        VulkanResourceHelpers::createFramebuffers(vkDevice->device, swapchain->swapChainFramebuffers, swapchain->swapChainImageViews, renderPass, depthImageView, swapchain->swapChainExtent);
    }

    void createTextureViewList() {
        for (auto& imageset : textures) {
            imageviews.push_back(imageset.textureImageView);
        }
    }

    void loadTextures() {
        std::string textureFilename("images.txt");
        std::ifstream texturelistfile(textureFilename);
        if (!texturelistfile.is_open()) {
            throw "couldnt open texture file";
        }
        else {
            std::cout << "opened texture file list" << std::endl;
        }

        std::string line;
        while (std::getline(texturelistfile, line)) {
            std::cout << line << std::endl;
            VkImage textureImage;
            VkDeviceMemory textureImageMemory;
            VkImageView textureImageView;
            VulkanResourceHelpers::createTextureImage(vkDevice->physicalDevice, vkDevice->device, commandPool, vkDevice->queues.graphicsQueue, textureImage, textureImageMemory, line);
            VulkanResourceHelpers::createTextureImageView(vkDevice->device, textureImage, textureImageView);
            TextureBufferSet imagebufferset{ textureImage, textureImageMemory, textureImageView };
            textures.push_back(imagebufferset);
        }
    }

    void cleanupSwapChain() {
        vkDestroyImageView(vkDevice->device, depthImageView, nullptr);
        vkDestroyImage(vkDevice->device, depthImage, nullptr);
        vkFreeMemory(vkDevice->device, depthImageMemory, nullptr);

        for (auto framebuffer : swapchain->swapChainFramebuffers) {
            vkDestroyFramebuffer(vkDevice->device, framebuffer, nullptr);
        }
        //vkDestroyFramebuffer(vkDevice->device, pickingFrameBuffer, nullptr);
        ////picking images and views
        //vkDestroyImageView(vkDevice->device, textureImageDepthPickView, nullptr);
        //vkDestroyImageView(vkDevice->device, textureImageColorPickView, nullptr);
        //vkDestroyImage(vkDevice->device, textureImageDepthPick, nullptr);
        //vkDestroyImage(vkDevice->device, textureImageColorPick, nullptr);
        //vkFreeMemory(vkDevice->device, textureImageDepthPickMemory, nullptr);
        //vkFreeMemory(vkDevice->device, textureImageColorPickMemory, nullptr);
        //...

        vkFreeCommandBuffers(vkDevice->device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        vkDestroyPipeline(vkDevice->device, graphicsPipeline, nullptr);
        //vkDestroyPipeline(device, l2graphicsPipeline, nullptr);
        vkDestroyPipeline(vkDevice->device, lineslistgraphicsPipeline, nullptr);
        vkDestroyPipeline(vkDevice->device, particlesgraphicsPipeline, nullptr);
        
        vkDestroyPipelineLayout(vkDevice->device, pipelineLayout, nullptr);
        vkDestroyPipelineLayout(vkDevice->device, notexturepipelineLayout, nullptr);
        
        vkDestroyRenderPass(vkDevice->device, renderPass, nullptr);
        

        for (auto imageView : swapchain->swapChainImageViews) {
            vkDestroyImageView(vkDevice->device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(vkDevice->device, swapchain->swapChain, nullptr);

        //actors recreatable cleanup

        //bata->recreatableResourceCleanup2(vkDevice->device);

        /*for (auto& ball : balls)
            ball->recreatableResourceCleanup(device);*/
        for (size_t i = 0; i < notextureUniformBuffers.size(); i++) {
            vkDestroyBuffer(vkDevice->device, notextureUniformBuffers[i], nullptr);
            vkFreeMemory(vkDevice->device, notextureUniformBuffersMemory[i], nullptr);
        }

        //chngActor->recreatableResourceCleanup(device);
        for (auto& w : walls) {
            w.recreatableResourceCleanup2(vkDevice->device);
        }
        for (auto& p : pickables) {
            p.recreatableResourceCleanup2(vkDevice->device);
        }
        for (auto& bp : bulletParticles) {
            bp.recreatableResourceCleanup(vkDevice->device);
        }
        for (auto& meshA : meshActors) {
            meshA.recreatableResourceCleanup2(vkDevice->device);
        }
        light1.recreatableResourceCleanup2(vkDevice->device);
        light2.recreatableResourceCleanup2(vkDevice->device);
        //layer2actor->recreatableResourceCleanup(device);
        //linelistsActor->recreatableResourceCleanup(device);
    }

    void cleanup() {
        cleanupSwapChain();

        vkDestroySampler(vkDevice->device, mainTextureSampler1, nullptr);

        for (auto& imageset : textures) {
            vkDestroyImageView(vkDevice->device, imageset.textureImageView, nullptr);
            vkDestroyImage(vkDevice->device, imageset.textureImage, nullptr);
            vkFreeMemory(vkDevice->device, imageset.textureImageMemory, nullptr);
        }

        vkDestroyDescriptorSetLayout(vkDevice->device, mainDescriptorSetLayout, nullptr);
        vkDestroyDescriptorSetLayout(vkDevice->device, notextureDescriptorSetLayout, nullptr);

        for (auto& w : walls) {
            w.vertexindexResourceCleanup(vkDevice->device);
        }

        for (auto& p : pickables) {
            p.vertexindexResourceCleanup(vkDevice->device);
        }
        for (auto& bp : bulletParticles) {
            bp.vertexindexResourceCleanup(vkDevice->device);
        }
        for (auto& meshA : meshActors) {
            meshA.vertexindexResourceCleanup(vkDevice->device);
        }
        light1.vertexindexResourceCleanup(vkDevice->device);
        light2.vertexindexResourceCleanup(vkDevice->device);

        vkDestroyBuffer(vkDevice->device, linelistindexBuffer, nullptr);
        vkFreeMemory(vkDevice->device, linelistindexBufferMemory, nullptr);
        vkDestroyBuffer(vkDevice->device, particlesindexBuffer, nullptr);
        vkFreeMemory(vkDevice->device, particlesindexBufferMemory, nullptr);

        vkDestroyDescriptorPool(vkDevice->device, mainDescriptorPool, nullptr);
        vkDestroyDescriptorPool(vkDevice->device, notextureDescriptorPool, nullptr);
        vkDestroyBuffer(vkDevice->device, linelistvertexBuffer, nullptr);
        vkFreeMemory(vkDevice->device, linelistvertexBufferMemory, nullptr);
        vkDestroyBuffer(vkDevice->device, particlesvertexBuffer, nullptr);
        vkFreeMemory(vkDevice->device, particlesvertexBufferMemory, nullptr);
        //bata->vertexindexResourceCleanup(vkDevice->device);
        //chngActor->vertexindexResourceCleanup(device);
        //layer2actor->vertexindexResourceCleanup(device);
        //...

        delete debugoverlay;
        //delete skybox;
        
        //delete bata;

        //delete chngActor;
        //delete layer2actor;


        for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
            vkDestroySemaphore(vkDevice->device, renderFinishedSemaphores[i], nullptr);
            vkDestroySemaphore(vkDevice->device, imageAvailableSemaphores[i], nullptr);
            vkDestroyFence(vkDevice->device, inFlightFences[i], nullptr);
        }

        vkDestroyCommandPool(vkDevice->device, commandPool, nullptr);

        vkDestroyDevice(vkDevice->device, nullptr);

        if (enableValidationLayers) {
            DestroyDebugUtilsMessengerEXT(vkDevice->instance, debugMessenger, nullptr);
        }

        vkDestroySurfaceKHR(vkDevice->instance, vkDevice->surface, nullptr);
        vkDestroyInstance(vkDevice->instance, nullptr);

        delete swapchain;
        delete vkDevice;

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    //swapchain stuff
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice& physdevice) {
        SwapChainSupportDetails details;

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physdevice, vkDevice->surface, &details.capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physdevice, vkDevice->surface, &formatCount, nullptr);

        if (formatCount != 0) {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physdevice, vkDevice->surface, &formatCount, details.formats.data());
        }

        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physdevice, vkDevice->surface, &presentModeCount, nullptr);

        if (presentModeCount != 0) {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physdevice, vkDevice->surface, &presentModeCount, details.presentModes.data());
        }

        return details;
    }

    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
        for (const auto& availableFormat : availableFormats) {
            if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
                return availableFormat;
            }
        }

        return availableFormats[0];
    }

    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
        for (const auto& availablePresentMode : availablePresentModes) {
            if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
                return availablePresentMode;
            }
        }

        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window) {
        if (capabilities.currentExtent.width != UINT32_MAX) {
            return capabilities.currentExtent;
        }
        else {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);

            VkExtent2D actualExtent = {
                static_cast<uint32_t>(width),
                static_cast<uint32_t>(height)
            };

            actualExtent.width = std::max(capabilities.minImageExtent.width, std::min(capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(capabilities.minImageExtent.height, std::min(capabilities.maxImageExtent.height, actualExtent.height));

            return actualExtent;
        }
    }

    VkSwapchainCreateInfoKHR generateVkSwapchainCreateInfoKHR(VkSurfaceKHR& surface, VkSurfaceFormatKHR& surfaceFormat, VkPhysicalDevice& physicalDevice,
        SwapChainSupportDetails& swapChainSupport, VkPresentModeKHR& presentMode, VkExtent2D& extent) {

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        static VkSwapchainCreateInfoKHR createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;

        createInfo.minImageCount = imageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        QueueFamilyIndices indices = VulkanHelper::findQueueFamilies(physicalDevice, surface);
        static uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };

        if (indices.graphicsFamily != indices.presentFamily) {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        return createInfo;
    }

    void createSwapchainImageViews() {
        swapchain->swapChainImageViews.resize(swapchain->swapChainImages.size());

        for (uint32_t i = 0; i < swapchain->swapChainImages.size(); i++) {
            swapchain->swapChainImageViews[i] = VulkanHelper::createImageView(vkDevice->device, swapchain->swapChainImages[i], swapchain->swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);
        }
    }

    void createSwapChain(GLFWwindow* window) {

        SwapChainSupportDetails swapChainSupport = querySwapChainSupport(vkDevice->physicalDevice);
        VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
        VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentModes);
        VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities, window);

        uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
        if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
            imageCount = swapChainSupport.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo = generateVkSwapchainCreateInfoKHR(vkDevice->surface, surfaceFormat, vkDevice->physicalDevice, swapChainSupport, presentMode, extent);

        if (vkCreateSwapchainKHR(vkDevice->device, &createInfo, nullptr, &swapchain->swapChain) != VK_SUCCESS) {
            throw std::runtime_error("failed to create swap chain!");
        }

        vkGetSwapchainImagesKHR(vkDevice->device, swapchain->swapChain, &imageCount, nullptr);
        swapchain->swapChainImages.resize(imageCount);
        vkGetSwapchainImagesKHR(vkDevice->device, swapchain->swapChain, &imageCount, swapchain->swapChainImages.data());

        swapchain->swapChainImageFormat = surfaceFormat.format;
        swapchain->swapChainExtent = extent;
    }

};