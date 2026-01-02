#ifndef COMPONENTS_H
#define COMPONENTS_H

#include "kwgpu/entity.h"
#include <cstdint>
#include <glm/glm.hpp>

#include <string.h>
#include <memory.h>

#include <string>

struct Transform
{
    glm::vec3 position;
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);

    Transform (float p_x, float p_y, float p_z)
    : position (glm::vec3(p_x, p_y, p_z)) {}

};

struct Keyboard
{

};

struct Camera
{
    Entity target;

    Camera (Entity target)
    : target(target) {}
};

// Should it store the Renderpipeline?
struct Shader
{
    std::string shader_name;

    Shader (std::string name)
    : shader_name(name) {}
};

struct Mesh
{
    std::string mesh_name;
    uint32_t vertex_count;

    Mesh(std::string name)
    : mesh_name(name) {}
};


#endif
