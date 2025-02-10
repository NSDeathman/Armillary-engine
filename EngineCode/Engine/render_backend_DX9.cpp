///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// Renderer backend realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "render_DX9.h"
#include "render_backend_DX9.h"
///////////////////////////////////////////////////////////////
CRenderBackendDX9::CRenderBackendDX9()
{
	colorwrite_mask = NULL;
	cull_mode = CULL_NONE;
	zwrite = FALSE;
}

void CRenderBackendDX9::set_ColorWriteEnable(u32 _mask)
{
	if (colorwrite_mask != _mask)
	{
		colorwrite_mask = _mask;
		Device->SetRenderState(D3DRS_COLORWRITEENABLE, _mask);
		Device->SetRenderState(D3DRS_COLORWRITEENABLE1, _mask);
		Device->SetRenderState(D3DRS_COLORWRITEENABLE2, _mask);
		Device->SetRenderState(D3DRS_COLORWRITEENABLE3, _mask);
	}
}

void CRenderBackendDX9::set_ZWriteEnable(bool write_state)
{
	if (zwrite != write_state)
	{
		zwrite = write_state;
		Device->SetRenderState(D3DRS_ZWRITEENABLE, write_state);
	}
}

void CRenderBackendDX9::set_CullMode(u32 _mode)
{
	Device->SetRenderState(D3DRS_CULLMODE, _mode);
}

void CRenderBackendDX9::set_FillMode(u32 _mode)
{
	Device->SetRenderState(D3DRS_FILLMODE, _mode);
}

void CRenderBackendDX9::set_anisotropy_filtering(int max_anisothropy)
{
	for (u32 i = 0; i < Render->MaxSimultaneousTextures; i++)
		Device->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, max_anisothropy);
}

/*
void CRenderBackendDX9::enable_anisotropy_filtering()
{
	for (u32 i = 0; i < Render->MaxSimultaneousTextures; i++)
		Device->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, psAnisotropic);
}
*/

void CRenderBackendDX9::disable_anisotropy_filtering()
{
	for (u32 i = 0; i < Render->MaxSimultaneousTextures; i++)
		Device->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 1);
}
///////////////////////////////////////////////////////////////
