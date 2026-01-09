#include "pch.h"
#include "Rendeructor.h"
#include "BackendDX11.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjLoader/TinyObjLoader.h>
#include <unordered_map>

// Хеширование для дедупликации вершин
struct VertexHash
{
	size_t operator()(const Vertex& v) const
	{
		size_t h1 = std::hash<float>{}(v.Position.x);
		size_t h2 = std::hash<float>{}(v.Position.y);
		size_t h3 = std::hash<float>{}(v.Position.z);
		size_t h4 = std::hash<float>{}(v.Normal.x);
		size_t h5 = std::hash<float>{}(v.UV.x);
		// Простой hash combine
		return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3) ^ (h5 << 4);
	}
};

struct VertexEqual
{
	bool operator()(const Vertex& a, const Vertex& b) const
	{
		const float EPSILON = 1e-5f;
		return (a.Position - b.Position).length_sq() < EPSILON && (a.Normal - b.Normal).length_sq() < EPSILON &&
			   (a.UV - b.UV).length_sq() < EPSILON;
	}
};

void Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices)
{
	if (Rendeructor::GetCurrent() && Rendeructor::GetCurrent()->GetBackendAPI())
	{
		m_vbHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateVertexBuffer(
			vertices.data(), vertices.size() * sizeof(Vertex), sizeof(Vertex));

		m_ibHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateIndexBuffer(
			indices.data(), indices.size() * sizeof(unsigned int));

		m_indexCount = (int)indices.size();
	}
}

bool Mesh::LoadFromOBJ(const std::string& filepath, const std::string& mtlBaseDir,
					   std::vector<RenderMaterial>& outMaterials)
{
	tinyobj::ObjReaderConfig reader_config;
	reader_config.mtl_search_path = mtlBaseDir;
	reader_config.triangulate = true;

	tinyobj::ObjReader reader;

	if (!reader.ParseFromFile(filepath, reader_config))
	{
		if (!reader.Error().empty())
		{
			// Log Error here
		}
		return false;
	}

	const auto& attrib = reader.GetAttrib();
	const auto& shapes = reader.GetShapes();
	const auto& materials = reader.GetMaterials();

	// ---------------------------------------------------------
	// 1. КОНВЕРТАЦИЯ МАТЕРИАЛОВ (PBR)
	// ---------------------------------------------------------
	outMaterials.clear();

	if (materials.empty())
	{
		RenderMaterial defaultMat;
		defaultMat.Name = "Default_PBR";
		defaultMat.BaseColor = Math::float4(0.8f, 0.8f, 0.8f, 1.0f);
		defaultMat.Roughness = 0.5f;
		defaultMat.Metallic = 0.0f;
		outMaterials.push_back(defaultMat);
	}
	else
	{
		for (const auto& tMat : materials)
		{
			RenderMaterial pbrMat;
			pbrMat.Name = tMat.name;

			// --- Base Color / Albedo ---
			// map_Kd -> BaseColor
			pbrMat.BaseColor = Math::float4(tMat.diffuse[0], tMat.diffuse[1], tMat.diffuse[2], 1.0f);
			if (!tMat.diffuse_texname.empty())
			{
				pbrMat.AlbedoMap = tMat.diffuse_texname;
			}

			// --- Alpha / Transparency ---
			// 'd' or 'dissolve' in MTL
			pbrMat.BaseColor.w = tMat.dissolve;
			if (!tMat.alpha_texname.empty())
			{
				pbrMat.AlphaMap = tMat.alpha_texname;
			}

			// --- Metallic ---
			// Blender PBR: 'Pm' param or 'map_Pm'
			// TinyObjLoader считывает PBR расширения
			if (tMat.metallic > 0.0f)
			{
				pbrMat.Metallic = tMat.metallic;
			}
			else
			{
				// Fallback: Если 'Ks' (Specular) серый и яркий, возможно это металл (старый workflow)
				// Но лучше оставить 0 по умолчанию для безопасности.
				pbrMat.Metallic = 0.0f;
			}

			if (!tMat.metallic_texname.empty())
			{
				pbrMat.MetallicMap = tMat.metallic_texname;
			}
			else if (!tMat.specular_texname.empty())
			{
				// Fallback: иногда map_Ks используется как металл-карта в конвертерах
				// Но это опасно, так как может быть specular intensity. Оставим пустым или добавим логику.
				// pbrMat.MetallicMap = tMat.specular_texname;
			}

			// --- Roughness ---
			// Blender PBR: 'Pr' param or 'map_Pr'
			if (tMat.roughness > 0.0f)
			{
				pbrMat.Roughness = tMat.roughness;
			}
			else
			{
				// Fallback: Конвертация из Shininess (Ns). Ns обычно 0..1000
				// Roughness = sqrt(2 / (Ns + 2)) - аппроксимация Blinn-Phong -> GGX
				// Или просто инверсия для грубой оценки.
				if (tMat.shininess > 0.0f)
				{
					pbrMat.Roughness = 1.0f - std::clamp(std::sqrt(tMat.shininess) / 10.0f, 0.0f, 1.0f);
				}
				else
				{
					pbrMat.Roughness = 0.5f; // Дефолт
				}
			}

			if (!tMat.roughness_texname.empty())
			{
				pbrMat.RoughnessMap = tMat.roughness_texname;
			}
			else if (!tMat.specular_highlight_texname.empty())
			{
				// map_Ns иногда используют
				pbrMat.RoughnessMap = tMat.specular_highlight_texname;
			}

			// --- Normal Map ---
			// map_Bump or bump or norm
			if (!tMat.bump_texname.empty())
				pbrMat.NormalMap = tMat.bump_texname;
			else if (!tMat.normal_texname.empty())
				pbrMat.NormalMap = tMat.normal_texname;

			// --- Emissive ---
			// Ke
			pbrMat.Emissive = Math::float3(tMat.emission[0], tMat.emission[1], tMat.emission[2]);
			if (!tMat.emissive_texname.empty())
			{
				pbrMat.EmissiveMap = tMat.emissive_texname;
			}

			// --- Ambient Occlusion ---
			// map_Ka or map_Occlusion (нестандартное)
			if (!tMat.ambient_texname.empty())
			{
				pbrMat.OcclusionMap = tMat.ambient_texname;
			}

			outMaterials.push_back(pbrMat);
		}
	}

	// ---------------------------------------------------------
	// 2. ГЕОМЕТРИЯ (Сортировка по материалам + Дедупликация)
	// ---------------------------------------------------------

	// Сбрасываем глобальные границы
	m_MinBound = Math::float3(FLT_MAX);
	m_MaxBound = Math::float3(-FLT_MAX);
	m_SubMeshes.clear();

	// Временные структуры
	std::vector<Vertex> finalVertices;
	std::vector<unsigned int> finalIndices;

	// Map для дедупликации вершин (Vertex -> Index)
	std::unordered_map<Vertex, unsigned int, VertexHash, VertexEqual> uniqueVertices;

	// Группировка: MaterialID -> Список полигонов (индексов tinyobj)
	// Используем map, чтобы материалы шли по порядку индексов
	std::map<int, std::vector<tinyobj::index_t>> materialGroups;

	// Временные границы для каждого материала
	std::map<int, std::pair<Math::float3, Math::float3>> materialBounds;

	// Проходим по всем формам
	for (const auto& shape : shapes)
	{
		size_t index_offset = 0;
		for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); f++)
		{
			int fv = shape.mesh.num_face_vertices[f]; // Всегда 3 (triangulate=true)

			// Получаем ID материала (с защитой от -1)
			int matId = shape.mesh.material_ids[f];
			if (matId < 0)
				matId = 0;

			// Инициализация границ
			if (materialBounds.find(matId) == materialBounds.end())
			{
				materialBounds[matId] = {Math::float3(FLT_MAX), Math::float3(-FLT_MAX)};
			}

			// Добавляем индексы вершин этого треугольника в группу материала
			for (size_t v = 0; v < fv; v++)
			{
				tinyobj::index_t idx = shape.mesh.indices[index_offset + v];
				materialGroups[matId].push_back(idx);

				// Предварительный расчет границ (по сырым данным)
				Math::float3 pos(attrib.vertices[3 * idx.vertex_index + 0], attrib.vertices[3 * idx.vertex_index + 1],
								 attrib.vertices[3 * idx.vertex_index + 2]);

				// Глобальные границы
				m_MinBound = Math::float3::min(m_MinBound, pos);
				m_MaxBound = Math::float3::max(m_MaxBound, pos);

				// Локальные границы
				auto& bounds = materialBounds[matId];
				bounds.first = Math::float3::min(bounds.first, pos);
				bounds.second = Math::float3::max(bounds.second, pos);
			}
			index_offset += fv;
		}
	}

	// Теперь строим финальные буферы, проходя по материалам
	for (const auto& [matId, indices] : materialGroups)
	{
		SubMesh subMesh;
		subMesh.MaterialIndex = matId;
		subMesh.IndexStart = (uint32_t)finalIndices.size();
		subMesh.BoundsMin = materialBounds[matId].first;
		subMesh.BoundsMax = materialBounds[matId].second;

		for (const auto& idx : indices)
		{
			Vertex vert;

			// Position
			vert.Position.x = attrib.vertices[3 * idx.vertex_index + 0];
			vert.Position.y = attrib.vertices[3 * idx.vertex_index + 1];
			vert.Position.z = attrib.vertices[3 * idx.vertex_index + 2];

			// Normal
			if (idx.normal_index >= 0)
			{
				vert.Normal.x = attrib.normals[3 * idx.normal_index + 0];
				vert.Normal.y = attrib.normals[3 * idx.normal_index + 1];
				vert.Normal.z = attrib.normals[3 * idx.normal_index + 2];
			}
			else
			{
				// Если нет нормалей, ставим заглушку (в идеале нужно пересчитать после загрузки)
				vert.Normal = Math::float3(0, 1, 0);
			}

			// UV
			if (idx.texcoord_index >= 0)
			{
				vert.UV.x = attrib.texcoords[2 * idx.texcoord_index + 0];
				// TinyObj v-flip для DirectX
				vert.UV.y = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
			}
			else
			{
				vert.UV = {0, 0};
			}

			// ДЕДУПЛИКАЦИЯ
			// Проверяем, есть ли такая вершина уже
			auto it = uniqueVertices.find(vert);
			if (it != uniqueVertices.end())
			{
				// Вершина уже есть, используем существующий индекс
				finalIndices.push_back(it->second);
			}
			else
			{
				// Новая вершина
				unsigned int newIndex = (unsigned int)finalVertices.size();
				finalVertices.push_back(vert);
				finalIndices.push_back(newIndex);
				uniqueVertices[vert] = newIndex;
			}
		}

		subMesh.IndexCount = (uint32_t)finalIndices.size() - subMesh.IndexStart;
		m_SubMeshes.push_back(subMesh);
	}

	// Отправка в GPU
	Create(finalVertices, finalIndices);

	return true;
}

// Вспомогательная функция для добавления квада (два треугольника)
void AddQuad(std::vector<unsigned int>& indices, int i0, int i1, int i2, int i3)
{
	indices.push_back(i0);
	indices.push_back(i1);
	indices.push_back(i2);
	indices.push_back(i0);
	indices.push_back(i2);
	indices.push_back(i3);
}

void Mesh::GenerateCube(Mesh& outMesh, float size)
{
	float h = size * 0.5f;

	// Нам нужно 24 вершины (4 на грань * 6 граней), чтобы UV и Нормали были жесткими
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	vertices.reserve(24);

	Math::float3 norm, tan, bitan;

	auto buildFace = [&](Math::float3 p1, Math::float3 p2, Math::float3 p3, Math::float3 p4, Math::float3 n) {
		int baseIndex = (int)vertices.size();
		// Tangent считается как p2 - p1 (U направление)
		Math::float3 t = (p2 - p1).normalize();
		Math::float3 b = Math::float3::cross(n, t);

		vertices.push_back(Vertex(p1, t, b, n, {0, 1})); // Bottom Left
		vertices.push_back(Vertex(p2, t, b, n, {0, 0})); // Top Left
		vertices.push_back(Vertex(p3, t, b, n, {1, 0})); // Top Right
		vertices.push_back(Vertex(p4, t, b, n, {1, 1})); // Bottom Right

		AddQuad(indices, baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 3);
	};

	// Front (Z-)
	buildFace({-h, -h, -h}, {-h, h, -h}, {h, h, -h}, {h, -h, -h}, {0, 0, -1});
	// Back (Z+)
	buildFace({h, -h, h}, {h, h, h}, {-h, h, h}, {-h, -h, h}, {0, 0, 1});
	// Left (X-)
	buildFace({-h, -h, h}, {-h, h, h}, {-h, h, -h}, {-h, -h, -h}, {-1, 0, 0});
	// Right (X+)
	buildFace({h, -h, -h}, {h, h, -h}, {h, h, h}, {h, -h, h}, {1, 0, 0});
	// Top (Y+)
	buildFace({-h, h, -h}, {-h, h, h}, {h, h, h}, {h, h, -h}, {0, 1, 0});
	// Bottom (Y-)
	buildFace({-h, -h, h}, {-h, -h, -h}, {h, -h, -h}, {h, -h, h}, {0, -1, 0});

	float half = size * 0.5f;
	outMesh.m_MinBound = Math::float3(-half, -half, -half);
	outMesh.m_MaxBound = Math::float3(half, half, half);

	outMesh.Create(vertices, indices);
}

void Mesh::GeneratePlane(Mesh& outMesh, float width, float depth)
{
	float hw = width * 0.5f;
	float hd = depth * 0.5f;

	std::vector<Vertex> v;
	std::vector<unsigned int> i;

	Math::float3 n(0, 1, 0);
	Math::float3 t(1, 0, 0);
	Math::float3 b(0, 0, 1);

	v.push_back(Vertex({-hw, 0, -hd}, t, b, n, {0, 1}));
	v.push_back(Vertex({-hw, 0, hd}, t, b, n, {0, 0}));
	v.push_back(Vertex({hw, 0, hd}, t, b, n, {1, 0}));
	v.push_back(Vertex({hw, 0, -hd}, t, b, n, {1, 1}));

	AddQuad(i, 0, 1, 2, 3);

	float hw = width * 0.5f;
	float hd = depth * 0.5f;
	outMesh.m_MinBound = Math::float3(-hw, 0.0f, -hd);
	outMesh.m_MaxBound = Math::float3(hw, 0.0f, hd);

	outMesh.Create(v, i);
}

void Mesh::GenerateScreenQuad(Mesh& outMesh)
{
	// Квадрат в плоскости XY от -1 до 1 (NDC coordinates)
	std::vector<Vertex> v;
	std::vector<unsigned int> i;

	Math::float3 n(0, 0, -1); // Смотрит в камеру
	Math::float3 t(1, 0, 0);
	Math::float3 b(0, 1, 0);

	// Fullscreen Triangles
	v.push_back(Vertex({-1, -1, 0}, t, b, n, {0, 1}));
	v.push_back(Vertex({-1, 1, 0}, t, b, n, {0, 0}));
	v.push_back(Vertex({1, 1, 0}, t, b, n, {1, 0}));
	v.push_back(Vertex({1, -1, 0}, t, b, n, {1, 1}));

	AddQuad(i, 0, 1, 2, 3);
	outMesh.Create(v, i);
}

void Mesh::GenerateSphere(Mesh& outMesh, float radius, int segments, int rings)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	for (int y = 0; y <= rings; y++)
	{
		for (int x = 0; x <= segments; x++)
		{
			float xSegment = (float)x / (float)segments;
			float ySegment = (float)y / (float)rings;

			float xPos = std::cos(xSegment * 2.0f * Math::Constants::PI) * std::sin(ySegment * Math::Constants::PI);
			float yPos = std::cos(ySegment * Math::Constants::PI);
			float zPos = std::sin(xSegment * 2.0f * Math::Constants::PI) * std::sin(ySegment * Math::Constants::PI);

			Math::float3 pos(xPos, yPos, zPos);
			Math::float3 normal = pos; // Для единичной сферы нормаль равна позиции
			pos = pos * radius;

			Math::float2 uv(xSegment, ySegment);

			// Простая аппроксимация касательных для сферы
			Math::float3 t = Math::float3::cross({0, 1, 0}, normal);
			if (t.length_sq() < 0.001f)
				t = {1, 0, 0};
			t = t.normalize();
			Math::float3 b = Math::float3::cross(normal, t);

			vertices.push_back(Vertex(pos, t, b, normal, uv));
		}
	}

	for (int y = 0; y < rings; y++)
	{
		for (int x = 0; x < segments; x++)
		{
			indices.push_back((y + 1) * (segments + 1) + x);
			indices.push_back(y * (segments + 1) + x);
			indices.push_back(y * (segments + 1) + x + 1);

			indices.push_back((y + 1) * (segments + 1) + x);
			indices.push_back(y * (segments + 1) + x + 1);
			indices.push_back((y + 1) * (segments + 1) + x + 1);
		}
	}

	outMesh.m_MinBound = Math::float3(-radius);
	outMesh.m_MaxBound = Math::float3(radius);

	outMesh.Create(vertices, indices);
}

void Mesh::GenerateHemisphere(Mesh& outMesh, float radius, int segments, int rings, bool flatBottom)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	// Купол (то же самое что сфера, но ySegment идет только до 0.5)
	// Но мы пересчитаем диапазоны, чтобы сохранить распределение вершин
	int domeRings = rings;

	for (int y = 0; y <= domeRings; y++)
	{
		for (int x = 0; x <= segments; x++)
		{
			float xSegment = (float)x / (float)segments;
			float ySegment = (float)y / (float)domeRings; // 0..1

			// Ограничиваем угол PI/2 (90 градусов)
			float phi = ySegment * (Math::Constants::PI * 0.5f);

			float xPos = std::cos(xSegment * 2.0f * Math::Constants::PI) * std::sin(phi);
			float yPos = std::cos(phi); // y идет от 1 (top) до 0 (equator)
			float zPos = std::sin(xSegment * 2.0f * Math::Constants::PI) * std::sin(phi);

			Math::float3 normal(xPos, yPos, zPos);
			normal = normal.normalize();
			Math::float3 pos = normal * radius;

			Math::float3 t = Math::float3::cross({0, 1, 0}, normal);
			if (t.length_sq() < 0.001f)
				t = {1, 0, 0};
			t = t.normalize();
			Math::float3 b = Math::float3::cross(normal, t);

			vertices.push_back(Vertex(pos, t, b, normal, {xSegment, ySegment}));
		}
	}

	// Индексы купола
	for (int y = 0; y < domeRings; y++)
	{
		for (int x = 0; x < segments; x++)
		{
			int stride = segments + 1;
			indices.push_back((y + 1) * stride + x);
			indices.push_back(y * stride + x);
			indices.push_back(y * stride + x + 1);

			indices.push_back((y + 1) * stride + x);
			indices.push_back(y * stride + x + 1);
			indices.push_back((y + 1) * stride + x + 1);
		}
	}

	if (flatBottom)
	{
		// Создаем центральную точку снизу
		int centerIndex = (int)vertices.size();
		Vertex centerV;
		centerV.Position = {0, 0, 0};
		centerV.Normal = {0, -1, 0}; // Смотрит вниз
		centerV.UV = {0.5f, 0.5f};
		centerV.Tangent = {1, 0, 0};
		centerV.Bitangent = {0, 0, 1};
		vertices.push_back(centerV);

		// Добавляем вершины по периметру (кольцо экватора) еще раз, но с нормалью вниз
		int ringStartIndex = (int)vertices.size();
		for (int x = 0; x <= segments; x++)
		{
			float angle = ((float)x / segments) * 2.0f * Math::Constants::PI;
			float cx = std::cos(angle) * radius;
			float cz = std::sin(angle) * radius;

			Vertex v;
			v.Position = {cx, 0, cz};
			v.Normal = {0, -1, 0};
			v.Tangent = {1, 0, 0};
			v.Bitangent = {0, 0, 1};
			v.UV = {cx / (2 * radius) + 0.5f, cz / (2 * radius) + 0.5f}; // UV маппим как круг на квадрате
			vertices.push_back(v);
		}

		for (int x = 0; x < segments; x++)
		{
			indices.push_back(centerIndex);
			indices.push_back(ringStartIndex + x + 1);
			indices.push_back(ringStartIndex + x);
		}
	}

	outMesh.m_MinBound = Math::float3(-radius);
	outMesh.m_MaxBound = Math::float3(radius);

	outMesh.Create(vertices, indices);
}

void Mesh::GenerateDisc(Mesh& outMesh, float radius, int segments)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	Math::float3 n(0, 1, 0);
	Math::float3 t(1, 0, 0);
	Math::float3 b(0, 0, 1);

	// Центр
	vertices.push_back(Vertex({0, 0, 0}, t, b, n, {0.5f, 0.5f}));

	for (int i = 0; i <= segments; ++i)
	{
		float angle = ((float)i / segments) * 2.0f * Math::Constants::PI;
		float x = std::cos(angle) * radius;
		float z = std::sin(angle) * radius;

		Math::float2 uv = {x / (2 * radius) + 0.5f, z / (2 * radius) + 0.5f};
		vertices.push_back(Vertex({x, 0, z}, t, b, n, uv));
	}

	for (int i = 0; i < segments; ++i)
	{
		indices.push_back(0);
		indices.push_back(i + 2); // winding order clockwise
		indices.push_back(i + 1);
	}

	outMesh.m_MinBound = Math::float3(-radius, 0.0f, -radius);
	outMesh.m_MaxBound = Math::float3(radius, 0.0f, radius);

	outMesh.Create(vertices, indices);
}

void Mesh::GenerateTriangle(Mesh& outMesh, float size)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	float h = size * std::sqrt(3.0f) / 2.0f; // высота равностороннего
	float zBottom = -h / 3.0f;				 // Центрируем (барицентр)
	float zTop = 2.0f * h / 3.0f;
	float half = size * 0.5f;

	Math::float3 n(0, 1, 0);
	Math::float3 t(1, 0, 0);
	Math::float3 b(0, 0, 1);

	vertices.push_back(Vertex({0, 0, zTop}, t, b, n, {0.5f, 0.0f}));		// Top
	vertices.push_back(Vertex({half, 0, zBottom}, t, b, n, {1.0f, 1.0f}));	// Bottom Right
	vertices.push_back(Vertex({-half, 0, zBottom}, t, b, n, {0.0f, 1.0f})); // Bottom Left

	indices.push_back(0);
	indices.push_back(1);
	indices.push_back(2);

	outMesh.m_MinBound = Math::float3(half, 0.0f, half);
	outMesh.m_MaxBound = Math::float3(-half, h, -half);

	outMesh.Create(vertices, indices);
}
