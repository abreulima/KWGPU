#include "webgpu/webgpu.h"
#include <cstdlib>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <memory>
#include <iostream>

#include <kwgpu/sprite_manager.h>


void SpriteManager::writeMipMaps(WGPUDevice device, WGPUTexture texture, WGPUExtent3D textureSize, uint32_t mipLevelCount, const unsigned char* pixel_data)
{

    WGPUTexelCopyTextureInfo destination = {};
    destination.texture = texture;
    destination.mipLevel = 0;
    destination.origin = { 0, 0, 0};
    destination.aspect = WGPUTextureAspect_All;

    WGPUTexelCopyBufferLayout data_layout = {};
    data_layout.offset = 0;
    data_layout.bytesPerRow = 4 * textureSize.width;
    data_layout.rowsPerImage = textureSize.height;

    WGPUQueue queue = wgpuDeviceGetQueue(device);

    wgpuQueueWriteTexture(queue, &destination, pixel_data, 4 * textureSize.width * textureSize.height, &data_layout, &textureSize);
    wgpuQueueRelease(queue);
}

std::shared_ptr<SpriteData> SpriteManager::LoadSpriteFromFile(std::string name, const char *file)
{
    int width, height, channels;

    std::shared_ptr<SpriteData> sprite = std::make_shared<SpriteData>();

    stbi_set_flip_vertically_on_load(true);
    unsigned char *pixel_data = stbi_load(file, &width, &height, &channels, 4);
    if (nullptr == pixel_data)
    {
        std::cerr << "Failed to load image " << file << std::endl;
        return (nullptr);
    }

    WGPUTextureDescriptor texture_descriptor = {};
    texture_descriptor.nextInChain = nullptr;
    texture_descriptor.dimension = WGPUTextureDimension_2D;
    texture_descriptor.format = WGPUTextureFormat_RGBA8Unorm; // by convention for bmp, png and jpg file. Be careful with other formats.
    texture_descriptor.mipLevelCount = 1;
    texture_descriptor.sampleCount = 1;
    texture_descriptor.size = { (unsigned int)width, (unsigned int)height, 1 };
    texture_descriptor.usage = WGPUTextureUsage_TextureBinding | WGPUTextureUsage_CopyDst;
    texture_descriptor.viewFormatCount = 0;
    texture_descriptor.viewFormats = nullptr;
    sprite->texture = wgpuDeviceCreateTexture(device, &texture_descriptor);

    //writeMipMaps(device, sprite->texture, texture_descriptor.size, texture_descriptor.mipLevelCount, pixel_data);

    WGPUTexelCopyTextureInfo dest = {
        .texture = sprite->texture,
        .mipLevel = 0,
        .origin = {0, 0, 0},
        .aspect = WGPUTextureAspect_All,
    };

       WGPUTexelCopyBufferLayout data_layout = {
        .offset = 0,
        .bytesPerRow = (uint32_t)(width * 4),
        .rowsPerImage = (uint32_t)height,
    };

    WGPUQueue queue = wgpuDeviceGetQueue(device);
    wgpuQueueWriteTexture(queue, &dest, pixel_data, width * height * 4, &data_layout, &texture_descriptor.size);
    //exit(1);

    stbi_image_free(pixel_data);

    WGPUTextureViewDescriptor texture_view_descriptor = {};
    texture_view_descriptor.format = texture_descriptor.format;
    texture_view_descriptor.dimension = WGPUTextureViewDimension_2D;
    texture_view_descriptor.baseMipLevel = 0;
    texture_view_descriptor.mipLevelCount = texture_descriptor.mipLevelCount;
    texture_view_descriptor.baseArrayLayer = 0;
    texture_view_descriptor.arrayLayerCount = 1;
    texture_view_descriptor.aspect = WGPUTextureAspect_All;
    sprite->texture_view = wgpuTextureCreateView(sprite->texture, &texture_view_descriptor);

    if (!sprite->texture_view)
    {
        std::cerr << "Failed to create texture view." << std::endl;
        exit(EXIT_FAILURE);
    }

	sprites[name] = std::move(sprite);
	return (sprite);
}

std::shared_ptr<SpriteData> SpriteManager::GetSprite(std::string name)
{
    return sprites[name];
}

void SpriteManager::SetDevice(WGPUDevice device)
{
    this->device = device;
}
