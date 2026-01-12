#ifndef SPRITE_MANAGER_H
#define SPRITE_MANAGER_H

#include "kwgpu/components.h"
#include "webgpu/webgpu.h"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

struct SpriteData
{
    WGPUTexture texture = nullptr;
    WGPUTextureView texture_view = nullptr;
};

class SpriteManager
{
    private:
        WGPUDevice device = nullptr;
    public:
        void SetDevice(WGPUDevice device);
        std::unordered_map<std::string, std::shared_ptr<SpriteData>> sprites;
        std::shared_ptr<SpriteData> LoadSpriteFromFile(std::string name, const char *file);
        std::shared_ptr<SpriteData> GetSprite(std::string name);

        void writeMipMaps(WGPUDevice device, WGPUTexture texture, WGPUExtent3D textureSize, uint32_t mipLevelCount, const unsigned char* pixel_data);
};

#endif
