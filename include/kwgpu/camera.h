#ifndef CAMERA_H
#define CAMERA_H

#include "glm/ext/vector_float3.hpp"
#include "glm/ext/vector_float4.hpp"
#include <glm/glm.hpp>
#include <memory>

struct CameraData
{
    glm::mat4 ProjectionMatrix;
    glm::mat4 ViewMatrix;
    glm::vec3 eye;
};

struct Ray
{
    glm::vec3 position;
    glm::vec3 direction;
};

class CameraManager
{
    private:


    public:
        std::shared_ptr<CameraData> cam_data;
        glm::vec3 RayIntersectPlane(glm::vec3 planePoint, glm::vec3 planeNormal, glm::vec3 rayPoint, glm::vec3 rayDirection);
        Ray ray(glm::vec2 mouse, glm::vec2 screenSize);
};


#endif
