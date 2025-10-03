//--------------------------------------------------------------------------------------
// File: MeshLoader.h
//
// Wrapper class for ID3DXMesh interface. Handles loading mesh data from an .obj file
// and resource management for material textures.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma once
//--------------------------------------------------------------------------------------
#include "render_backend_DX9_include.h"
#include "growable_array.h"
//--------------------------------------------------------------------------------------
struct VERTEX
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 tangent;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texcoord;
};

// Used for a hashtable vertex cache when creating the mesh from a .obj file
struct CacheEntry
{
	UINT index;
	CacheEntry* pNext;
};

//--------------------------------------------------------------------------------------
struct Material
{
	CHAR strName[MAX_PATH];

	CHAR strTextureAlbedo[MAX_PATH];
	IDirect3DTexture9* pTextureAlbedo;

	bool bHaveNormal;
	CHAR strTextureNormal[MAX_PATH];
	IDirect3DTexture9* pTextureNormal;

	bool bHaveRoughness;
	CHAR strTextureRoughness[MAX_PATH];
	IDirect3DTexture9* pTextureRoughness;

	bool bHaveMetallic;
	CHAR strTextureMetallic[MAX_PATH];
	IDirect3DTexture9* pTextureMetallic;

	bool bHaveAO;
	CHAR strTextureAO[MAX_PATH];
	IDirect3DTexture9* pTextureAO;
};

class CMesh
{
  private:
	void InitMaterial(Material* pMaterial);

	HRESULT LoadGeometryFromOBJ(const std::string& strFilePath, const std::string& strFileName);
	HRESULT LoadMaterialsFromMTL(const std::string& strFilePath, const std::string& strFileName);
	HRESULT LoadTextures();
	HRESULT CreateMesh();

	DWORD AddVertex(UINT hash, VERTEX* pVertex);
	void DeleteCache();

	ID3DXMesh* m_pMesh; // Encapsulated D3DX Mesh

	CGrowableArray<CacheEntry*> m_VertexCache; // Hashtable cache for locating duplicate vertices
	CGrowableArray<VERTEX> m_Vertices;		   // Filled and copied to the vertex buffer
	CGrowableArray<DWORD> m_Indices;		   // Filled and copied to the index buffer
	CGrowableArray<DWORD> m_Attributes;		   // Filled and copied to the attribute buffer
	CGrowableArray<Material*> m_Materials;	   // Holds material properties per subset

  public:
	HRESULT Create(const std::string& strFilePath, const std::string& strFilename);

	void DrawSubsets();

	UINT GetNumMaterials() const
	{
		return m_Materials.GetSize();
	}

	Material* GetMaterial(UINT iMaterial)
	{
		return m_Materials.GetAt(iMaterial);
	}

	ID3DXMesh* GetMesh()
	{
		return m_pMesh;
	}

	void Destroy();

	CMesh();
	~CMesh();
};
