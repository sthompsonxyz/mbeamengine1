#pragma once


struct RenderLayer {
    RenderLayer(
        vkDeviceInstance* vkDevice_,
        vkSwapChain* swapChain_,
        VkCommandPool& commandPool_,
        VkRenderPass& renderPass_)
        : vkDevice(vkDevice_), swapChain(swapChain_), commandPool(commandPool_), renderPass(renderPass_) {}

    ~RenderLayer() {}
    vkDeviceInstance* vkDevice;
    vkSwapChain* swapChain;
    VkCommandPool& commandPool;
    VkRenderPass& renderPass;

    virtual void layerInit();
};

void RenderLayer::layerInit() {
    std::cout << "no ya didnae!" << std::endl;
};