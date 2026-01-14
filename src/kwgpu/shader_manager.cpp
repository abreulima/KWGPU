#include <cstdint>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>

#include <kwgpu/shader_manager.h>
#include <vector>
#include <webgpu/webgpu.h>


void ShaderManager::SetDevice(WGPUDevice device)
{
    this->device = device;
}


void ShaderManager::SetSpriteManager(SpriteManager *sprite_manager)
{
    this->sprite_manager = sprite_manager;
}

ShaderData ShaderManager::GetShader(std::string name)
{
    return (pipelines[name]);
}

WGPUBindGroup ShaderManager::GetBindGroup(std::string name)
{
    return (pipelines[name].bind_group);
}

WGPUBuffer ShaderManager::GetUniformBuffer(std::string name)
{
    return (pipelines[name].uniform_buffer);
}


WGPUBuffer ShaderManager::CreateUniformBuffer()
{
    WGPUBufferDescriptor buffer_descriptor = {};
    buffer_descriptor.size = sizeof(Uniforms);
    buffer_descriptor.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;

    WGPUBuffer temp;
    return temp;

}

WGPUBindGroup ShaderManager::CreateBindGroup()
{
    WGPUBindGroupEntry bind_group_entry = {};
    bind_group_entry.binding = 0;
    //bind_group_entry.buffer // obj UniformBuffer
    bind_group_entry.size = sizeof(Uniforms);

    WGPUBindGroupDescriptor bind_group_descriptor = {};
    //bind_group_descriptor.layout
    bind_group_descriptor.entries = &bind_group_entry;

    WGPUBindGroup temp;
    return (temp);
}


ShaderData ShaderManager::CreateShaderFromFile(std::string name, std::string path, WGPUTextureFormat format)
{
    std::ifstream inf(path);

    if (!inf.is_open())
    {
        std::cerr << "Failed loading file." << path <<  std::endl;
        exit(EXIT_FAILURE);
    }

    std::string source, line;
    while (std::getline(inf, line))
    {
        source += line + "\n";
    }

    return CreateShader(name, source.c_str(), format);
}

ShaderData ShaderManager::CreateShader(std::string name, const char *shader_code, WGPUTextureFormat format)
{
    ShaderData shader_data;

    /* Uniform Buffer Creation */
    WGPUBufferDescriptor buffer_descriptor = {};
    buffer_descriptor.size = sizeof(Uniforms);
    buffer_descriptor.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
    shader_data.uniform_buffer = wgpuDeviceCreateBuffer(device, &buffer_descriptor);

    /* Bind Group Layout Creation */
    std::vector<WGPUBindGroupLayoutEntry> binding_layout_entries(3);

    // Uniform Buffer
    binding_layout_entries[0].binding = 0;
    binding_layout_entries[0].visibility = WGPUShaderStage_Vertex | WGPUShaderStage_Fragment;
    binding_layout_entries[0].buffer.type = WGPUBufferBindingType_Uniform;
    binding_layout_entries[0].buffer.minBindingSize = sizeof(Uniforms);

    // Texture View
    binding_layout_entries[1].binding = 1;
    binding_layout_entries[1].visibility = WGPUShaderStage_Fragment;
    binding_layout_entries[1].texture.sampleType = WGPUTextureSampleType_Float;
    binding_layout_entries[1].texture.viewDimension = WGPUTextureViewDimension_2D;

    // Sampler Texture
    binding_layout_entries[2].binding = 2;
    binding_layout_entries[2].visibility = WGPUShaderStage_Fragment;
    binding_layout_entries[2].sampler.type = WGPUSamplerBindingType_Filtering;

    WGPUBindGroupLayoutDescriptor bind_group_layout_descriptor = {};
    bind_group_layout_descriptor.entryCount = (uint32_t)binding_layout_entries.size();
    bind_group_layout_descriptor.entries = binding_layout_entries.data();
    shader_data.bind_group_layout = wgpuDeviceCreateBindGroupLayout(device, &bind_group_layout_descriptor);

    // Create Bind group

    // Shader Module
    WGPUShaderModuleDescriptor shader_module_descriptor = {};
    shader_module_descriptor.nextInChain = nullptr;

    WGPUShaderSourceWGSL shader_source_wgsl = {};
    shader_source_wgsl.chain.next = nullptr;
    shader_source_wgsl.chain.sType = WGPUSType_ShaderSourceWGSL;

    shader_module_descriptor.nextInChain = &shader_source_wgsl.chain;
    shader_source_wgsl.code = {shader_code, WGPU_STRLEN};

    WGPUShaderModule shader_module = {};
    shader_module = wgpuDeviceCreateShaderModule(device, &shader_module_descriptor);

    // Create Pipeline Layout
    WGPUPipelineLayoutDescriptor pipeline_layout_descriptor = {};
    pipeline_layout_descriptor.bindGroupLayoutCount = 1;
    pipeline_layout_descriptor.bindGroupLayouts = &shader_data.bind_group_layout;
    WGPUPipelineLayout pipeline_layout = wgpuDeviceCreatePipelineLayout(device, &pipeline_layout_descriptor);

    // RenderPipeline
    WGPURenderPipelineDescriptor render_pipeline_descriptor = {};
    render_pipeline_descriptor.layout = pipeline_layout;
    render_pipeline_descriptor.nextInChain = nullptr;

    WGPUVertexBufferLayout vertex_buffer_layout = {};
    std::vector<WGPUVertexAttribute> vertex_attributes(3);

    // Position
    vertex_attributes[0].shaderLocation = 0; // @location(0)
    vertex_attributes[0].format = WGPUVertexFormat_Float32x3;
    vertex_attributes[0].offset = 0;

    // Texture Coordinates
    vertex_attributes[1].shaderLocation = 1; // @location(1)
    vertex_attributes[1].format = WGPUVertexFormat_Float32x2;
    vertex_attributes[1].offset = 3 * sizeof(float);

    // Normal
    vertex_attributes[2].shaderLocation = 2; // @location (2)
    vertex_attributes[2].format = WGPUVertexFormat_Float32x3;
    vertex_attributes[2].offset = 5 * sizeof(float);

    vertex_buffer_layout.attributeCount = static_cast<uint32_t>(vertex_attributes.size());
    vertex_buffer_layout.attributes = vertex_attributes.data();

    vertex_buffer_layout.arrayStride = 8 * sizeof(float);
    vertex_buffer_layout.stepMode = WGPUVertexStepMode_Vertex;

    render_pipeline_descriptor.vertex.bufferCount = 1;
    render_pipeline_descriptor.vertex.buffers = &vertex_buffer_layout;

    render_pipeline_descriptor.vertex.module = shader_module;
    render_pipeline_descriptor.vertex.entryPoint = {"vs_main", WGPU_STRLEN};
    render_pipeline_descriptor.vertex.constantCount = 0;
    render_pipeline_descriptor.vertex.constants = nullptr;

    render_pipeline_descriptor.primitive.topology = WGPUPrimitiveTopology_TriangleList;
    render_pipeline_descriptor.primitive.stripIndexFormat = WGPUIndexFormat_Undefined;
    render_pipeline_descriptor.primitive.frontFace = WGPUFrontFace_CCW;
    render_pipeline_descriptor.primitive.cullMode = WGPUCullMode_None;

    WGPUFragmentState fragment_state = {};
    fragment_state.module = shader_module;
    fragment_state.entryPoint = {"fs_main", WGPU_STRLEN};
    fragment_state.constantCount = 0;
    fragment_state.constants = nullptr;

    WGPUBlendState blend_state = {};
    blend_state.color.srcFactor = WGPUBlendFactor_SrcAlpha;
    blend_state.color.dstFactor = WGPUBlendFactor_OneMinusSrcAlpha;
    blend_state.color.operation = WGPUBlendOperation_Add;

    blend_state.alpha.srcFactor = WGPUBlendFactor_Zero;
    blend_state.alpha.dstFactor = WGPUBlendFactor_One;
    blend_state.alpha.operation = WGPUBlendOperation_Add;

    WGPUColorTargetState color_target_state = {};
    color_target_state.format = format;
    color_target_state.blend = &blend_state;
    color_target_state.writeMask = WGPUColorWriteMask_All;

    fragment_state.targetCount = 1;
    fragment_state.targets = &color_target_state;

    render_pipeline_descriptor.fragment = &fragment_state;

    // Depth Testing
    WGPUDepthStencilState depth_stencil_state = {};
    depth_stencil_state.format = WGPUTextureFormat_Depth24Plus;
    depth_stencil_state.depthWriteEnabled = WGPUOptionalBool_True;
    depth_stencil_state.depthCompare = WGPUCompareFunction_Less;
    depth_stencil_state.stencilReadMask = 0;
    depth_stencil_state.stencilWriteMask = 0;

    render_pipeline_descriptor.depthStencil = &depth_stencil_state;

    render_pipeline_descriptor.multisample.count = 1;
    render_pipeline_descriptor.multisample.mask = ~0u;
    render_pipeline_descriptor.multisample.alphaToCoverageEnabled = false;

    shader_data.pipeline = wgpuDeviceCreateRenderPipeline(device, &render_pipeline_descriptor);

    // Add to unordered map
    pipelines[name] = shader_data;
    return (shader_data);

}
