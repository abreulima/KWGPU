#ifndef MESH_MANAGER_H
#define MESH_MANAGER_H

#include "kwgpu/components.h"
#include "webgpu/webgpu.h"
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

struct MeshData
{
    WGPUBuffer buffer = nullptr;
    uint32_t vertex_count = 0;
};

// changing the order breaks it, why?
struct Vertex
{
    glm::vec3 position;
    glm::vec2 text_coord;
    glm::vec3 normal;
};

class MeshManager
{
    private:
        WGPUDevice device = nullptr;
    public:
        void SetDevice(WGPUDevice device);
        std::unordered_map<std::string, std::shared_ptr<MeshData>> meshes;
        std::shared_ptr<MeshData> LoadMeshFromFile(std::string name, const char *file);
        std::shared_ptr<MeshData> GetMesh(std::string name);
};

#endif
