#include "glm/ext/quaternion_transform.hpp"
#include "kwgpu/components.h"
#include "webgpu/webgpu.h"
#include <kwgpu/karia.h>

#include <sdl3webgpu.h>
#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext.hpp>
#include <glm/glm.hpp>

// Systems
#include <kwgpu/systems/movement.h>

#define SDL_MAIN_HANDLED

Karia::Karia()
{
    is_running = true;
    adapter = nullptr;
    device = nullptr;
}

void Karia::Start()
{
    // Start SDL Config.
    if (!SDL_Init(SDL_INIT_VIDEO))
    {
        std::cerr << "Error: Unable to initialize SDL." << std::endl;
        exit(EXIT_FAILURE);
    }

    window = SDL_CreateWindow("Hello Karia", 800, 600, 0);
    if (!window)
    {
        std::cerr << "Error: Unable to create window." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Start WebGPU Config.
    // Instance
    WGPUInstanceDescriptor instance_descriptor = {};
    instance_descriptor.nextInChain = nullptr;

    instance = wgpuCreateInstance(&instance_descriptor);
    if (!instance)
    {
        std::cerr << "Error: Unable to create instance." << std::endl;
        exit(EXIT_FAILURE);
    }

    // Surface
    surface = SDL_GetWGPUSurface(instance, window);
    if (!surface)
    {
        std::cerr << "Error: Unable to get surface." << std::endl;
        exit (EXIT_FAILURE);
    }

    // Adapter
    WGPURequestAdapterOptions request_adapter_options = {};
    request_adapter_options.compatibleSurface = surface;
    request_adapter_options.nextInChain = nullptr;

    WGPURequestAdapterCallbackInfo request_adapter_callback_info = {};
    request_adapter_callback_info.userdata1 = &adapter;
    request_adapter_callback_info.userdata2 = nullptr;
    request_adapter_callback_info.callback = RequestAdapter;

    wgpuInstanceRequestAdapter(instance, &request_adapter_options, request_adapter_callback_info);
    if (!adapter)
    {
        std::cerr << "Error: Unable to request adapter." << std::endl;
        exit (EXIT_FAILURE);
    }

    // Device
    WGPUDeviceDescriptor device_descriptor = {};
    device_descriptor.nextInChain = nullptr;

    WGPURequestDeviceCallbackInfo request_device_callback_info = {};
    request_device_callback_info.nextInChain = nullptr;
    request_device_callback_info.userdata1 = &device;
    request_device_callback_info.callback = RequestDevice;

    wgpuAdapterRequestDevice(adapter, &device_descriptor, request_device_callback_info);
    if (!device)
    {
        std::cerr << "Error: Unable to request device." << std::endl;
        exit (EXIT_FAILURE);
    }

    // Queue
    queue = wgpuDeviceGetQueue(device);

    // Surface Capabilities & Configuration
    int width;
    int height;

    SDL_GetWindowSize(window, &width, &height);

    WGPUSurfaceCapabilities surface_capabilities = {};
    wgpuSurfaceGetCapabilities(surface, adapter, &surface_capabilities);

    WGPUSurfaceConfiguration surface_configuration = {};
    surface_configuration.nextInChain = nullptr;
    surface_configuration.width = width;
    surface_configuration.height = height;
    surface_configuration.usage = WGPUTextureUsage_RenderAttachment;
    surface_configuration.format = surface_capabilities.formats[0];
    surface_configuration.presentMode = WGPUPresentMode_Fifo;
    surface_configuration.alphaMode = surface_capabilities.alphaModes[0];
    surface_configuration.viewFormatCount = 0;
    surface_configuration.viewFormats = nullptr;
    surface_configuration.device = device;

    format = surface_capabilities.formats[0];

    wgpuSurfaceConfigure(surface, &surface_configuration);

    // Init Depth Buffer
    WGPUTextureDescriptor depth_texture_descriptor = {};
    depth_texture_descriptor.size.width = width;
    depth_texture_descriptor.size.height = height;
    depth_texture_descriptor.size.depthOrArrayLayers = 1;
    depth_texture_descriptor.mipLevelCount = 1;
    depth_texture_descriptor.sampleCount = 1;
    depth_texture_descriptor.dimension = WGPUTextureDimension_2D;
    depth_texture_descriptor.format = WGPUTextureFormat_Depth24Plus;
    depth_texture_descriptor.usage = WGPUTextureUsage_RenderAttachment;

    depth = wgpuDeviceCreateTexture(device, &depth_texture_descriptor);
    depth_view = wgpuTextureCreateView(depth, nullptr);


    // Internal Config.
    shader_manager.SetDevice(device);
    mesh_manager.SetDevice(device);

    // Load Game
    Load();

    // Done
    std::cout << "Start done." << std::endl;
}

void Karia::Load()
{
    // Get Content From File

    // Loading Meshes
    mesh_manager.LoadMeshFromFile("cube", "data/models/cube.obj");
    mesh_manager.LoadMeshFromFile("bunny", "data/models/bunny.obj");
    mesh_manager.LoadMeshFromFile("circle", "data/models/player.obj");
    mesh_manager.LoadMeshFromFile("plane", "data/models/plane.obj");

    // Loading Shaders
    shader_manager.CreateShaderFromFile("basic", "data/shaders/basic.wgsl", format);
    shader_manager.CreateShaderFromFile("basic_2", "data/shaders/basic.wgsl", format);
    shader_manager.CreateShaderFromFile("basic_3", "data/shaders/basic.wgsl", format);
    shader_manager.CreateShaderFromFile("error", "data/shaders/error.wgsl", format);

    // Player Entity
    Entity player;
    player.add_component<Transform>(0.0f, 0.0f, 0.0f);
    player.add_component<Keyboard>();
    player.add_component<Shader>("basic");
    player.add_component<Mesh>("circle");

    // Enemy Entity
    Entity enemy;
    enemy.add_component<Transform>(-3.0f, -3.0f, 0.0f);
    enemy.add_component<Shader>("basic_2");
    enemy.add_component<Mesh>("cube");

    Entity plane;
    plane.add_component<Transform>(0.0f, 0.0f, 0.0f);
    plane.add_component<Shader>("basic_3");
    plane.add_component<Mesh>("plane");


    Entity cam;
    cam.add_component<Transform>(0.0f, 0.0f, 0.0f);
    cam.add_component<Camera>(player);

    entity_manager.add_entitity(enemy);
    entity_manager.add_entitity(player);
    entity_manager.add_entitity(cam);
    entity_manager.add_entitity(plane);

    auto movement_system = std::make_shared<MovementSystem>(&entity_manager);
    system_manager.RegisterSystem(movement_system);
}

void Karia::Update()
{
    while (SDL_PollEvent(&event))
    {
        if (event.type == SDL_EVENT_QUIT)
            is_running = false;

        if (event.type == SDL_EVENT_KEY_DOWN)
        {
            if (event.key.key ==  SDLK_ESCAPE)
                is_running = false;
        }
    }

    // Update Systems
    system_manager.update(0);

}

void Karia::Draw()
{
    WGPUTextureView target_view = GetNextTextureView();
    if (!target_view)
        return ;

    WGPUCommandEncoderDescriptor encoder_descriptor = {};
    encoder_descriptor.nextInChain = nullptr;

    WGPUCommandEncoder command_encoder = wgpuDeviceCreateCommandEncoder(device, &encoder_descriptor);

    WGPURenderPassDescriptor renderpass_descriptor = {};
    renderpass_descriptor.nextInChain = nullptr;

    // Color Attachment
    WGPURenderPassColorAttachment renderpass_color_attachment = {};
    renderpass_color_attachment.view = target_view;
    renderpass_color_attachment.resolveTarget = nullptr;
    renderpass_color_attachment.loadOp = WGPULoadOp_Clear;
    renderpass_color_attachment.storeOp = WGPUStoreOp_Store;
    renderpass_color_attachment.clearValue = WGPUColor{1.0, 0.0, 1.0, 1.0};

    renderpass_descriptor.colorAttachmentCount = 1;
    renderpass_descriptor.colorAttachments = &renderpass_color_attachment;
    renderpass_descriptor.timestampWrites = nullptr;

    //renderpass_descriptor.depthStencilAttachment = nullptr;
    renderpass_color_attachment.depthSlice = WGPU_DEPTH_SLICE_UNDEFINED;

    // Depth Attachment
    WGPURenderPassDepthStencilAttachment renderpass_depth_stencil_attachment = {};
    renderpass_depth_stencil_attachment.view = depth_view;
    renderpass_depth_stencil_attachment.depthLoadOp = WGPULoadOp_Clear;
    renderpass_depth_stencil_attachment.depthStoreOp = WGPUStoreOp_Store;
    renderpass_depth_stencil_attachment.depthClearValue = 1.0f;

    renderpass_descriptor.depthStencilAttachment = &renderpass_depth_stencil_attachment;


    WGPURenderPassEncoder renderpass_enconder = {};
    renderpass_enconder = wgpuCommandEncoderBeginRenderPass(command_encoder, &renderpass_descriptor);

    // Camera

    Camera *cam = nullptr;

    for (auto &e: entity_manager.get_entities())
    {
        if (e.has_component<Camera>())
        {
            cam = &e.get_component<Camera>();
            break ;
        }
    }

    Transform target_transform = cam->target.get_component<Transform>();

    glm::vec3 eye = target_transform.position + glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)) * 10.0f;

    glm::mat4 view = glm::lookAt(
        eye,
        target_transform.position,
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    float aspect = 800.0f / 600.0f;
    float zoom = 5.0f;

    glm::mat4 projection = glm::ortho(
        -zoom * aspect,
            zoom * aspect,
            -zoom,
            zoom,
            -100.0f,
            100.0f
    );




    // Render System
    for (auto &e : entity_manager.get_entities())
    {
        if (e.has_component<Shader>() && e.has_component<Transform>() && e.has_component<Mesh>())
        {
            // Resources
            std::string shader_name = e.get_component<Shader>().shader_name;
            std::string mesh_name = e.get_component<Mesh>().mesh_name;

            auto render_pipeline = shader_manager.GetShader(shader_name);
            auto uniform_buffer = shader_manager.GetUniformBuffer(shader_name);
            auto bind_group = shader_manager.GetBindGroup(shader_name);
            auto mesh_data = mesh_manager.GetMesh(mesh_name);

            // Components
            Transform transform = e.get_component<Transform>();

            // Transformations
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);
            model = glm::rotate(model, 3.14f, glm::vec3(0.0f, 1.0f, 0.0f));


            Uniforms uniforms;
            uniforms.mvp = projection * view * model;

            wgpuQueueWriteBuffer(queue, uniform_buffer, 0, &uniforms, sizeof(Uniforms));


            wgpuRenderPassEncoderSetPipeline(renderpass_enconder, render_pipeline);
            wgpuRenderPassEncoderSetBindGroup(renderpass_enconder, 0, bind_group, 0, nullptr);

            wgpuRenderPassEncoderSetVertexBuffer(renderpass_enconder, 0, mesh_data->buffer, 0, WGPU_WHOLE_SIZE);
            wgpuRenderPassEncoderDraw(renderpass_enconder, mesh_data->vertex_count, 1, 0, 0);
        }
    }

    wgpuRenderPassEncoderEnd(renderpass_enconder);
    wgpuRenderPassEncoderRelease(renderpass_enconder);

    // Command Buffer
    WGPUCommandBufferDescriptor command_buffer_descriptor = {};
    command_buffer_descriptor.nextInChain = nullptr;

    WGPUCommandBuffer command_buffer;
    command_buffer = wgpuCommandEncoderFinish(command_encoder, &command_buffer_descriptor);
    wgpuCommandEncoderRelease(command_encoder);

    wgpuQueueSubmit(queue, 1, &command_buffer);
    wgpuCommandBufferRelease(command_buffer);

    wgpuTextureViewRelease(target_view);
    wgpuSurfacePresent(surface);
}

void Karia::End()
{

}

WGPUTextureView Karia::GetNextTextureView()
{
    WGPUTextureView texture_view = nullptr;

    WGPUSurfaceTexture surface_texture;
    wgpuSurfaceGetCurrentTexture(surface, &surface_texture);

    // Wait to complete current texture
    if (surface_texture.status != WGPUSurfaceGetCurrentTextureStatus_SuccessOptimal)
        return nullptr;

    WGPUTextureViewDescriptor texture_view_descriptor = {};
    texture_view_descriptor.nextInChain = nullptr;
    texture_view_descriptor.format = wgpuTextureGetFormat(surface_texture.texture);
    texture_view_descriptor.dimension = WGPUTextureViewDimension_2D;
    texture_view_descriptor.baseMipLevel = 0;
    texture_view_descriptor.mipLevelCount = 1;
    texture_view_descriptor.baseArrayLayer = 0;
    texture_view_descriptor.arrayLayerCount = 1;
    texture_view_descriptor.aspect = WGPUTextureAspect_All;

    texture_view = wgpuTextureCreateView(surface_texture.texture, &texture_view_descriptor);

    if (!texture_view)
        return (nullptr);

    return texture_view;
}

void Karia::RequestAdapter(
    WGPURequestAdapterStatus status,
    WGPUAdapter adapter,
    WGPUStringView message,
    void *userdata1,
    void *userdata2
)
{
    if (status == WGPURequestAdapterStatus_Success)
        *static_cast<WGPUAdapter *>(userdata1) = adapter;
}

void Karia::RequestDevice(
    WGPURequestDeviceStatus status,
    WGPUDevice device,
    WGPUStringView message,
    void *userdata1,
    void *userdata2
)
{
    if (status == WGPURequestDeviceStatus_Success)
        *static_cast<WGPUDevice *>(userdata1) = device;
}
