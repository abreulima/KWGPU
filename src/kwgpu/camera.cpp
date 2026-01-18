#include "glm/geometric.hpp"
#include <kwgpu/camera.h>

// Source - https://stackoverflow.com/a
// Posted by Bas Smit, modified by community. See post 'Timeline' for change history
// Retrieved 2026-01-18, License - CC BY-SA 4.0

/*
Vector3 Intersect(Vector3 planeP, Vector3 planeN, Vector3 rayP, Vector3 rayD)
{
    var d = Vector3.Dot(planeP, -planeN);
    var t = -(d + Vector3.Dot(rayP, planeN)) / Vector3.Dot(rayD, planeN);
    return rayP + t * rayD;
}
*/


glm::vec3 Camera::RayIntersectPlane(glm::vec3 planePoint, glm::vec3 planeNormal, glm::vec3 rayPoint, glm::vec3 rayDirection)
{
    float d = glm::dot(planePoint, -planeNormal);
    auto t = -(d + glm::dot(rayPoint, planeNormal)) / glm::dot(rayDirection, planeNormal);
    return rayPoint + t * rayDirection;
}
