#ifndef SHADER_MANAGER_H
#define SHADER_MANAGER_H

#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/vector_float4.hpp"
#include "kwgpu/sprite_manager.h"
#include <memory>
#include <string>
#include <unordered_map>

#include <webgpu/webgpu.h>

struct ShaderData
{
    WGPURenderPipeline pipeline;
    WGPUBuffer uniform_buffer;
    WGPUBindGroup bind_group;
    WGPUBindGroupLayout bind_group_layout;
};

struct Uniforms
{
    glm::mat4 mvp;
};

#endif

class ShaderManager
{
    private:
        WGPUDevice device = nullptr;
        SpriteManager *sprite_manager = nullptr;

    public:
        std::unordered_map<std::string, ShaderData> pipelines;

        void SetDevice(WGPUDevice device);
        void SetSpriteManager(SpriteManager *sprite_manager);

        ShaderData GetShader(std::string name);
        WGPUBindGroup GetBindGroup(std::string name);
        WGPUBuffer GetUniformBuffer(std::string name);

        ShaderData CreateShader(std::string name, const char *shader_code, WGPUTextureFormat format);
        ShaderData CreateShaderFromFile(std::string name, std::string path, WGPUTextureFormat format);

        // Experimental
        WGPUBuffer CreateUniformBuffer();
        WGPUBindGroup CreateBindGroup();
};
