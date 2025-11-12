#!/bin/bash

VULKAN_SDK_PATH=/home/sammysamsam/devel/1.3.211.0/x86_64/
${VULKAN_SDK_PATH}/bin/glslc shader.vert -o vert.spv
${VULKAN_SDK_PATH}/bin/glslc shader.frag -o frag.spv
${VULKAN_SDK_PATH}/bin/glslc picking.vert -o pickingvert.spv
${VULKAN_SDK_PATH}/bin/glslc picking.frag -o pickingfrag.spv