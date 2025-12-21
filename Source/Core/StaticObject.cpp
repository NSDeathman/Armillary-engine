#include "stdafx.h"
#include "StaticObject.h"
#include <Rendeructor/Rendeructor.h>
#include "Render.h"
#include "Scene.h"
#include "Camera.h"

namespace Core::World
{
CStaticObject::CStaticObject(const std::string& name, std::shared_ptr<StaticModel> model) : CGameObject(name), m_Model(model)
{
	m_Type = ObjectType::Static;
}

void CStaticObject::SetShader(const ShaderPass& pass)
{
	m_ObjectShader = pass;
	m_HasShader = true;
}

void CStaticObject::Draw()
{
	if (!m_Model || !m_Model->MeshData || !m_HasShader)
		return;

	// 1. Устанавливаем шейдерный пас
	RenderBackend.SetShaderPass(m_ObjectShader);

	// 2. Устанавливаем текстуру (если есть)
	if (m_Model->Albedo && m_Model->Albedo->GetHandle())
	{
		m_ObjectShader.AddTexture("txDiffuse", *m_Model->Albedo);
		// После добавления текстуры нужно снова установить шейдерный пас
		RenderBackend.SetShaderPass(m_ObjectShader);
	}

	// 3. Устанавливаем матрицы
	auto camera = Renderer.GetCurrentCamera();
	if (camera)
	{
		Math::float4x4 world = m_Transform.GetMatrix();
		Math::float4x4 view = camera->GetViewMatrix();
		Math::float4x4 proj = camera->GetProjectionMatrix();

		// Вариант A: Две матрицы (как в работающем примере для инстанцированных объектов)
		Math::float4x4 worldTransposed = world.transposed();
		Math::float4x4 viewProjTransposed = (view * proj).transposed();

		RenderBackend.SetConstant("World", worldTransposed);
		RenderBackend.SetConstant("ViewProjection", viewProjTransposed);

		// ИЛИ Вариант B: Одна матрица (как в работающем примере для пола)
		// Math::float4x4 wvpTransposed = (world * view * proj).transposed();
		// RenderBackend.SetConstant("WorldViewProjection", wvpTransposed);
	}

	// 4. Рисуем меш
	RenderBackend.DrawMesh(*m_Model->MeshData);
}
} // namespace Core::World
