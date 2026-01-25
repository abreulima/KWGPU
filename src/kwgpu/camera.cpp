#include "glm/ext/matrix_projection.hpp"
#include "glm/ext/vector_float4.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
#include <kwgpu/camera.h>

#include <iostream>

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


/* Generate a ray from mouse position
 * and convert 2D screen to 3D World
 */

 /*
  * // Source - https://stackoverflow.com/a
  // Posted by AudioGuy, modified by community. See post 'Timeline' for change history
  // Retrieved 2026-01-21, License - CC BY-SA 4.0

  Vector3 worldUpDirection = new Vector3(0, 1, 0); // if your world is y-up

  // Get mouse coordinates (2d) relative to window position:
  Vector2 mousePosRelativeToWindow = GetMouseCoordsRelativeToWindow(); // (0,0) would be top left window corner

  // get camera direction vector:
  Vector3 camDirection = Vector3.Normalize(cameraTarget - cameraPosition);

  // get x and y coordinates relative to frustum width and height.
  // glOrthoWidth and glOrthoHeight are the sizeX and sizeY values
  // you created your projection matrix with. If your frustum has a width of 100,
  // x would become -50 when the mouse is left and +50 when the mouse is right.
  float x = +(2.0f * mousePosRelativeToWindow .X / viewportWidth  - 1) * (glOrthoWidth  / 2);
  float y = -(2.0f * mousePosRelativeToWindow .Y / viewPortHeight - 1) * (glOrthoHeight / 2);

  // Now, you want to calculate the camera's local right and up vectors
  // (depending on the camera's current view direction):
  Vector3 cameraRight = Vector3.Normalize(Vector3.Cross(camDirection, worldUpDirection));
  Vector3 cameraUp = Vector3.Normalize(Vector3.Cross(cameraRight, camDirection));

  // Finally, calculate the ray origin:
  Vector3 rayOrigin = cameraPosition + cameraRight * x + cameraUp * y;
  Vector3 rayDirection = camDirection;

  */

/*
Ray CameraManager::ray(glm::vec2 mouse, glm::vec2 screenSize)
{
     // 3D Nomalised Device Coordinates
     float ndc_x = (2.0f * (mouse.x ) / (double)(screenSize.x)) - 1.0f;
     float ndc_y = (2.0f * (mouse.y ) / (double)(screenSize.y)) - 1.0f;
     float ndc_z = 1.0f;
     glm::vec3 ndc = glm::vec3(ndc_x, ndc_y, ndc_z);

     glm::vec4 ray_clip = glm::vec4(ndc.x, ndc.y, 1.0, 1.0);

     // To Eyes
     glm::vec4 ray_eye = glm::inverse(cam_data->ProjectionMatrix) * ray_clip;
     ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0, 0.0);

     // To World Coords
     glm::vec3 ray_world = glm::inverse(cam_data->ViewMatrix) * ray_eye;
     ray_world = glm::normalize(ray_world);

     Ray r;
     r.position = ray_eye;
     r.position = ray_world;

     return r;
}
*/

// Source - https://stackoverflow.com/a
// Posted by Zübeyir Bodur, modified by community. See post 'Timeline' for change history
// Retrieved 2026-01-25, License - CC BY-SA 4.0

/*
vec3 rayCast(double xpos, double ypos, mat4 view, mat4 projection, unsigned SCR_WIDTH, unsigned SCR_HEIGHT)
{
    float x = (2.0f * xpos) / SCR_WIDTH - 1.0f;
    float y = 1.0f - (2.0f * ypos) / SCR_SHEIGHT;
    float z = 1.0f;
    vec3 ray_nds = vec3(x, y, z);
    // Change this part
    vec4 ray_clip = vec4(ray_nds.x, ray_nds.y, ray_nds.z, 1.0f);
    vec4 ray_eye = inverse(projection) * ray_clip;
    // And this part
    ray_eye = vec4(ray_eye.x, ray_eye.y, ray_eye.z, 0.0f);
    vec4 inv_ray_wor = (inverse(view) * ray_eye);
    vec3 ray_wor = vec3(inv_ray_wor.x, inv_ray_wor.y, inv_ray_wor.z);
    ray_wor = normalize(ray_wor);
    return ray_wor;
}
*/

Ray CameraManager::ray(glm::vec2 mouse, glm::vec2 screenSize)
{

    //float x = (2.0f * (mouse.x ) / (double)(screenSize.x)) - 1.0f;
    //float y = 1.0f - (2.0f * mouse.y ) / (double)(screenSize.y);
    // Source - https://stackoverflow.com/a
    // Posted by jackw11111, modified by community. See post 'Timeline' for change history
    // Retrieved 2026-01-25, License - CC BY-SA 4.0

    float mx = mouse.x;
    float my = screenSize.y - mouse.y;

    glm::vec3 rayOrigin = glm::unProject(
                            glm::vec3(mx, my, 0.0f),
                            cam_data->ViewMatrix,
                            cam_data->ProjectionMatrix,
                            glm::vec4(0, 0, screenSize.x, screenSize.y)
    );

    glm::vec3 rayDirection = -glm::vec3(cam_data->ViewMatrix[0][2],
                                            cam_data->ViewMatrix[1][2],
                                            cam_data->ViewMatrix[2][2]);



    Ray r;
    r.position = rayOrigin;
    r.direction = glm::normalize(rayDirection);

    return r;

    /*
    glm::vec3 ray_nds = glm::vec3(x, y, 1.0f);

    glm::vec4 ray_clip = glm::vec4(ray_nds.x, ray_nds.y, ray_nds.z, 1.0f);
    glm::vec4 ray_eye = glm::inverse(cam_data->ProjectionMatrix) * ray_clip;

    ray_eye = glm::vec4(ray_eye.x , ray_eye.y, ray_eye.z, 0.0f);
    glm::vec4 inv_ray_wr = glm::inverse(cam_data->ViewMatrix) * ray_eye;
    glm::vec3 ray_wor = glm::vec3(inv_ray_wr.x, inv_ray_wr.y, inv_ray_wr.z);
    ray_wor = glm::normalize(ray_wor);

    Ray r;
    float near = -100.0f;
    float far = 100.0f;

    r.position = ray_eye;
    r.direction = ray_wor;

    return r;

    glm::vec3 worldUp = glm::vec3(0, 1, 0);

    glm::vec2 mousPosRelativeToWindow = mouse;

    glm::vec3 cameraTarget = glm::vec3(0, 0, 0);
    glm::vec3 cameraPosition =  glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)) * 10.0f;

    glm::vec3 camDirection = glm::normalize(glm::vec3(cameraTarget - cameraPosition));

    */

    //std::cout << "Mouser" << x << " " << y << std::endl;

    //float x = +(2.0f * mousPosRelativeToWindow.x / screenSize.x  - 1) * (screenSize.x  / 2);
    //float y = -(2.0f * mousPosRelativeToWindow.y / screenSize.y - 1) * (screenSize.y / 2);

    //this.ray.origin.set( coords.x, coords.y, ( camera.near + camera.far ) / ( camera.near - camera.far ) ).unproject( camera ); // set origin in plane of camera
	//this.ray.direction.set( 0, 0, - 1 ).transformDirection( camera.matrixWorld );
	//this.camera = camera;

    //glm::vec3 cameraRight = glm::normalize(glm::cross(camDirection, worldUp));
    //glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, camDirection));

    return r;
}


glm::vec3 CameraManager::RayIntersectPlane(glm::vec3 planePoint, glm::vec3 planeNormal, glm::vec3 rayPoint, glm::vec3 rayDirection)
{

    float denom = glm::dot(rayDirection, planeNormal);

    if (glm::abs(denom) < 1e-6f)
        return glm::vec3(0.0f);

    float t = glm::dot(planePoint - rayPoint, planeNormal) / denom;

    return rayPoint + t * rayDirection;
    //float d = glm::dot(planePoint, -planeNormal);
    //auto t = -(d + glm::dot(rayPoint, planeNormal)) / glm::dot(rayDirection, planeNormal);
    //return rayPoint + t * rayDirection;
}
