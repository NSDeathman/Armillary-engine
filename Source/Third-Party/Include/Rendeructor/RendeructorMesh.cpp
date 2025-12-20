#include "pch.h"
#include "Rendeructor.h"
#include "BackendDX11.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include <TinyObjLoader/TinyObjLoader.h>

void Mesh::Create(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices) {
    if (Rendeructor::GetCurrent() && Rendeructor::GetCurrent()->GetBackendAPI()) {

        m_vbHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateVertexBuffer(
            vertices.data(),
            vertices.size() * sizeof(Vertex),
            sizeof(Vertex)
        );

        m_ibHandle = Rendeructor::GetCurrent()->GetBackendAPI()->CreateIndexBuffer(
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );

        m_indexCount = (int)indices.size();
    }
}

bool Mesh::LoadFromOBJ(const std::string& filepath) {
    tinyobj::ObjReaderConfig reader_config;
    reader_config.mtl_search_path = "";
    reader_config.triangulate = true;

    tinyobj::ObjReader reader;

    if (!reader.ParseFromFile(filepath, reader_config)) {
        if (!reader.Error().empty()) {
            std::cerr << "[TinyObj Error] " << reader.Error() << std::endl;
        }
        return false;
    }

    const auto& attrib = reader.GetAttrib();
    const auto& shapes = reader.GetShapes();

    bool hasNormals = !attrib.normals.empty();

    // Для вычисления нормалей: создаем массив нормалей для каждой уникальной позиции
    std::vector<Math::float3> positionNormals(attrib.vertices.size() / 3, { 0, 0, 0 });
    std::vector<int> positionFaceCount(attrib.vertices.size() / 3, 0);

    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // ПЕРВЫЙ ПРОХОД: собираем информацию о треугольниках для вычисления нормалей
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            // Получаем индексы позиций для этого треугольника
            tinyobj::index_t idx0 = shapes[s].mesh.indices[index_offset + 0];
            tinyobj::index_t idx1 = shapes[s].mesh.indices[index_offset + 1];
            tinyobj::index_t idx2 = shapes[s].mesh.indices[index_offset + 2];

            // Получаем позиции вершин
            Math::float3 p0 = {
                attrib.vertices[3 * idx0.vertex_index + 0],
                attrib.vertices[3 * idx0.vertex_index + 1],
                attrib.vertices[3 * idx0.vertex_index + 2]
            };

            Math::float3 p1 = {
                attrib.vertices[3 * idx1.vertex_index + 0],
                attrib.vertices[3 * idx1.vertex_index + 1],
                attrib.vertices[3 * idx1.vertex_index + 2]
            };

            Math::float3 p2 = {
                attrib.vertices[3 * idx2.vertex_index + 0],
                attrib.vertices[3 * idx2.vertex_index + 1],
                attrib.vertices[3 * idx2.vertex_index + 2]
            };

            // Вычисляем нормаль треугольника
            Math::float3 edge1 = p1 - p0;
            Math::float3 edge2 = p2 - p0;
            Math::float3 faceNormal = Math::float3::cross(edge1, edge2);

            // Накопление нормали для каждой позиции вершины
            positionNormals[idx0.vertex_index] = positionNormals[idx0.vertex_index] + faceNormal;
            positionNormals[idx1.vertex_index] = positionNormals[idx1.vertex_index] + faceNormal;
            positionNormals[idx2.vertex_index] = positionNormals[idx2.vertex_index] + faceNormal;

            positionFaceCount[idx0.vertex_index]++;
            positionFaceCount[idx1.vertex_index]++;
            positionFaceCount[idx2.vertex_index]++;

            index_offset += fv;
        }
    }

    // Нормализуем накопленные нормали
    for (size_t i = 0; i < positionNormals.size(); i++) {
        if (positionFaceCount[i] > 0) {
            positionNormals[i] = positionNormals[i].normalize();
        }
    }

    // ВТОРОЙ ПРОХОД: создаем вершины
    for (size_t s = 0; s < shapes.size(); s++) {
        size_t index_offset = 0;

        for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
            int fv = shapes[s].mesh.num_face_vertices[f];

            for (size_t v = 0; v < fv; v++) {
                tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
                Vertex vertex;

                // Position
                vertex.Position.x = attrib.vertices[3 * idx.vertex_index + 0];
                vertex.Position.y = attrib.vertices[3 * idx.vertex_index + 1];
                vertex.Position.z = attrib.vertices[3 * idx.vertex_index + 2];

                // Normal (берем из файла или вычисленную)
                if (hasNormals && idx.normal_index >= 0) {
                    vertex.Normal.x = attrib.normals[3 * idx.normal_index + 0];
                    vertex.Normal.y = attrib.normals[3 * idx.normal_index + 1];
                    vertex.Normal.z = attrib.normals[3 * idx.normal_index + 2];
                }
                else {
                    // Используем вычисленную нормаль для этой позиции
                    vertex.Normal = positionNormals[idx.vertex_index];

                    // Если нормаль нулевая (маловероятно, но на всякий случай)
                    if (vertex.Normal.length_sq() < 0.0001f) {
                        vertex.Normal = { 0, 1, 0 };
                    }
                }

                // UV
                if (idx.texcoord_index >= 0) {
                    vertex.UV.x = attrib.texcoords[2 * idx.texcoord_index + 0];
                    vertex.UV.y = 1.0f - attrib.texcoords[2 * idx.texcoord_index + 1];
                }
                else {
                    vertex.UV = { 0, 0 };
                }

                // Пока оставляем tangent/bitangent нулевыми
                vertex.Tangent = { 0, 0, 0 };
                vertex.Bitangent = { 0, 0, 0 };

                vertices.push_back(vertex);
                indices.push_back((unsigned int)vertices.size() - 1);
            }
            index_offset += fv;
        }
    }

    // Создаем буферы
    Create(vertices, indices);

    std::cout << "[Mesh] Loaded: " << filepath
        << "\n  Vertices: " << vertices.size()
        << "\n  Indices: " << indices.size()
        << "\n  Normals: " << (hasNormals ? "from file" : "computed from " + std::to_string(positionNormals.size()) + " unique positions")
        << std::endl;

    return true;
}

// Вспомогательная функция для добавления квада (два треугольника)
void AddQuad(std::vector<unsigned int>& indices, int i0, int i1, int i2, int i3) {
    indices.push_back(i0); indices.push_back(i1); indices.push_back(i2);
    indices.push_back(i0); indices.push_back(i2); indices.push_back(i3);
}

void Mesh::GenerateCube(Mesh& outMesh, float size) {
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

        vertices.push_back(Vertex(p1, t, b, n, { 0, 1 })); // Bottom Left
        vertices.push_back(Vertex(p2, t, b, n, { 0, 0 })); // Top Left
        vertices.push_back(Vertex(p3, t, b, n, { 1, 0 })); // Top Right
        vertices.push_back(Vertex(p4, t, b, n, { 1, 1 })); // Bottom Right

        AddQuad(indices, baseIndex, baseIndex + 1, baseIndex + 2, baseIndex + 3);
    };

    // Front (Z-)
    buildFace({ -h, -h, -h }, { -h,  h, -h }, { h,  h, -h }, { h, -h, -h }, { 0, 0, -1 });
    // Back (Z+)
    buildFace({ h, -h,  h }, { h,  h,  h }, { -h,  h,  h }, { -h, -h,  h }, { 0, 0, 1 });
    // Left (X-)
    buildFace({ -h, -h,  h }, { -h,  h,  h }, { -h,  h, -h }, { -h, -h, -h }, { -1, 0, 0 });
    // Right (X+)
    buildFace({ h, -h, -h }, { h,  h, -h }, { h,  h,  h }, { h, -h,  h }, { 1, 0, 0 });
    // Top (Y+)
    buildFace({ -h,  h, -h }, { -h,  h,  h }, { h,  h,  h }, { h,  h, -h }, { 0, 1, 0 });
    // Bottom (Y-)
    buildFace({ -h, -h,  h }, { -h, -h, -h }, { h, -h, -h }, { h, -h,  h }, { 0, -1, 0 });

    outMesh.Create(vertices, indices);
}

void Mesh::GeneratePlane(Mesh& outMesh, float width, float depth) {
    float hw = width * 0.5f;
    float hd = depth * 0.5f;

    std::vector<Vertex> v;
    std::vector<unsigned int> i;

    Math::float3 n(0, 1, 0);
    Math::float3 t(1, 0, 0);
    Math::float3 b(0, 0, 1);

    v.push_back(Vertex({ -hw, 0, -hd }, t, b, n, { 0, 1 }));
    v.push_back(Vertex({ -hw, 0,  hd }, t, b, n, { 0, 0 }));
    v.push_back(Vertex({ hw, 0,  hd }, t, b, n, { 1, 0 }));
    v.push_back(Vertex({ hw, 0, -hd }, t, b, n, { 1, 1 }));

    AddQuad(i, 0, 1, 2, 3);
    outMesh.Create(v, i);
}

void Mesh::GenerateScreenQuad(Mesh& outMesh) {
    // Квадрат в плоскости XY от -1 до 1 (NDC coordinates)
    std::vector<Vertex> v;
    std::vector<unsigned int> i;

    Math::float3 n(0, 0, -1); // Смотрит в камеру
    Math::float3 t(1, 0, 0);
    Math::float3 b(0, 1, 0);

    // Fullscreen Triangles
    v.push_back(Vertex({ -1, -1, 0 }, t, b, n, { 0, 1 }));
    v.push_back(Vertex({ -1,  1, 0 }, t, b, n, { 0, 0 }));
    v.push_back(Vertex({ 1,  1, 0 }, t, b, n, { 1, 0 }));
    v.push_back(Vertex({ 1, -1, 0 }, t, b, n, { 1, 1 }));

    AddQuad(i, 0, 1, 2, 3);
    outMesh.Create(v, i);
}

void Mesh::GenerateSphere(Mesh& outMesh, float radius, int segments, int rings) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    for (int y = 0; y <= rings; y++) {
        for (int x = 0; x <= segments; x++) {
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
            Math::float3 t = Math::float3::cross({ 0, 1, 0 }, normal);
            if (t.length_sq() < 0.001f) t = { 1, 0, 0 };
            t = t.normalize();
            Math::float3 b = Math::float3::cross(normal, t);

            vertices.push_back(Vertex(pos, t, b, normal, uv));
        }
    }

    for (int y = 0; y < rings; y++) {
        for (int x = 0; x < segments; x++) {
            indices.push_back((y + 1) * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + x + 1);

            indices.push_back((y + 1) * (segments + 1) + x);
            indices.push_back(y * (segments + 1) + x + 1);
            indices.push_back((y + 1) * (segments + 1) + x + 1);
        }
    }
    outMesh.Create(vertices, indices);
}

void Mesh::GenerateHemisphere(Mesh& outMesh, float radius, int segments, int rings, bool flatBottom) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    // Купол (то же самое что сфера, но ySegment идет только до 0.5)
    // Но мы пересчитаем диапазоны, чтобы сохранить распределение вершин
    int domeRings = rings;

    for (int y = 0; y <= domeRings; y++) {
        for (int x = 0; x <= segments; x++) {
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

            Math::float3 t = Math::float3::cross({ 0, 1, 0 }, normal);
            if (t.length_sq() < 0.001f) t = { 1, 0, 0 };
            t = t.normalize();
            Math::float3 b = Math::float3::cross(normal, t);

            vertices.push_back(Vertex(pos, t, b, normal, { xSegment, ySegment }));
        }
    }

    // Индексы купола
    for (int y = 0; y < domeRings; y++) {
        for (int x = 0; x < segments; x++) {
            int stride = segments + 1;
            indices.push_back((y + 1) * stride + x);
            indices.push_back(y * stride + x);
            indices.push_back(y * stride + x + 1);

            indices.push_back((y + 1) * stride + x);
            indices.push_back(y * stride + x + 1);
            indices.push_back((y + 1) * stride + x + 1);
        }
    }

    if (flatBottom) {
        // Создаем центральную точку снизу
        int centerIndex = (int)vertices.size();
        Vertex centerV;
        centerV.Position = { 0, 0, 0 };
        centerV.Normal = { 0, -1, 0 }; // Смотрит вниз
        centerV.UV = { 0.5f, 0.5f };
        centerV.Tangent = { 1, 0, 0 };
        centerV.Bitangent = { 0, 0, 1 };
        vertices.push_back(centerV);

        // Добавляем вершины по периметру (кольцо экватора) еще раз, но с нормалью вниз
        int ringStartIndex = (int)vertices.size();
        for (int x = 0; x <= segments; x++) {
            float angle = ((float)x / segments) * 2.0f * Math::Constants::PI;
            float cx = std::cos(angle) * radius;
            float cz = std::sin(angle) * radius;

            Vertex v;
            v.Position = { cx, 0, cz };
            v.Normal = { 0, -1, 0 };
            v.Tangent = { 1, 0, 0 }; v.Bitangent = { 0, 0, 1 };
            v.UV = { cx / (2 * radius) + 0.5f, cz / (2 * radius) + 0.5f };// UV маппим как круг на квадрате
            vertices.push_back(v);
        }

        for (int x = 0; x < segments; x++) {
            indices.push_back(centerIndex);
            indices.push_back(ringStartIndex + x + 1);
            indices.push_back(ringStartIndex + x);
        }
    }

    outMesh.Create(vertices, indices);
}

void Mesh::GenerateDisc(Mesh& outMesh, float radius, int segments) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Math::float3 n(0, 1, 0);
    Math::float3 t(1, 0, 0);
    Math::float3 b(0, 0, 1);

    // Центр
    vertices.push_back(Vertex({ 0,0,0 }, t, b, n, { 0.5f, 0.5f }));

    for (int i = 0; i <= segments; ++i) {
        float angle = ((float)i / segments) * 2.0f * Math::Constants::PI;
        float x = std::cos(angle) * radius;
        float z = std::sin(angle) * radius;

        Math::float2 uv = { x / (2 * radius) + 0.5f, z / (2 * radius) + 0.5f };
        vertices.push_back(Vertex({ x, 0, z }, t, b, n, uv));
    }

    for (int i = 0; i < segments; ++i) {
        indices.push_back(0);
        indices.push_back(i + 2); // winding order clockwise
        indices.push_back(i + 1);
    }

    outMesh.Create(vertices, indices);
}

void Mesh::GenerateTriangle(Mesh& outMesh, float size) {
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    float h = size * std::sqrt(3.0f) / 2.0f; // высота равностороннего
    float zBottom = -h / 3.0f;    // Центрируем (барицентр)
    float zTop = 2.0f * h / 3.0f;

    Math::float3 n(0, 1, 0);
    Math::float3 t(1, 0, 0);
    Math::float3 b(0, 0, 1);

    vertices.push_back(Vertex({ 0, 0, zTop }, t, b, n, { 0.5f, 0.0f })); // Top
    vertices.push_back(Vertex({ size * 0.5f, 0, zBottom }, t, b, n, { 1.0f, 1.0f })); // Bottom Right
    vertices.push_back(Vertex({ -size * 0.5f, 0, zBottom }, t, b, n, { 0.0f, 1.0f })); // Bottom Left

    indices.push_back(0);
    indices.push_back(1);
    indices.push_back(2);

    outMesh.Create(vertices, indices);
}