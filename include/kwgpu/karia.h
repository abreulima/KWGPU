#ifndef KARIA_H
#define KARIA_H

#include <webgpu/webgpu.h>
#include <SDL3/SDL.h>

#include <kwgpu/entity_manager.h>
#include <kwgpu/shader_manager.h>
#include <kwgpu/mesh_manager.h>
#include <kwgpu/systems/system_manager.h>

class Karia
{
    public:

    // WebGPU Related
    WGPUInstance instance;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUQueue queue;
    WGPUSurface surface;
    WGPUTextureFormat format;

    // SDL Related
    SDL_Window *window;
    SDL_Event event;
    bool *key_states;

    // General
    bool is_running;

    // G Buffer
    WGPUTexture albedo;
    WGPUTexture normal;
    WGPUTexture depth;

    WGPUTextureView albedo_view;
    WGPUTextureView normal_view;
    WGPUTextureView depth_view;

    // ECS
    EntityManager entity_manager;
    SystemManager system_manager;

    ShaderManager shader_manager;
    MeshManager mesh_manager;

    // Methods
    Karia();
    void Start();
    void Update();
    void Draw();
    void End();
    void Load();

    // Async Requests
    void static RequestAdapter(WGPURequestAdapterStatus status, WGPUAdapter adapter, WGPUStringView message, void *userdata1, void *userdata2);
    void static RequestDevice(WGPURequestDeviceStatus status, WGPUDevice device, WGPUStringView message, void *userdata1, void *userdata2);

    // Helper
    WGPUTextureView GetNextTextureView();
    WGPURenderPipeline CreateRenderPipeline();
};


#endif
