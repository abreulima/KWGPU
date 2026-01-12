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


    void SetScale(float s_x, float s_y, float s_z)
    {
        scale = glm::vec3(s_x, s_y, s_z);
    }


    /*
    void SetRotation(float r_x, float r_y, float r_z)
    {
        rotation = glm::vec3(s_x, s_y, s_z);
    }
    */

};

struct Keyboard
{

};

struct Sprite
{
    std::string sprite_name;
    float w;
    float h;

    Sprite(std::string name)
    : sprite_name(name) {};

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
