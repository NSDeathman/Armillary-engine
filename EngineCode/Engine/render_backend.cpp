///////////////////////////////////////////////////////////////
// Created: 16.01.2025
// Author: NS_Deathman
// Renderer backend realization
///////////////////////////////////////////////////////////////
#pragma once
///////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "render.h"
#include "render_backend.h"
///////////////////////////////////////////////////////////////
CBackend::CBackend()
{
	colorwrite_mask = NULL;
	cull_mode = CULL_NONE;
	zwrite = FALSE;
}

void CBackend::set_ColorWriteEnable(u32 _mask)
{
	if (colorwrite_mask != _mask)
	{
		colorwrite_mask = _mask;
		Render->m_pDirect3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE, _mask);
		Render->m_pDirect3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE1, _mask);
		Render->m_pDirect3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE2, _mask);
		Render->m_pDirect3dDevice->SetRenderState(D3DRS_COLORWRITEENABLE3, _mask);
	}
}

void CBackend::set_ZWriteEnable(bool write_state)
{
	if (zwrite != write_state)
	{
		zwrite = write_state;
		Render->m_pDirect3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, write_state);
	}
}

void CBackend::set_CullMode(u32 _mode)
{
	Render->m_pDirect3dDevice->SetRenderState(D3DRS_CULLMODE, _mode);
}

void CBackend::set_FillMode(u32 _mode)
{
	Render->m_pDirect3dDevice->SetRenderState(D3DRS_FILLMODE, _mode);
}

void CBackend::set_anisotropy_filtering(int max_anisothropy)
{
	for (u32 i = 0; i < Render->MaxSimultaneousTextures; i++)
		Render->m_pDirect3dDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, max_anisothropy);
}

/*
void CBackend::enable_anisotropy_filtering()
{
	for (u32 i = 0; i < Render->MaxSimultaneousTextures; i++)
		Render->m_pDirect3dDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, psAnisotropic);
}
*/

void CBackend::disable_anisotropy_filtering()
{
	for (u32 i = 0; i < Render->MaxSimultaneousTextures; i++)
		Render->m_pDirect3dDevice->SetSamplerState(i, D3DSAMP_MAXANISOTROPY, 1);
}
///////////////////////////////////////////////////////////////
