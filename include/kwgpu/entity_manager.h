#ifndef ENTITY_MANAGER_H
#define ENTITY_MANAGER_H

#include <kwgpu/entity.h>
#include <memory>
#include <vector>
#include <string>

class EntityManager
{
    public:
        std::vector<Entity> entities;

        void add_entitity(Entity e)
        {
            entities.emplace_back(e);
        }

        std::vector<Entity> &get_entities()
        {
            return entities;
        }

        template <typename T>
        Entity *get_entity()
        {
            for (auto &e: entities)
            {
                if (e.has_component<T>())
                    return &e;
            }
            return (nullptr);
        }

        std::vector<std::string> GetActiveEntitiesNames()
        {
            std::vector<std::string> names;

            for (auto &e: entities)
            {
                names.push_back(e.name);
            }

            return (names);
        }

        Entity* GetEntityByName(std::string name)
        {
            for(auto &e: entities)
            {
                if (name.compare(e.name) == 0)
                return (&e);
            }
            return (nullptr);
        }

};

#endif
