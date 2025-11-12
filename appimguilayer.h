#pragma once


//imgui stuff

struct imguiLayer : RenderLayer {

    imguiLayer(
        vkDeviceInstance* vkDevice_,
        vkSwapChain* swapChain_,
        VkCommandPool& commandPool_,
        VkRenderPass& renderPass_)
        : RenderLayer(vkDevice_, swapChain_, commandPool_, renderPass_){}

    ~imguiLayer() {}

    std::vector<VkCommandBuffer> imguiCommandBuffers;
    VkDescriptorPool imguiDescPool;

    void layerInit() override;


    void setupImgui() {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        // Setup Dear ImGui style
        ImGui::StyleColorsDark();
        //ImGui::StyleColorsLight();

        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

        setupImguiCmdBuffers();
        createImguiDescriptorPool();
    }

    void setupImguiCmdBuffers() {
        imguiCommandBuffers.resize(swapChain->swapChainFramebuffers.size());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)imguiCommandBuffers.size();

        if (vkAllocateCommandBuffers(vkDevice->device, &allocInfo, imguiCommandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("failed to allocate imgui command buffers!");
        }
    }

    void createImguiDescriptorPool() {
        // Setup Platform/Renderer backends
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };

        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000;
        pool_info.poolSizeCount = std::size(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;


        vkCreateDescriptorPool(vkDevice->device, &pool_info, nullptr, &imguiDescPool);
    }

    void setupGlfwVulkan(
        GLFWwindow* window
    ) {
        ImGui_ImplGlfw_InitForVulkan(window, true);

        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = vkDevice->instance;
        init_info.PhysicalDevice = vkDevice->physicalDevice;
        init_info.Device = vkDevice->device;
        init_info.Queue = vkDevice->queues.graphicsQueue;
        init_info.DescriptorPool = imguiDescPool;
        init_info.MinImageCount = 3;
        init_info.ImageCount = 3;
        init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
        ImGui_ImplVulkan_Init(&init_info, renderPass);
    }

    void FrameRender(
        ImDrawData* draw_data,
        size_t imguiCmdBufferIdx
    ) {
        VkResult err;

        //err = vkResetCommandPool(g_Device, fd->CommandPool, 0);
                //check_vk_result(err);
        {
            VkCommandBufferBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            err = vkBeginCommandBuffer(imguiCommandBuffers[imguiCmdBufferIdx], &info);
            check_vk_result(err);
        }
        {
            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = { 0.8f, 0.3f, 0.0f, 0.0f };
            clearValues[1].depthStencil = { 1.0f, 0 };

            VkRenderPassBeginInfo info = {};
            info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            info.renderPass = renderPass;
            info.framebuffer = swapChain->swapChainFramebuffers[imguiCmdBufferIdx];
            info.renderArea.offset = { 0, 0 };
            info.renderArea.extent = swapChain->swapChainExtent;
            info.clearValueCount = static_cast<uint32_t>(clearValues.size());
            info.pClearValues = clearValues.data();
            vkCmdBeginRenderPass(imguiCommandBuffers[imguiCmdBufferIdx], &info, VK_SUBPASS_CONTENTS_INLINE);
        }

        //    // Record dear imgui primitives into command buffer
        ImGui_ImplVulkan_RenderDrawData(draw_data, imguiCommandBuffers[imguiCmdBufferIdx]);

        // Submit command buffer
        vkCmdEndRenderPass(imguiCommandBuffers[imguiCmdBufferIdx]);
        {

            err = vkEndCommandBuffer(imguiCommandBuffers[imguiCmdBufferIdx]);
            check_vk_result(err);

        }


    }

    void shutdownImgui() {
        
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        vkDestroyDescriptorPool(vkDevice->device, imguiDescPool, nullptr);
        ImGui::DestroyContext();
    }

    void imguiLayerDrawRender() {
        bool show_demo_window = true;
        bool show_another_window = false;
        ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
        ImGuiIO& io = ImGui::GetIO(); (void)io;

        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        bool thingy = true;

        if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!"); // , & thingy, ImGuiWindowFlags_NoMove);                          // Create a window called "Hello, world!" and append into it.
            //ImGui::SetWindowPos(ImVec2(1200.0f, 600.0f));

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
            ImGui::End();
        }

        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }

        ImGui::Render();
    }
};

void imguiLayer::layerInit() {
    std::cout << "called setup imgui" << std::endl;
    setupImgui();
}
