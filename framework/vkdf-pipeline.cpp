#include "vkdf-pipeline.hpp"

VkPipeline
vkdf_create_gfx_pipeline(VkdfContext *ctx,
                         VkPipelineCache *pipeline_cache,
                         uint32_t num_vi_bindings,
                         VkVertexInputBindingDescription *vi_bindings,
                         uint32_t num_vi_attribs,
                         VkVertexInputAttributeDescription *vi_attribs,
                         bool enable_depth_test,
                         VkCompareOp depth_compare_op,
                         VkRenderPass render_pass,
                         VkPipelineLayout pipeline_layout,
                         VkPrimitiveTopology primitive,
                         VkCullModeFlagBits cull_mode,
                         uint32_t num_color_attachments,
                         const VkPipelineShaderStageCreateInfo *vs_info,
                         const VkPipelineShaderStageCreateInfo *fs_info)
{
   VkPipeline pipeline;

   // Vertex input
   VkPipelineVertexInputStateCreateInfo vi;
   vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vi.pNext = NULL;
   vi.flags = 0;
   vi.vertexBindingDescriptionCount = num_vi_bindings;
   vi.pVertexBindingDescriptions = vi_bindings;
   vi.vertexAttributeDescriptionCount = num_vi_attribs;
   vi.pVertexAttributeDescriptions = vi_attribs;

   // Input assembly
   VkPipelineInputAssemblyStateCreateInfo ia;
   ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   ia.pNext = NULL;
   ia.flags = 0;
   ia.primitiveRestartEnable = VK_FALSE;
   ia.topology = primitive;

   // Viewport (Dynamic)
   VkPipelineViewportStateCreateInfo vp = {};
   vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
   vp.pNext = NULL;
   vp.flags = 0;
   vp.viewportCount = 1;
   vp.scissorCount = 1;
   vp.pScissors = NULL;
   vp.pViewports = NULL;

   // Rasterization
   VkPipelineRasterizationStateCreateInfo rs;
   rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   rs.pNext = NULL;
   rs.flags = 0;
   rs.polygonMode = VK_POLYGON_MODE_FILL;
   rs.cullMode = cull_mode;
   rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
   rs.depthClampEnable = VK_FALSE;
   rs.rasterizerDiscardEnable = VK_FALSE;
   rs.depthBiasEnable = VK_FALSE;
   rs.depthBiasConstantFactor = 0;
   rs.depthBiasClamp = 0;
   rs.depthBiasSlopeFactor = 0;
   rs.lineWidth = 1.0f;

   // Multisampling
   VkPipelineMultisampleStateCreateInfo ms;
   ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   ms.pNext = NULL;
   ms.flags = 0;
   ms.pSampleMask = NULL;
   ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
   ms.sampleShadingEnable = VK_FALSE;
   ms.alphaToCoverageEnable = VK_FALSE;
   ms.alphaToOneEnable = VK_FALSE;
   ms.minSampleShading = 0.0;

   // Depth / Stencil
   VkPipelineDepthStencilStateCreateInfo ds;
   ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
   ds.pNext = NULL;
   ds.flags = 0;
   ds.depthTestEnable = enable_depth_test;
   ds.depthWriteEnable = enable_depth_test;
   ds.depthCompareOp = depth_compare_op;
   ds.depthBoundsTestEnable = VK_FALSE;
   ds.minDepthBounds = 0;
   ds.maxDepthBounds = 0;
   ds.stencilTestEnable = VK_FALSE;
   ds.back.failOp = VK_STENCIL_OP_KEEP;
   ds.back.passOp = VK_STENCIL_OP_KEEP;
   ds.back.compareOp = VK_COMPARE_OP_ALWAYS;
   ds.back.compareMask = 0;
   ds.back.reference = 0;
   ds.back.depthFailOp = VK_STENCIL_OP_KEEP;
   ds.back.writeMask = 0;
   ds.front = ds.back;

   // Blending
   VkPipelineColorBlendAttachmentState att_state[32];
   for (uint32_t i = 0; i < num_color_attachments; i++) {
      att_state[i].colorWriteMask = 0xf;
      att_state[i].blendEnable = VK_FALSE;
      att_state[i].alphaBlendOp = VK_BLEND_OP_ADD;
      att_state[i].colorBlendOp = VK_BLEND_OP_ADD;
      att_state[i].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
      att_state[i].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
      att_state[i].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
      att_state[i].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
   }

   VkPipelineColorBlendStateCreateInfo cb;
   cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
   cb.flags = 0;
   cb.pNext = NULL;
   cb.attachmentCount = num_color_attachments;
   cb.pAttachments = att_state;
   cb.logicOpEnable = VK_FALSE;
   cb.logicOp = VK_LOGIC_OP_COPY;
   cb.blendConstants[0] = 1.0f;
   cb.blendConstants[1] = 1.0f;
   cb.blendConstants[2] = 1.0f;
   cb.blendConstants[3] = 1.0f;

   // Dynamic state (Viewport, Scissor)
   int dynamic_state_count = 0;
   VkDynamicState dynamic_state_enables[VK_DYNAMIC_STATE_RANGE_SIZE];
   memset(dynamic_state_enables, 0, sizeof(dynamic_state_enables));
   dynamic_state_enables[dynamic_state_count++] =
      VK_DYNAMIC_STATE_SCISSOR;
   dynamic_state_enables[dynamic_state_count++] =
      VK_DYNAMIC_STATE_VIEWPORT;

   VkPipelineDynamicStateCreateInfo dynamic_state_info;
   dynamic_state_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   dynamic_state_info.pNext = NULL;
   dynamic_state_info.flags = 0;
   dynamic_state_info.pDynamicStates = dynamic_state_enables;
   dynamic_state_info.dynamicStateCount = dynamic_state_count;

   // Shader stages
   VkPipelineShaderStageCreateInfo shader_stages[2];
   shader_stages[0] = *vs_info;
   if (fs_info)
      shader_stages[1] = *fs_info;

   // Create pipeline
   VkGraphicsPipelineCreateInfo pipeline_info;
   pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   pipeline_info.pNext = NULL;
   pipeline_info.layout = pipeline_layout;
   pipeline_info.basePipelineHandle = VK_NULL_HANDLE;
   pipeline_info.basePipelineIndex = 0;
   pipeline_info.flags = 0;
   pipeline_info.pVertexInputState = &vi;
   pipeline_info.pInputAssemblyState = &ia;
   pipeline_info.pTessellationState = NULL;
   pipeline_info.pViewportState = &vp;
   pipeline_info.pRasterizationState = &rs;
   pipeline_info.pMultisampleState = &ms;
   pipeline_info.pDepthStencilState = &ds;
   pipeline_info.pColorBlendState = &cb;
   pipeline_info.pDynamicState = &dynamic_state_info;
   pipeline_info.pStages = shader_stages;
   pipeline_info.stageCount = fs_info ? 2 : 1;
   pipeline_info.renderPass = render_pass;
   pipeline_info.subpass = 0;

   VK_CHECK(vkCreateGraphicsPipelines(ctx->device,
                                      pipeline_cache ? *pipeline_cache : NULL,
                                      1,
                                      &pipeline_info,
                                      NULL,
                                      &pipeline));

   return pipeline;
}

VkPipeline
vkdf_create_gfx_pipeline(VkdfContext *ctx,
                         VkPipelineCache *pipeline_cache,
                         uint32_t num_vi_bindings,
                         VkVertexInputBindingDescription *vi_bindings,
                         uint32_t num_vi_attribs,
                         VkVertexInputAttributeDescription *vi_attribs,
                         bool enable_depth_test,
                         VkCompareOp depth_compare_op,
                         VkRenderPass render_pass,
                         VkPipelineLayout pipeline_layout,
                         VkPrimitiveTopology primitive,
                         VkCullModeFlagBits cull_mode,
                         uint32_t num_color_attachments,
                         VkShaderModule vs_module,
                         VkShaderModule fs_module)
{
   VkPipelineShaderStageCreateInfo vs_info, fs_info;

   vkdf_pipeline_fill_shader_stage_info(&vs_info,
                                        VK_SHADER_STAGE_VERTEX_BIT,
                                        vs_module);
   if (fs_module) {
      vkdf_pipeline_fill_shader_stage_info(&fs_info,
                                           VK_SHADER_STAGE_FRAGMENT_BIT,
                                           fs_module);
   }

   return vkdf_create_gfx_pipeline(ctx, pipeline_cache,
                                   num_vi_bindings, vi_bindings,
                                   num_vi_attribs, vi_attribs,
                                   enable_depth_test, depth_compare_op,
                                   render_pass,
                                   pipeline_layout,
                                   primitive,
                                   cull_mode,
                                   num_color_attachments,
                                   &vs_info,
                                   fs_module ? &fs_info : NULL);
}

VkPipeline
vkdf_create_depth_pipeline(VkdfContext *ctx,
                           VkPipelineCache *pipeline_cache,
                           uint32_t num_vi_bindings,
                           VkVertexInputBindingDescription *vi_bindings,
                           uint32_t num_vi_attribs,
                           VkVertexInputAttributeDescription *vi_attribs,
                           bool enable_depth_test,
                           VkCompareOp depth_compare_op,
                           VkRenderPass render_pass,
                           VkPipelineLayout pipeline_layout,
                           VkPrimitiveTopology primitive,
                           VkCullModeFlagBits cull_mode,
                           uint32_t num_color_attachments,
                           const VkPipelineShaderStageCreateInfo *vs_info,
                           const VkPipelineShaderStageCreateInfo *fs_info)
{
   VkPipeline pipeline;

   // Vertex input
   VkPipelineVertexInputStateCreateInfo vi;
   memset(&vi, 0, sizeof vi);

   vi.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
   vi.vertexBindingDescriptionCount = num_vi_bindings;
   vi.pVertexBindingDescriptions = vi_bindings;
   vi.vertexAttributeDescriptionCount = num_vi_attribs;
   vi.pVertexAttributeDescriptions = vi_attribs;

   // Input assembly
   VkPipelineInputAssemblyStateCreateInfo ia;
   memset(&ia, 0, sizeof ia);

   ia.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
   ia.topology = primitive;

   // Rasterization
   VkPipelineRasterizationStateCreateInfo rs;
   memset(&rs, 0, sizeof rs);

   rs.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
   rs.polygonMode = VK_POLYGON_MODE_FILL;
   rs.cullMode = cull_mode;
   rs.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
   rs.lineWidth = 1.0f;

   // Multisampling
   VkPipelineMultisampleStateCreateInfo ms;
   memset(&ms, 0, sizeof ms);
   ms.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
   ms.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

   // Depth / Stencil
   VkPipelineDepthStencilStateCreateInfo ds;
   memset(&ds, 0, sizeof ds);

   ds.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
   ds.depthTestEnable = VK_TRUE; //FIXME
   ds.depthWriteEnable = VK_TRUE;
   ds.depthCompareOp = depth_compare_op;
   ds.back.compareOp = VK_COMPARE_OP_NEVER;
   ds.front = ds.back;

   VkPipelineColorBlendStateCreateInfo cb;
   memset(&cb, 0, sizeof cb);
   cb.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;

   // Shader stages
   VkPipelineShaderStageCreateInfo shader_stages[2];
   shader_stages[0] = *vs_info;
   if (fs_info)
      shader_stages[1] = *fs_info;

   // Viewport (Dynamic)
   VkPipelineViewportStateCreateInfo vp;
   memset(&vp, 0, sizeof vp);
   vp.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
   vp.viewportCount = 1;
   vp.scissorCount = 1;
   // Dynamic state (Viewport, Scissor)
   int dynamic_state_count = 0;
   VkDynamicState dynamic_state_enables[VK_DYNAMIC_STATE_RANGE_SIZE];
   memset(dynamic_state_enables, 0, sizeof(dynamic_state_enables));
   dynamic_state_enables[dynamic_state_count++] =
      VK_DYNAMIC_STATE_SCISSOR;
   dynamic_state_enables[dynamic_state_count++] =
      VK_DYNAMIC_STATE_VIEWPORT;

   VkPipelineDynamicStateCreateInfo dynamic_state_info;
   memset(&dynamic_state_info, 0, sizeof dynamic_state_info);
   dynamic_state_info.sType =
      VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
   dynamic_state_info.pDynamicStates = dynamic_state_enables;
   dynamic_state_info.dynamicStateCount = dynamic_state_count;

   // Create pipeline
   VkGraphicsPipelineCreateInfo pipeline_info;
   memset(&pipeline_info, 0, sizeof pipeline_info);

   pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
   pipeline_info.layout = pipeline_layout;
   pipeline_info.pVertexInputState = &vi;
   pipeline_info.pInputAssemblyState = &ia;
   pipeline_info.pRasterizationState = &rs;
   pipeline_info.pMultisampleState = &ms;
   pipeline_info.pDepthStencilState = &ds;
   pipeline_info.pStages = shader_stages;
   pipeline_info.stageCount = fs_info ? 2 : 1;
   pipeline_info.renderPass = render_pass;

   pipeline_info.pDynamicState = &dynamic_state_info;
   pipeline_info.pViewportState = &vp;

   VK_CHECK(vkCreateGraphicsPipelines(ctx->device,
            NULL,
            1,
            &pipeline_info,
            NULL,
            &pipeline));

   return pipeline;
}

VkPipeline
vkdf_create_depth_pipeline(VkdfContext *ctx,
                           VkPipelineCache *pipeline_cache,
                           uint32_t num_vi_bindings,
                           VkVertexInputBindingDescription *vi_bindings,
                           uint32_t num_vi_attribs,
                           VkVertexInputAttributeDescription *vi_attribs,
                           bool enable_depth_test,
                           VkCompareOp depth_compare_op,
                           VkRenderPass render_pass,
                           VkPipelineLayout pipeline_layout,
                           VkPrimitiveTopology primitive,
                           VkCullModeFlagBits cull_mode,
                           uint32_t num_color_attachments,
                           VkShaderModule vs_module,
                           VkShaderModule fs_module)
{
   VkPipelineShaderStageCreateInfo vs_info, fs_info;

   vkdf_pipeline_fill_shader_stage_info(&vs_info,
         VK_SHADER_STAGE_VERTEX_BIT,
         vs_module);
   if (fs_module) {
      vkdf_pipeline_fill_shader_stage_info(&fs_info,
            VK_SHADER_STAGE_FRAGMENT_BIT,
            fs_module);
   }

   return vkdf_create_depth_pipeline(ctx, pipeline_cache,
         num_vi_bindings, vi_bindings,
         num_vi_attribs, vi_attribs,
         enable_depth_test, depth_compare_op,
         render_pass,
         pipeline_layout,
         primitive,
         cull_mode,
         0,
         &vs_info,
         fs_module ? &fs_info : NULL);
}
