#ifndef MOVEMENT_H
#define MOVEMENT_H

#include "glm/ext/vector_float3.hpp"
#include "kwgpu/components.h"
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_scancode.h>
#include <cstdint>
#include <kwgpu/karia.h>

#include <SDL3/SDL.h>

#include <kwgpu/systems/system_manager.h>

class MovementSystem : public System
{
    public:
        MovementSystem(EntityManager* em) : System(em) {}

        void update(float dt) override
        {
            for (auto &e : entitiy_mananger->get_entities())
            {
                if (e.has_component<Keyboard>() && e.has_component<Transform>())
                {
                    Transform& transform = e.get_component<Transform>();
                    const bool *key_states = SDL_GetKeyboardState(nullptr);

                    glm::vec3 direction = glm::vec3(0, 0, 0);

                    float speed = 0.10f;

                    if (key_states[SDL_SCANCODE_W])
                        transform.position.x += speed;

                    if (key_states[SDL_SCANCODE_S])
                        transform.position.x -= speed;

                    if (key_states[SDL_SCANCODE_A])
                        transform.position.z += speed;

                    if (key_states[SDL_SCANCODE_D])
                        transform.position.z -= speed;

                }
            }
        }
};

#endif
