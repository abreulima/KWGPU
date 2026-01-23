#include "glm/ext/matrix_projection.hpp"
#include "glm/fwd.hpp"
#include "glm/geometric.hpp"
#include "glm/matrix.hpp"
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

Ray CameraManager::ray(glm::vec2 mouse, glm::vec2 screenSize)
{
    glm::vec3 worldUp = glm::vec3(0, 1, 0);

    glm::vec2 mousPosRelativeToWindow = mouse;

    glm::vec3 cameraTarget = glm::vec3(0, 0, 0);
    glm::vec3 cameraPosition =  glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f)) * 10.0f;

    glm::vec3 camDirection = glm::normalize(glm::vec3(cameraTarget - cameraPosition));

    float x = (2.0f * (mouse.x ) / (double)(screenSize.x)) - 1.0f;
    float y = (2.0f * (mouse.y ) / (double)(screenSize.y)) - 1.0f;
    //float x = +(2.0f * mousPosRelativeToWindow.x / screenSize.x  - 1) * (screenSize.x  / 2);
    //float y = -(2.0f * mousPosRelativeToWindow.y / screenSize.y - 1) * (screenSize.y / 2);

    //this.ray.origin.set( coords.x, coords.y, ( camera.near + camera.far ) / ( camera.near - camera.far ) ).unproject( camera ); // set origin in plane of camera
	//this.ray.direction.set( 0, 0, - 1 ).transformDirection( camera.matrixWorld );
	//this.camera = camera;

    glm::vec3 cameraRight = glm::normalize(glm::cross(camDirection, worldUp));
    glm::vec3 cameraUp = glm::normalize(glm::cross(cameraRight, camDirection));

    Ray r;
    float near = -100.0f;
    float far = 100.0f;

    r.position = glm::vec3(1, 1, 1);
    r.direction = camDirection;

    return r;
}


glm::vec3 CameraManager::RayIntersectPlane(glm::vec3 planePoint, glm::vec3 planeNormal, glm::vec3 rayPoint, glm::vec3 rayDirection)
{
    float d = glm::dot(planePoint, -planeNormal);
    auto t = -(d + glm::dot(rayPoint, planeNormal)) / glm::dot(rayDirection, planeNormal);
    return rayPoint + t * rayDirection;
}
