#ifndef SYSTEM_MANAGER_H
#define SYSTEM_MANAGER_H

#include <memory>
#include <type_traits>
#include <vector>

#include <kwgpu/entity_manager.h>

class System
{
    public:
        System (EntityManager *em) : entitiy_mananger(em) {};
        virtual void update(float dt) = 0;
        virtual void init() {}
    protected:
        EntityManager *entitiy_mananger;
};

class RenderSystem
{
    public:
        RenderSystem (EntityManager *em) : entitiy_mananger(em) {};
        virtual void render() = 0;
        virtual void init();
    protected:
        EntityManager *entitiy_mananger;
};

class SystemManager
{
    public:

        template <typename T>
        void RegisterSystem(std::shared_ptr<T> system)
        {
            static_assert(std::is_base_of_v<System, T>, "T must be derived from System class.\n");
            systems.push_back(system);
        }

        void update(float dt)
        {
            for (auto& system : systems)
            {
                system->update(dt);
            }
        }

    private:
        std::vector<std::shared_ptr<System>> systems;
        std::vector<std::shared_ptr<RenderSystem>> render_systems;

};

#endif
