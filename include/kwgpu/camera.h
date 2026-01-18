#ifndef CAMERA_H
#define CAMERA_H

#include "glm/ext/vector_float3.hpp"
#include <glm/glm.hpp>

class Camera
{
    private:


    public:
        glm::vec3 RayIntersectPlane(glm::vec3 planePoint, glm::vec3 planeNormal, glm::vec3 rayPoint, glm::vec3 rayDirection);
};


#endif
