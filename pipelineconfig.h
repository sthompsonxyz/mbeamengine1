#pragma once


pipelinebindings mainpipebindattrs = VulkanRenderPipelineHelpers::getpipelinebindingattribdefinitions();
renderPassConfig main_layers_renderpasscfg{
    VK_ATTACHMENT_LOAD_OP_CLEAR,
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};
graphicsPipelineConfig main_triangles_pipelinecfg{
    "vert.spv",
    "frag.spv",
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    mainpipebindattrs,
    false,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD
};

pipelinebindings pickable_pipebindattrs = VulkanRenderPipelineHelpers::getpipelinebindingattribdefinitions();
renderPassConfig pickable_renderpasscfg{
    VK_ATTACHMENT_LOAD_OP_CLEAR,
    VK_IMAGE_LAYOUT_UNDEFINED,
    VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
};
graphicsPipelineConfig pickable_triangles_pipelinecfg{
    "pickingvert.spv",
    "pickingfrag.spv",
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    mainpipebindattrs,
    false,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD
};

pipelinebindings linespipebindattrs = VulkanRenderPipelineHelpers::getpipelinebindingattribdefinitionsLists();
renderPassConfig lines_renderpasscfg{
    VK_ATTACHMENT_LOAD_OP_LOAD,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};
graphicsPipelineConfig lines_pipelinecfg{
    "linesvert.spv",
    "linesfrag.spv",
    VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
    linespipebindattrs,
    false,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_MAX
};

pipelinebindings layer2pipebindattrs = VulkanRenderPipelineHelpers::getpipelinebindingattribdefinitions();
renderPassConfig layer2_renderpasscfg{
    VK_ATTACHMENT_LOAD_OP_LOAD,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};
graphicsPipelineConfig layer2_triangles_pipelinecfg{
    "layer2vert.spv",
    "layer2frag.spv",
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
    layer2pipebindattrs,
    false,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_MAX
};

pipelinebindings particlepipebindattrs = VulkanRenderPipelineHelpers::getpipelinebindingattribdefinitionsLists();
renderPassConfig particle_renderpasscfg{
    VK_ATTACHMENT_LOAD_OP_LOAD,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};
graphicsPipelineConfig particle_points_pipelinecfg{
    "particlesvert.spv",
    "particlesfrag.spv",
    VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    particlepipebindattrs,
    false,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_MAX
};

pipelinebindings bulletparticlepipebindattrs = VulkanRenderPipelineHelpers::getpipelinebindingattribdefinitionsLists();
renderPassConfig bulletparticle_renderpasscfg{
    VK_ATTACHMENT_LOAD_OP_LOAD,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};
graphicsPipelineConfig bulletparticle_points_pipelinecfg{
    "bulletparticlesvert.spv",
    "bulletparticlesfrag.spv",
    VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
    bulletparticlepipebindattrs,
    false,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_MAX
};

pipelinebindings debugpipebindattrs = overlays::getdebugpipelinepipelinebindingattribdefinitions();
renderPassConfig debug_overlay_renderpasscfg{
    VK_ATTACHMENT_LOAD_OP_LOAD,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
    VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};
graphicsPipelineConfig debug_overlay_pipelinecfg{
    "textoverlay/text.vert.spv",
    "textoverlay/text.frag.spv",
    VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
    debugpipebindattrs,
    true,
    VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT,
    VK_BLEND_FACTOR_SRC_ALPHA,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_OP_ADD,
    VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
    VK_BLEND_FACTOR_ZERO,
    VK_BLEND_OP_ADD
};