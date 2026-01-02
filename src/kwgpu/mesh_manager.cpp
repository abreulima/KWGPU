#include "glm/ext/vector_float2.hpp"
#include <cstdint>
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <vector>
#include <memory>

#include <kwgpu/mesh_manager.h>

std::shared_ptr<MeshData> MeshManager::LoadMeshFromFile(std::string name, const char *file)
{

    std::shared_ptr<MeshData> mesh = std::make_shared<MeshData>();


    tinyobj::attrib_t attributes;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning;
	std::string error;

    std::vector<Vertex> vertices;

    /*
	Note: old way
    std::vector<glm::vec3> positions;
	std::vector<glm::vec2> text_coords;
    std::vector<glm::vec3> normals;
    */

	if (!tinyobj::LoadObj(
		&attributes,
		&shapes,
		&materials,
		&warning,
		&error,
		file)
	)
	{
		throw std::runtime_error("ast::assets::loadOBJFile: Error: " + warning + error);
	}

	for (const auto &shape: shapes)
	{
		for (const auto &index: shape.mesh.indices)
		{

		    Vertex v = {};

			v.position = {
				attributes.vertices[3 * index.vertex_index + 0],
				attributes.vertices[3 * index.vertex_index + 1],
				attributes.vertices[3 * index.vertex_index + 2]
			};

			if (index.texcoord_index >= 0)
			{
			    v.text_coord = {
					attributes.texcoords[2 * index.texcoord_index + 0],
					attributes.texcoords[2 * index.texcoord_index + 1]
				};
			}
			else
			{
			   v.text_coord = glm::vec2(1.0f); // fallback
			}

			if (index.normal_index >= 0)
		    {
				v.normal = {
					attributes.normals[3 * index.normal_index + 0],
					attributes.normals[3 * index.normal_index + 1],
					attributes.normals[3 * index.normal_index + 2]
				};
			}
			else
			{
			    v.normal = glm::vec3(0.0f, 1.0f, 0.0f); // fallback
			}

			vertices.push_back(v);
		}
	}

	WGPUBufferDescriptor buffer_descriptor = {};
	buffer_descriptor.size = vertices.size() * sizeof(Vertex);
	buffer_descriptor.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;

	WGPUBuffer buffer = wgpuDeviceCreateBuffer(device, &buffer_descriptor);
	wgpuQueueWriteBuffer(wgpuDeviceGetQueue(device), buffer, 0, vertices.data(), buffer_descriptor.size);

	mesh->buffer = buffer;
	mesh->vertex_count = static_cast<uint32_t>(vertices.size());

	meshes[name] = std::move(mesh);
	return (mesh);
}

std::shared_ptr<MeshData> MeshManager::GetMesh(std::string name)
{
    return meshes[name];
}

void MeshManager::SetDevice(WGPUDevice device)
{
    this->device = device;
}
