//--------------------------------------------------------------------------------------
// File: MeshLoader.cpp
//
// Wrapper class for ID3DXMesh interface. Handles loading mesh data from an .obj file
// and resource management for material textures.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#pragma warning(disable : 4995)
//--------------------------------------------------------------------------------------
#include "stdafx.h"
#include "meshloader.h"
#include <fstream>
#include "log.h"
#include "render_DX9.h"
#include "render_backend_DX9_deprecated.h"
#include "defines.h"
//--------------------------------------------------------------------------------------
using namespace std;
using namespace Core;
//--------------------------------------------------------------------------------------
#pragma warning(default : 4995)
//--------------------------------------------------------------------------------------
D3DVERTEXELEMENT9 VERTEX_DECLARATION[] = 
{ 
	{0, 0, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
	{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	{0, 36, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END() 
};

//--------------------------------------------------------------------------------------
CMesh::CMesh()
{
	m_pMesh = NULL;
}

//--------------------------------------------------------------------------------------
CMesh::~CMesh()
{
	Destroy();
}

//--------------------------------------------------------------------------------------
void CMesh::Destroy()
{
	concurrency::parallel_for(int(0), m_Materials.GetSize(), [&](int iMaterial) 
	{
		Material* pMaterial = m_Materials.GetAt(iMaterial);

		// Avoid releasing the same texture twice
		for (int x = iMaterial + 1; x < m_Materials.GetSize(); x++)
		{
			Material* pCur = m_Materials.GetAt(x);
			if (pCur->pTextureAlbedo == pMaterial->pTextureAlbedo)
				pCur->pTextureAlbedo = NULL;

			if (pCur->pTextureNormal == pMaterial->pTextureNormal)
				pCur->pTextureNormal = NULL;

			if (pCur->pTextureRoughness == pMaterial->pTextureRoughness)
				pCur->pTextureRoughness = NULL;

			if (pCur->pTextureMetallic == pMaterial->pTextureMetallic)
				pCur->pTextureMetallic = NULL;
		}

		SAFE_RELEASE(pMaterial->pTextureAlbedo);
		SAFE_RELEASE(pMaterial->pTextureNormal);
		SAFE_RELEASE(pMaterial->pTextureRoughness);
		SAFE_RELEASE(pMaterial->pTextureMetallic);
		SAFE_RELEASE(pMaterial->pTextureAO);
		SAFE_DELETE(pMaterial);
	});

	m_Materials.RemoveAll();
	m_Vertices.RemoveAll();
	m_Indices.RemoveAll();
	m_Attributes.RemoveAll();

	SAFE_RELEASE(m_pMesh);
}

//--------------------------------------------------------------------------------------
HRESULT CMesh::Create(const string& strFilePath, const string& strFilename)
{
	Log("Creating mesh from OBJ file with name: %s", strFilename.c_str());

	HRESULT hr = E_FAIL;

	Destroy();

	hr = LoadGeometryFromOBJ(strFilePath, strFilename);

	ASSERT(SUCCEEDED(hr), "Can't load geometry from OBJ: %s", strFilePath + strFilename);

	hr = LoadTextures();

	hr = CreateMesh();

	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CMesh::LoadGeometryFromOBJ(const string& strFilepath, const string& strFileName)
{
	CHAR strMaterialFilename[MAX_PATH] = {0};
	HRESULT hr = S_OK;

	// Create temporary storage for the input data
	CGrowableArray<D3DXVECTOR3> Positions;
	CGrowableArray<D3DXVECTOR2> TexCoords;
	CGrowableArray<D3DXVECTOR3> Normals;

	// Pre-allocate memory for better performance
	Positions.SetSize(2048);
	TexCoords.SetSize(1024);
	Normals.SetSize(1024);
	m_Indices.SetSize(8192);
	m_Attributes.SetSize(512);

	DWORD dwCurSubset = 0;

	// File input - use full path
	string strFullPath = strFilepath + strFileName;
	ifstream InFile(strFullPath.c_str());
	if (!InFile) [[unlikely]]
	{
		ERROR_MESSAGE("Can't open file: %s", strFullPath.c_str());
		return E_FAIL;
	}

	// Optimize material lookup
	typedef std::unordered_map<std::string, DWORD> MaterialMap;
	MaterialMap materialMap;

	CHAR strCommand[256] = {0};
	CHAR strLine[1024] = {0};

	while (InFile.getline(strLine, sizeof(strLine)))
	{
		if (strLine[0] == '\0' || strLine[0] == '#')
			continue;

		// Parse command safely
		if (sscanf_s(strLine, "%255s", strCommand, (unsigned)_countof(strCommand)) < 1)
			continue;

		const char* pData = strLine + strlen(strCommand);

		while (*pData == ' ')
			pData++; // Skip whitespace

		if (strcmp(strCommand, "v") == 0)
		{
			// Vertex Position
			D3DXVECTOR3 vec;
			if (sscanf_s(pData, "%f %f %f", &vec.x, &vec.y, &vec.z) == 3)
			{
				vec.x = -vec.x; // Mirror X
				Positions.Add(vec);
			}
		}
		else if (strcmp(strCommand, "vt") == 0)
		{
			// Vertex TexCoord
			D3DXVECTOR2 tex;
			if (sscanf_s(pData, "%f %f", &tex.x, &tex.y) == 2)
			{
				TexCoords.Add(tex);
			}
		}
		else if (strcmp(strCommand, "vn") == 0)
		{
			// Vertex Normal
			D3DXVECTOR3 norm;
			if (sscanf_s(pData, "%f %f %f", &norm.x, &norm.y, &norm.z) == 3)
			{
				Normals.Add(norm);
			}
		}
		else if (strcmp(strCommand, "f") == 0)
		{
			// Face - process all vertices in the face
			VERTEX vertex;
			vector<DWORD> faceIndices;
			const char* pFaceData = pData;

			while (*pFaceData != '\0')
			{
				ZeroMemory(&vertex, sizeof(VERTEX));

				// Parse vertex indices: position/texcoord/normal
				int iPosition = 0, iTexCoord = 0, iNormal = 0;
				int count = sscanf_s(pFaceData, "%d/%d/%d", &iPosition, &iTexCoord, &iNormal);

				if (count >= 1 && iPosition > 0)
				{
					// Get position (1-based to 0-based)
					vertex.position = Positions[iPosition - 1];

					// Get texture coordinate if available
					if (count >= 2 && iTexCoord > 0 && iTexCoord <= TexCoords.GetSize())
					{
						vertex.texcoord = TexCoords[iTexCoord - 1];
					}

					// Get normal if available
					if (count >= 3 && iNormal > 0 && iNormal <= Normals.GetSize())
					{
						vertex.normal = Normals[iNormal - 1];
					}

					// Add vertex and get index
					DWORD index = AddVertex(iPosition, &vertex);
					if (index == (DWORD)-1)
					{
						InFile.close();
						return E_OUTOFMEMORY;
					}

					faceIndices.push_back(index);
				}

				// Move to next vertex in face
				while (*pFaceData != '\0' && *pFaceData != ' ')
					pFaceData++;

				while (*pFaceData == ' ')
					pFaceData++;
			}

			// Triangulate face if it has more than 3 vertices
			if (faceIndices.size() >= 3) [[unlikely]]
			{
				if (faceIndices.size() == 3)
				{
					m_Indices.Add(faceIndices[0]);
					m_Indices.Add(faceIndices[2]);
					m_Indices.Add(faceIndices[1]);
					m_Attributes.Add(dwCurSubset);
				}
				else
				{
					for (size_t i = 1; i < faceIndices.size() - 1; ++i)
					{
						m_Indices.Add(faceIndices[0]);
						m_Indices.Add(faceIndices[i + 1]);
						m_Indices.Add(faceIndices[i]);
						m_Attributes.Add(dwCurSubset);
					}
				}
			}
		}
		else if (strcmp(strCommand, "mtllib") == 0)
		{
			// Material library
			sscanf_s(pData, "%255s", strMaterialFilename, (unsigned)_countof(strMaterialFilename));
		}
		else if (strcmp(strCommand, "usemtl") == 0)
		{
			// Material
			CHAR strName[MAX_PATH] = {0};
			sscanf_s(pData, "%255s", strName, (unsigned)_countof(strName));

			// Use hash map for faster material lookup
			auto it = materialMap.find(strName);
			if (it != materialMap.end())
			{
				dwCurSubset = it->second;
			}
			else
			{
				// Check existing materials first
				bool bFound = false;
				for (int iMaterial = 0; iMaterial < m_Materials.GetSize(); iMaterial++)
				{
					Material* pCurMaterial = m_Materials.GetAt(iMaterial);
					if (strcmp(pCurMaterial->strName, strName) == 0)
					{
						bFound = true;
						dwCurSubset = iMaterial;
						materialMap[strName] = dwCurSubset;
						break;
					}
				}

				if (!bFound)
				{
					Material* pMaterial = new (std::nothrow) Material();
					if (pMaterial == nullptr)
					{
						InFile.close();
						return E_OUTOFMEMORY;
					}

					InitMaterial(pMaterial);
					strcpy_s(pMaterial->strName, MAX_PATH - 1, strName);

					dwCurSubset = m_Materials.GetSize();
					m_Materials.Add(pMaterial);
					materialMap[strName] = dwCurSubset;

					Log("Added new material: %s", strName);
				}
			}
		}
	}

	// Cleanup
	InFile.close();
	DeleteCache();

	// Load materials if specified
	if (strMaterialFilename[0] != '\0')
	{
		hr = LoadMaterialsFromMTL(strFilepath, strMaterialFilename);

		if (FAILED(hr))
			Log("Warning: Failed to load materials from: %s", strMaterialFilename);
	}

	return S_OK;
}

//--------------------------------------------------------------------------------------
void CMesh::DeleteCache()
{
	// Iterate through all the elements in the cache and subsequent linked lists
	for(int i = 0; i < m_VertexCache.GetSize(); i++) 
	{
		CacheEntry* pEntry = m_VertexCache.GetAt(i);
		while (pEntry != NULL)
		{
			CacheEntry* pNext = pEntry->pNext;
			SAFE_DELETE(pEntry);
			pEntry = pNext;
		}
	}

	m_VertexCache.RemoveAll();
}

//--------------------------------------------------------------------------------------
HRESULT CMesh::LoadTextures()
{
	HRESULT hr = E_FAIL;

	for (int iMaterial = 0; iMaterial < m_Materials.GetSize(); iMaterial++)
	{
		Material* pMaterial = m_Materials.GetAt(iMaterial);

		if (pMaterial->strTextureAlbedo[0]) [[likely]]
		{
			// Avoid loading the same texture twice
			bool bFound = false;

			for (int x = 0; x < iMaterial; x++)
			{
				Material* pCur = m_Materials.GetAt(x);

				if (0 == strcmp(pCur->strTextureAlbedo, pMaterial->strTextureAlbedo))
				{
					bFound = true;
					pMaterial->pTextureAlbedo = pCur->pTextureAlbedo;
					pMaterial->pTextureNormal = pCur->pTextureNormal;
					pMaterial->pTextureRoughness = pCur->pTextureRoughness;
					pMaterial->pTextureMetallic = pCur->pTextureMetallic;
					break;
				}
			}

			// Not found, load the texture
			if (!bFound) [[likely]]
			{
				Log("Loading albedo texture: %s", pMaterial->strTextureAlbedo);
				hr = D3DXCreateTextureFromFile(Device, pMaterial->strTextureAlbedo, &(pMaterial->pTextureAlbedo));
				ASSERT(SUCCEEDED(hr), "Can't load texture from OBJ: %s", pMaterial->strTextureAlbedo);

				if (pMaterial->bHaveNormal)
				{
					Log("Loading normal texture: %s", pMaterial->strTextureNormal);
					hr = D3DXCreateTextureFromFile(Device, pMaterial->strTextureNormal, &(pMaterial->pTextureNormal));
					ASSERT(SUCCEEDED(hr), "Can't load texture from OBJ: %s", pMaterial->strTextureNormal);
				}
				else
				{
					string DummyNormalPath = TEXTURES + (string) "dummy_normal.dds";
					hr = D3DXCreateTextureFromFile(Device, DummyNormalPath.c_str(), &(pMaterial->pTextureNormal));
				}

				if (pMaterial->bHaveRoughness)
				{
					Log("Loading roughness texture: %s", pMaterial->strTextureRoughness);
					hr = D3DXCreateTextureFromFile(Device, pMaterial->strTextureRoughness,
												   &(pMaterial->pTextureRoughness));
					ASSERT(SUCCEEDED(hr), "Can't load texture from OBJ: %s", pMaterial->strTextureRoughness);
				}
				else
				{
					string DummyRoughnessPath = TEXTURES + (string) "dummy_white.dds";
					hr = D3DXCreateTextureFromFile(Device, DummyRoughnessPath.c_str(), &(pMaterial->pTextureRoughness));
				}

				if (pMaterial->bHaveMetallic)
				{
					Log("Loading metallic texture: %s", pMaterial->strTextureMetallic);
					hr = D3DXCreateTextureFromFile(Device, pMaterial->strTextureMetallic,
												   &(pMaterial->pTextureMetallic));
					ASSERT(SUCCEEDED(hr), "Can't load texture from OBJ: %s", pMaterial->strTextureMetallic);
				}
				else
				{
					string DummyMetallicPath = TEXTURES + (string) "dummy_black.dds";
					hr = D3DXCreateTextureFromFile(Device, DummyMetallicPath.c_str(), &(pMaterial->pTextureRoughness));
				}

				if (pMaterial->bHaveAO)
				{
					Log("Loading AO texture: %s", pMaterial->strTextureAO);
					hr = D3DXCreateTextureFromFile(Device, pMaterial->strTextureAO, &(pMaterial->pTextureAO));
					ASSERT(SUCCEEDED(hr), "Can't load texture from OBJ: %s", pMaterial->strTextureAO);
				}
				else
				{
					string DummyAOPath = TEXTURES + (string) "dummy_white.dds";
					hr = D3DXCreateTextureFromFile(Device, DummyAOPath.c_str(), &(pMaterial->pTextureAO));
				}
			}
		}
	}

	return hr;
}

//--------------------------------------------------------------------------------------
DWORD CMesh::AddVertex(UINT hash, VERTEX* pVertex)
{
	// If this vertex doesn't already exist in the Vertices list, create a new entry.
	// Add the index of the vertex to the Indices list.
	bool bFoundInList = false;
	DWORD index = 0;

	// Since it's very slow to check every element in the vertex list, a hashtable stores
	// vertex indices according to the vertex position's index as reported by the OBJ file
	if ((UINT)m_VertexCache.GetSize() > hash)
	{
		CacheEntry* pEntry = m_VertexCache.GetAt(hash);
		while (pEntry != NULL)
		{
			VERTEX* pCacheVertex = m_Vertices.GetData() + pEntry->index;

			// If this vertex is identical to the vertex already in the list, simply
			// point the index buffer to the existing vertex
			if (0 == memcmp(pVertex, pCacheVertex, sizeof(VERTEX)))
			{
				bFoundInList = true;
				index = pEntry->index;
				break;
			}

			pEntry = pEntry->pNext;
		}
	}

	// Vertex was not found in the list. Create a new entry, both within the Vertices list
	// and also within the hashtable cache
	if (!bFoundInList)
	{
		// Add to the Vertices list
		index = m_Vertices.GetSize();
		m_Vertices.Add(*pVertex);

		// Add this to the hashtable
		CacheEntry* pNewEntry = new CacheEntry;
		if (pNewEntry == NULL)
			return (DWORD)-1;

		pNewEntry->index = index;
		pNewEntry->pNext = NULL;

		// Grow the cache if needed
		while ((UINT)m_VertexCache.GetSize() <= hash)
		{
			m_VertexCache.Add(NULL);
		}

		// Add to the end of the linked list
		CacheEntry* pCurEntry = m_VertexCache.GetAt(hash);
		if (pCurEntry == NULL)
		{
			// This is the head element
			m_VertexCache.SetAt(hash, pNewEntry);
		}
		else
		{
			// Find the tail
			while (pCurEntry->pNext != NULL)
			{
				pCurEntry = pCurEntry->pNext;
			}

			pCurEntry->pNext = pNewEntry;
		}
	}

	return index;
}

//--------------------------------------------------------------------------------------
HRESULT CMesh::LoadMaterialsFromMTL(const string& strFilePath, const string& strFileName)
{
	Log("Loading material from MTL file with name: %s", strFileName.c_str());

	// File input
	CHAR strCommand[256] = {0};

	ifstream InFile((string)strFilePath + strFileName);

	if (!InFile)
	{
		ERROR_MESSAGE("Can't open ifstream in create mesh from OBJ (MTL Loading)");
		Log("Can't open ifstream in create mesh from OBJ (MTL Loading): %s", string(strFilePath + strFileName).c_str());
		return E_FAIL;
	}

	Material* pMaterial = NULL;

	for (;;)
	{
		InFile >> strCommand;
		if (!InFile)
			break;

		if (0 == strcmp(strCommand, "newmtl"))
		{
			// Switching active materials
			CHAR strName[MAX_PATH] = {0};
			InFile >> strName;

			pMaterial = NULL;
			for (int i = 0; i < m_Materials.GetSize(); i++)
			{
				Material* pCurMaterial = m_Materials.GetAt(i);
				if (0 == strcmp(pCurMaterial->strName, strName))
				{
					pMaterial = pCurMaterial;
					break;
				}
			}
		}

		// The rest of the commands rely on an active material
		if (pMaterial == NULL)
			continue;

		CHAR strTextureBuffer[MAX_PATH];

		if (0 == strcmp(strCommand, "#"))
		{
			// Comment
		}
		else if (0 == strcmp(strCommand, "map_Kd"))
		{
			InFile >> strTextureBuffer;
			strcat_s(pMaterial->strTextureAlbedo, TEXTURES);
			strcat_s(pMaterial->strTextureAlbedo, strTextureBuffer);
		}
		else if (0 == strcmp(strCommand, "map_Bump"))
		{
			InFile >> strTextureBuffer;
			strcat_s(pMaterial->strTextureNormal, TEXTURES);
			strcat_s(pMaterial->strTextureNormal, strTextureBuffer);
			pMaterial->bHaveNormal = true;
		}
		else if (0 == strcmp(strCommand, "map_Ns"))
		{
			InFile >> strTextureBuffer;
			strcat_s(pMaterial->strTextureRoughness, TEXTURES);
			strcat_s(pMaterial->strTextureRoughness, strTextureBuffer);
			pMaterial->bHaveRoughness = true;
		}
		else if (0 == strcmp(strCommand, "refl"))
		{
			InFile >> strTextureBuffer;
			strcat_s(pMaterial->strTextureMetallic, TEXTURES);
			strcat_s(pMaterial->strTextureMetallic, strTextureBuffer);
			pMaterial->bHaveMetallic = true;
		}
		else if (0 == strcmp(strCommand, "ao"))
		{
			InFile >> strTextureBuffer;
			strcat_s(pMaterial->strTextureAO, TEXTURES);
			strcat_s(pMaterial->strTextureAO, strTextureBuffer);
			pMaterial->bHaveAO = true;
		}
		else
		{
			// Unimplemented or unrecognized command
		}

		InFile.ignore(1000, '\n');
	}

	InFile.close();

	return S_OK;
}

//--------------------------------------------------------------------------------------
HRESULT CMesh::CreateMesh()
{
	HRESULT hr = E_FAIL;

	Log("Creating mesh from readed data");

	ID3DXMesh* pMesh = NULL;

	hr = D3DXCreateMesh( m_Indices.GetSize() / 3,			// Number of faces is indices count divided by 3 (each triangle consists of 3 indices).
						 m_Vertices.GetSize(),				// Total number of vertices to be included in the mesh.
						 D3DXMESH_MANAGED | D3DXMESH_32BIT, // Specify managed memory usage and indicate the use of 32-bit indices.
						 VERTEX_DECLARATION,				// The vertex declaration structure that defines the format of the vertex data.
						 Device,							// The Direct3D device used for rendering.
						 &pMesh );							// Output parameter that will receive the created mesh.


	ASSERT(SUCCEEDED(hr), "Can't create mesh from OBJ: %s", strFilePath + strFilename);

	VERTEX* pVertex; // Pointer to access the vertex data within the mesh.
	hr = pMesh->LockVertexBuffer(0, (void**)&pVertex); // Lock the vertex buffer to allow writing of vertex data.
	ASSERT(SUCCEEDED(hr), "Can't Lock Vertex Buffer in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to check lock success.
	memcpy(pVertex, m_Vertices.GetData(), m_Vertices.GetSize() * sizeof(VERTEX)); // Copy the vertex data from the source to the buffer.
	pMesh->UnlockVertexBuffer(); // Unlock the vertex buffer after copying data.
	m_Vertices.RemoveAll(); // Clear the original vertex data array to free up resources.

	DWORD* pIndex; // Pointer for accessing the index data.
	hr = pMesh->LockIndexBuffer(0, (void**)&pIndex); // Lock the index buffer for writing.
	ASSERT(SUCCEEDED(hr), "Can't Lock Index Buffer in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to ensure lock succeeded.
	memcpy(pIndex, m_Indices.GetData(), m_Indices.GetSize() * sizeof(DWORD)); // Copy the index data.
	pMesh->UnlockIndexBuffer(); // Unlock the index buffer after copying.
	m_Indices.RemoveAll(); // Clear the original index data array to free up resources.

	DWORD* pSubset; // Pointer for accessing the attribute data.
	hr = pMesh->LockAttributeBuffer(0, &pSubset); // Lock the attribute buffer for writing.
	ASSERT(SUCCEEDED(hr), "Can't Lock Attribute Buffer in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to ensure lock succeeded.
	memcpy(pSubset, m_Attributes.GetData(), m_Attributes.GetSize() * sizeof(DWORD)); // Copy attribute data associated with the mesh.
	pMesh->UnlockAttributeBuffer(); // Unlock the attribute buffer after copying.
	m_Attributes.RemoveAll(); // Clear the original attribute data array to free up resources.

	Log("Raw mesh created");

	Log("Create adjacency for mesh");

	// Reorder the vertices according to subset and optimize the mesh for the graphics card's vertex cache.
	// This improves performance by allowing efficient access to vertex data during rendering.
	DWORD* aAdjacency = new DWORD[pMesh->GetNumFaces() * 3L]; // Allocate an array to store adjacency information for the faces.

	ASSERT((aAdjacency != NULL), "Can't Generate Adjacency in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to ensure allocation was successful.

	hr = pMesh->GenerateAdjacency(1e-6f, aAdjacency); // Generate adjacency information for the mesh.

	ASSERT(SUCCEEDED(hr), "Can't Generate Adjacency in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to check for success of adjacency generation.

	Log("Optimize mesh");

	hr = pMesh->OptimizeInplace(D3DXMESHOPT_ATTRSORT | D3DXMESHOPT_VERTEXCACHE | D3DXMESHOPT_COMPACT | D3DXMESHOPT_DEVICEINDEPENDENT, 
								aAdjacency, 
								NULL, 
								NULL, 
								NULL); // Optimize the mesh in-place for better rendering performance.

	ASSERT(SUCCEEDED(hr), "Can't Optimize Inplace in create mesh from OBJ: %s", strFilePath + strFilename); // Assert to check for successful optimization.

	SAFE_DELETE_ARRAY(aAdjacency); // Clean up the adjacency array to prevent memory leaks.

	Log("Create tangents for mesh"); // Log that the tangent generation process for the mesh is starting.

	// Calculate tangent vectors for the mesh. Tangents are essential for proper normal mapping in shader programs.
	// The parameters below define how the tangents will be computed.
	// - D3DDECLUSAGE_TEXCOORD and 0: Specify the texture coordinate stream to use.
	// - D3DX_DEFAULT: Use default settings for tangent computation.
	// - D3DDECLUSAGE_TANGENT and 0: Define where to store the computed tangents in the vertex buffer.
	// - D3DDECLUSAGE_NORMAL and 0: Specify the normal stream to use for tangent computation.
	// - NULL
	// - NULL: Placeholder for additional output parameters.
	// - 0.01f, 0.25f, 0.01f: Parameters influencing the tangent computation behavior (e.g., precision).
	D3DXComputeTangentFrameEx(	pMesh,
								D3DDECLUSAGE_TEXCOORD,
								0,
								D3DX_DEFAULT,
								0,
								D3DDECLUSAGE_TANGENT,
								0,
								D3DDECLUSAGE_NORMAL,
								0,
								NULL,
								NULL,
								0.01f,
								0.25f,
								0.01f,
								&m_pMesh,
								NULL  );

	Log("Mesh successfully created!"); // Log that the mesh creation process has completed successfully.

	Log("Mesh statistic:");									// Log header for mesh statistics output.
	Log("Vertices count: %d", m_pMesh->GetNumVertices()); // Output the number of vertices in the mesh.
	Log("Faces count: %d", m_pMesh->GetNumFaces());			// Output the number of faces (triangles) in the mesh.
	Log("Materials count: %d", m_Materials.GetSize());	  // Output the number of materials associated with the mesh.
	Log("\n"); // Print a newline for better readability in the output logs.

	return S_OK; // Return success status to indicate that the mesh creation operation completed without errors.
}

//--------------------------------------------------------------------------------------
void CMesh::DrawSubsets()
{
	// Meshes are divided into subsets, one for each material. Render them in a loop
	for (UINT iSubset = 0; iSubset < GetNumMaterials(); iSubset++)
	{
		if (m_pMesh)
		{
			Material* pMaterial = GetMaterial(iSubset);

			Device->SetTexture(0, pMaterial->pTextureAlbedo);
			RenderBackend->SetTextureFiltration(0, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR);

			Device->SetTexture(1, pMaterial->pTextureNormal);
			RenderBackend->SetTextureFiltration(1, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR);

			Device->SetTexture(2, pMaterial->pTextureRoughness);
			RenderBackend->SetTextureFiltration(2, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR);

			Device->SetTexture(3, pMaterial->pTextureMetallic);
			RenderBackend->SetTextureFiltration(3, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR);

			Device->SetTexture(4, pMaterial->pTextureAO);
			RenderBackend->SetTextureFiltration(4, D3DTEXF_ANISOTROPIC, D3DTEXF_ANISOTROPIC, D3DTEXF_LINEAR);

			m_pMesh->DrawSubset(iSubset);
		}
	}
}

//--------------------------------------------------------------------------------------
void CMesh::InitMaterial(Material* pMaterial)
{
	ZeroMemory(pMaterial, sizeof(Material));

	pMaterial->pTextureAlbedo = NULL;
	pMaterial->pTextureNormal = NULL;
	pMaterial->pTextureRoughness = NULL;
	pMaterial->pTextureMetallic = NULL;
	pMaterial->pTextureAO = NULL;

	pMaterial->bHaveMetallic = false;
	pMaterial->bHaveRoughness = false;
	pMaterial->bHaveNormal = false;
	pMaterial->bHaveAO = false;
}
