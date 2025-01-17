//--------------------------------------------------------------------------------------
// File: MeshLoader.h
//
// Wrapper class for ID3DXMesh interface. Handles loading mesh data from an .obj file
// and resource management for material textures.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#ifndef _MESHLOADER_H_
#define _MESHLOADER_H_
#pragma once

#include "stdafx.h"
#include "growable_array.h"

// Vertex format
struct VERTEX
{
	D3DXVECTOR3 position;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texcoord;
};

// Used for a hashtable vertex cache when creating the mesh from a .obj file
struct CacheEntry
{
	UINT index;
	CacheEntry* pNext;
};

// Material properties per mesh subset
struct Material
{
	CHAR strName[MAX_PATH];

	D3DXVECTOR3 vAmbient;
	D3DXVECTOR3 vDiffuse;
	D3DXVECTOR3 vSpecular;

	int nShininess;
	float fAlpha;

	bool bSpecular;

	CHAR strTexture[MAX_PATH];
	IDirect3DTexture9* pTexture;
	D3DXHANDLE hTechnique;
};

class CMeshLoader
{
  public:
	CMeshLoader();
	~CMeshLoader();

	void Destroy();

	HRESULT Create(IDirect3DDevice9* pd3dDevice, const CHAR* strFilename);

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
	CHAR* GetMediaDirectory()
	{
		return m_strMediaDir;
	}

  private:
	HRESULT LoadGeometryFromOBJ(const CHAR* strFileName);
	HRESULT LoadMaterialsFromMTL(const CHAR* strFileName);
	void InitMaterial(Material* pMaterial);

	DWORD AddVertex(UINT hash, VERTEX* pVertex);
	void DeleteCache();

	IDirect3DDevice9* m_pd3dDevice; // Direct3D Device object associated with this mesh
	ID3DXMesh* m_pMesh;				// Encapsulated D3DX Mesh

	CGrowableArray<CacheEntry*> m_VertexCache; // Hashtable cache for locating duplicate vertices
	CGrowableArray<VERTEX> m_Vertices;		   // Filled and copied to the vertex buffer
	CGrowableArray<DWORD> m_Indices;		   // Filled and copied to the index buffer
	CGrowableArray<DWORD> m_Attributes;		   // Filled and copied to the attribute buffer
	CGrowableArray<Material*> m_Materials;	   // Holds material properties per subset

	CHAR m_strMediaDir[MAX_PATH]; // Directory where the mesh was found
};

#endif // _MESHLOADER_H_
