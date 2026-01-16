#include "SDL3/SDL_scancode.h"
#include "kwgpu/components.h"
#include "kwgpu/sprite_manager.h"
#include "webgpu/webgpu.h"
#include <kwgpu/karia.h>

#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_wgpu.h>

#include <sdl3webgpu.h>
#include <iostream>

#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/ext.hpp>
#include <glm/glm.hpp>
//#include

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

    window = SDL_CreateWindow("Hello Karia", 1920, 1080, 0);
    if (!window)
    {
        std::cerr << "Error: Unable to create window." << std::endl;
        exit(EXIT_FAILURE);
    }

    //SDL_SetWindowFullscreen(window, true);
    //SDL_SetWindowFullscreen

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
    sprite_manager.SetDevice(device);

    // ImGui
    // Setup Dear ImGui context

    float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup scaling
    ImGuiStyle& style = ImGui::GetStyle();
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (using io.ConfigDpiScaleFonts=true makes this unnecessary. We leave both here for documentation purpose)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForOther(window);

    ImGui_ImplWGPU_InitInfo init_info;
    init_info.Device = device;
    init_info.NumFramesInFlight = 3;
    init_info.RenderTargetFormat = surface_configuration.format;
    init_info.DepthStencilFormat = WGPUTextureFormat_Depth24Plus;
    ImGui_ImplWGPU_Init(&init_info);


    // Load Game
    Load();

    // Done
    std::cout << "Start done." << std::endl;
}

void Karia::Update()
{
    while (SDL_PollEvent(&event))
    {
        ImGui_ImplSDL3_ProcessEvent(&event);
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

    float aspect = 1920.0f / 1080.0f;
    static float zoom = 5.0f;

    const bool *key_states = SDL_GetKeyboardState(nullptr);
    if (key_states[SDL_SCANCODE_PAGEUP])
        zoom += 0.2f;
    if (key_states[SDL_SCANCODE_PAGEDOWN])
        zoom -= 0.2f;


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
        if (e.has_component<Shader>() && e.has_component<Transform>() && e.has_component<Mesh>() && e.has_component<Sprite>())
        {
            // Resources
            std::string shader_name = e.get_component<Shader>().shader_name;
            std::string mesh_name = e.get_component<Mesh>().mesh_name;
            std::string sprite_name = e.get_component<Sprite>().sprite_name;

            auto shader_data = shader_manager.GetShader(shader_name);
            //auto uniform_buffer = shader_manager.GetUniformBuffer(shader_name);
            auto bind_group = shader_manager.GetBindGroup(shader_name);
            auto mesh_data = mesh_manager.GetMesh(mesh_name);
            auto sprite_data = sprite_manager.GetSprite(sprite_name);

            // Components
            Transform transform = e.get_component<Transform>();

            // Transformations
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, transform.position);
            model = glm::scale(model, transform.scale);

            model = glm::rotate(model, transform.rotation.x, glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, transform.rotation.y, glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, transform.rotation.z, glm::vec3(0.0f, 0.0f, 1.0f));

            //model = glm::rotate(model, 3.14f, glm::vec3(1.0f, 0.0f, 0.0f));

            wgpuRenderPassEncoderSetPipeline(renderpass_enconder, shader_data.pipeline);

            Uniforms uniforms;
            uniforms.mvp = projection * view * model;



            // Create a sampler
            WGPUSamplerDescriptor sampler_desc =
            {
                .addressModeU = WGPUAddressMode_ClampToEdge,
                .addressModeV = WGPUAddressMode_ClampToEdge,
                .addressModeW = WGPUAddressMode_ClampToEdge,
                .magFilter = WGPUFilterMode_Nearest,
                .minFilter = WGPUFilterMode_Nearest,
                .mipmapFilter = WGPUMipmapFilterMode_Nearest,
                .lodMinClamp = 0.0f,
                .lodMaxClamp = 1.0f,
                .compare = WGPUCompareFunction_Undefined,
                .maxAnisotropy = 1,
            };
            WGPUSampler sampler = wgpuDeviceCreateSampler(device, &sampler_desc);

            WGPUBufferDescriptor buffer_descriptor = {};
            buffer_descriptor.size = sizeof(Uniforms);
            buffer_descriptor.usage = WGPUBufferUsage_Uniform | WGPUBufferUsage_CopyDst;
            auto uniform_buffer = wgpuDeviceCreateBuffer(device, &buffer_descriptor);

            std::vector<WGPUBindGroupEntry> bindings(3);

            bindings[0].binding = 0;
            bindings[0].buffer = uniform_buffer;
            bindings[0].offset = 0;
            bindings[0].size = sizeof(Uniforms);

            // uv-texture is default
            bindings[1].binding = 1;
            bindings[1].textureView = sprite_data->texture_view;
            //bindings[1].textureView = nullptr;

            bindings[2].binding = 2;
            bindings[2].sampler = sampler;

            wgpuQueueWriteBuffer(queue, uniform_buffer, 0, &uniforms, sizeof(Uniforms));

            //std::cout << sprite_manager->GetSprite("uv-texture")->texture_view << std::endl;
            //bindings[1].textureView = nullptr;


            WGPUBindGroupDescriptor bind_group_descriptor = {};
            bind_group_descriptor.layout = shader_data.bind_group_layout;
            bind_group_descriptor.entryCount = (uint32_t)bindings.size();
            bind_group_descriptor.entries = bindings.data();
            shader_data.bind_group = wgpuDeviceCreateBindGroup(device, &bind_group_descriptor);

            wgpuRenderPassEncoderSetBindGroup(renderpass_enconder, 0, shader_data.bind_group, 0, nullptr);

            wgpuRenderPassEncoderSetVertexBuffer(renderpass_enconder, 0, mesh_data->buffer, 0, WGPU_WHOLE_SIZE);
            wgpuRenderPassEncoderDraw(renderpass_enconder, mesh_data->vertex_count, 1, 0, 0);


        }
    }

    ImGui_ImplWGPU_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();

    // Build our UI
    {
        static float f = 0.0f;
        static int counter = 0;
        static bool show_demo_window = true;
        static bool show_another_window = false;

        ImGui::Begin("Manager");

        // Create a window called "Hello, world!" and append into it.
        //const char* items[] = { "Apple", "Banana", "Cherry", "Kiwi", "Mango", "Orange", "Pineapple", "Strawberry", "Watermelon" };

        std::vector<std::string> names = entity_manager.GetActiveEntitiesNames();
        std::vector<const char *> items;
        for (auto& n: names)
            items.push_back(n.c_str());

        static int selected = -1;

        ImGui::Text("Entities");
        ImGui::Separator();
        ImGui::ListBox("##Entities", &selected, items.data(), items.size(), 5);

        if (selected != -1)
        {
            static bool window = true;
            ImGui::Begin("Properties", &window);
            ImGui::Text("Entity %s", items[selected]);
            ImGui::Separator();


            Entity *e = entity_manager.GetEntityByName(items[selected]);

            // Keyboard
            if (e->has_component<Keyboard>())
            {
                ImGui::SeparatorText("Keyboard");
            }

            // Transform
            if (e->has_component<Transform>())
            {

                {
                Transform& transform = e->get_component<Transform>();

                ImGui::SeparatorText("Transform");

                //ImGui::Text("Position");
                ImGui::SliderFloat3("Position", glm::value_ptr(transform.position), -100.0f, 100.0f);
                ImGui::SliderFloat3("Scale", glm::value_ptr(transform.scale), 0.0f, 5.0f);
                ImGui::SliderFloat3("Rotation", glm::value_ptr(transform.rotation), 0.0f, 360.0f);

                //ImGui::InputFloat3("##PositionInput", glm::value_ptr(transform.position));
                }

            }


            // Mesh
            if (e->has_component<Mesh>())
            {
                ImGui::SeparatorText("Mesh");
            }

            // Shader
            if (e->has_component<Shader>())
            {
                ImGui::SeparatorText("Shader");
            }

            ImGui::End();
        }



        ImGuiIO& io = ImGui::GetIO();
        ImGui::Text("FPS: %.1f", io.Framerate);
        ImGui::End();
    }

    // Draw the UI
    ImGui::EndFrame();
    // Convert the UI defined above into low-level drawing commands
    ImGui::Render();
    // Execute the low-level drawing commands on the WebGPU backend
    ImGui_ImplWGPU_RenderDrawData(ImGui::GetDrawData(), renderpass_enconder);


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

    if (ImGui_ImplWGPU_IsSurfaceStatusError(surface_texture.status))
    {
        fprintf(stderr, "Unrecoverable Surface Texture status=%#.8x\n", surface_texture.status);
        abort();
    }

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
