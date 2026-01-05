#ifndef ENTITY_H
#define ENTITY_H

#include <memory>
#include <string>
#include <typeindex>
#include <unordered_map>

class Entity
{
    public:

    bool is_active;
    std::string name;
    std::unordered_map<std::type_index, std::shared_ptr<void>> components;


    Entity() {};
    Entity(std::string name) : name(name) {};

    template <typename T, typename... Args>
    T& add_component(Args&&... args)
    {
        auto component = std::make_shared<T>(std::forward<Args>(args)...);
        components[typeid(T)] = component;
        return *component;
    }

    template <typename T>
    T& get_component()
    {
        return *std::static_pointer_cast<T>(components.at(typeid(T)));
    }

    template <typename T>
    bool has_component()
    {
        return components.count(typeid(T)) > 0;
    }

};

#endif
