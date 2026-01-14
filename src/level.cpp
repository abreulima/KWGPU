#include "kwgpu/components.h"
#include "kwgpu/entity.h"
#include <kwgpu/karia.h>

#include <kwgpu/systems/movement.h>

void Karia::Load()
{
    // Get Content From File
    shader_manager.SetSpriteManager(&sprite_manager);
    sprite_manager.LoadSpriteFromFile("uv-texture", "data/sprites/textures/GRASS_1A.PNG");
    sprite_manager.LoadSpriteFromFile("grass", "data/sprites/textures/GRASS_1A.PNG");
    sprite_manager.LoadSpriteFromFile("iron_golem", "data/sprites/iron_golem.png");
    sprite_manager.LoadSpriteFromFile("dingus", "data/sprites/dingus.png");
    sprite_manager.LoadSpriteFromFile("cross", "data/sprites/grave.png");
    sprite_manager.LoadSpriteFromFile("dingus", "data/sprites/dingus.png");

    // Loading Meshes
    mesh_manager.LoadMeshFromFile("cube", "data/models/cube.obj");
    mesh_manager.LoadMeshFromFile("bunny", "data/models/bunny.obj");
    mesh_manager.LoadMeshFromFile("circle", "data/models/player.obj");
    mesh_manager.LoadMeshFromFile("grass", "data/models/grass.obj");
    mesh_manager.LoadMeshFromFile("golem", "data/models/iron_golem.obj");
    mesh_manager.LoadMeshFromFile("dingus", "data/models/dingus.obj");
    mesh_manager.LoadMeshFromFile("cross", "data/models/grave.obj");

    // Loading Sprites

    // Loading Shaders
    shader_manager.CreateShaderFromFile("basic", "data/shaders/basic.wgsl", format);
    shader_manager.CreateShaderFromFile("basic_2", "data/shaders/basic.wgsl", format);
    shader_manager.CreateShaderFromFile("basic_3", "data/shaders/basic.wgsl", format);
    shader_manager.CreateShaderFromFile("error", "data/shaders/error.wgsl", format);
    shader_manager.CreateShaderFromFile("cross", "data/shaders/basic.wgsl", format);

    // Player Entity
    Entity player = Entity("Player");
    player.add_component<Shader>("basic");
    player.add_component<Mesh>("golem");
    player.add_component<Sprite>("iron_golem");
    player.add_component<Transform>(0.0f, 0.0f, 0.0f);
    player.add_component<Keyboard>();

    // grass Entity
    Entity grass = Entity("Grass");
    grass.add_component<Transform>(-3.0f, -3.0f, 0.0f);
    grass.add_component<Sprite>("grass");
    grass.add_component<Shader>("basic_2");
    grass.add_component<Mesh>("grass");

    Entity cross = Entity("Cross");
    cross.add_component<Transform>(0.0f, 0.0f, 0.0f);
    cross.add_component<Sprite>("cross");
    cross.add_component<Mesh>("cross");
    cross.add_component<Shader>("cross");
    cross.get_component<Transform>().SetScale(0.05f, 0.05f, 0.05f);


    //Entity plane = Entity("Plane");
    //plane.add_component<Transform>(0.0f, 0.0f, 0.0f);
    //plane.add_component<Shader>("basic_3");
    //plane.add_component<Mesh>("grass");

    Entity outrobixo = Entity("Gato");
    outrobixo.add_component<Transform>(0.0f, 0.0f, 0.0f);
    outrobixo.add_component<Shader>("basic_3");
    outrobixo.add_component<Mesh>("dingus");
    outrobixo.add_component<Sprite>("dingus");
    outrobixo.get_component<Transform>().SetScale(0.25f, 0.25f, 0.25f);


    //plane.get_component<Transform>().SetRotation(0.15f, 0.15f, 0.15f);

    Entity cam = Entity("Camera");
    cam.add_component<Transform>(0.0f, 0.0f, 0.0f);
    cam.add_component<Camera>(player);

    //entity_manager.add_entitity(grass);
    entity_manager.add_entitity(player);
    entity_manager.add_entitity(cam);
    entity_manager.add_entitity(grass);
    //entity_manager.add_entitity(outrobixo);
    entity_manager.add_entitity(cross);

    auto movement_system = std::make_shared<MovementSystem>(&entity_manager);
    system_manager.RegisterSystem(movement_system);

}
