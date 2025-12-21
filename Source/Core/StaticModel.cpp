#include "stdafx.h"
#include "StaticModel.h"

namespace Core::World
{
StaticModel::StaticModel()
{
	MeshData = std::make_shared<Mesh>();
	Albedo = std::make_shared<Texture>();
	Name = "Unknown_Model";
}
} // namespace Core::World
