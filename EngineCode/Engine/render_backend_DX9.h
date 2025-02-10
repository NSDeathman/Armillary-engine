///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// Renderer backend realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
///////////////////////////////////////////////////////////////
class CRenderBackendDX9
{
private:
	u32 colorwrite_mask;
	u32 cull_mode;
	bool zwrite;

public:
	enum
	{
		CULL_CCW = D3DCULL_CCW,
		CULL_CW = D3DCULL_CW,
		CULL_NONE = D3DCULL_NONE
	};

	enum
	{
		FILL_POINT = D3DFILL_POINT,
		FILL_WIREFRAME = D3DFILL_WIREFRAME,
		FILL_SOLID = D3DFILL_SOLID
	};

	void set_ColorWriteEnable(u32 _mask);
	void set_ZWriteEnable(bool write_state);
	void set_CullMode(u32 _mode);
	void set_FillMode(u32 _mode);
	void set_anisotropy_filtering(int max_anisothropy);
	void enable_anisotropy_filtering();
	void disable_anisotropy_filtering();

	CRenderBackendDX9();
	~CRenderBackendDX9() = default;
};
///////////////////////////////////////////////////////////////
extern CRenderBackendDX9* RenderBackend;
///////////////////////////////////////////////////////////////
