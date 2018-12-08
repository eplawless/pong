#pragma once

#include <d3d11.h>

#include "../utility.h"

class Texture
{
public: // methods
	bool Initialize(ID3D11Device *pDevice, WCHAR *filename);
	void Shutdown();

	ID3D11ShaderResourceView *GetTexture() { return m_pShaderResourceView.Get(); }

private: // members
	ScopedD3DPointer<ID3D11Resource> m_pTexture;
	ScopedD3DPointer<ID3D11ShaderResourceView> m_pShaderResourceView;
};